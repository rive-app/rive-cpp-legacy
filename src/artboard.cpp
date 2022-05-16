#include "rive/artboard.hpp"
#include "rive/backboard.hpp"
#include "rive/animation/animation.hpp"
#include "rive/animation/linear_animation_instance.hpp"
#include "rive/dependency_sorter.hpp"
#include "rive/draw_rules.hpp"
#include "rive/draw_target.hpp"
#include "rive/draw_target_placement.hpp"
#include "rive/drawable.hpp"
#include "rive/animation/keyed_object.hpp"
#include "rive/node.hpp"
#include "rive/renderer.hpp"
#include "rive/shapes/paint/shape_paint.hpp"
#include "rive/importers/import_stack.hpp"
#include "rive/importers/backboard_importer.hpp"
#include "rive/nested_artboard.hpp"
#include "rive/animation/state_machine_instance.hpp"

#include <stack>
#include <unordered_map>

using namespace rive;

Artboard::~Artboard() {
    for (auto object : m_Objects) {
        // First object is artboard
        if (object == this) {
            continue;
        }
        delete object;
    }

    // Instances reference back to the original artboard's animations and state
    // machines, so don't delete them here, they'll get cleaned up when the
    // source is deleted.
    // TODO: move this logic into ArtboardInstance destructor???
    if (!m_IsInstance) {
        for (auto object : m_Animations) {
            delete object;
        }
        for (auto object : m_StateMachines) {
            delete object;
        }
    }
}

static bool canContinue(StatusCode code) {
    // We currently only cease loading on invalid object.
    return code != StatusCode::InvalidObject;
}

StatusCode Artboard::initialize() {
    StatusCode code;

    m_BackgroundPath = makeCommandPath(PathSpace::Neither);
    m_ClipPath = makeCommandPath(PathSpace::Neither);

    // onAddedDirty guarantees that all objects are now available so they can be
    // looked up by index/id. This is where nodes find their parents, but they
    // can't assume that their parent's parent will have resolved yet.
    for (auto object : m_Objects) {
        if (object == nullptr) {
            // objects can be null if they were not understood by this runtime.
            continue;
        }
        if (!canContinue(code = object->onAddedDirty(this))) {
            return code;
        }
    }

    // Animations and StateMachines initialize only once on the source/origin
    // Artboard. Instances will hold references to the original Animations and StateMachines, so
    // running this code for instances will effectively initialize them twice. This can lead to
    // unpredictable behaviour. One such example was that resolved objects like event inputs were
    // being added to lists twice.
    if (!isInstance()) {
        for (auto object : m_Animations) {
            if (!canContinue(code = object->onAddedDirty(this))) {
                return code;
            }
        }

        for (auto object : m_StateMachines) {
            if (!canContinue(code = object->onAddedDirty(this))) {
                return code;
            }
        }
    }

    // Store a map of the drawRules to make it easier to lookup the matching
    // rule for a transform component.
    std::unordered_map<Core*, DrawRules*> componentDrawRules;

    // onAddedClean is called when all individually referenced components have
    // been found and so components can look at other components' references and
    // assume that they have resolved too. This is where the whole hierarchy is
    // linked up and we can traverse it to find other references (my parent's
    // parent should be type X can be checked now).
    for (auto object : m_Objects) {
        if (object == nullptr) {
            continue;
        }
        if (!canContinue(code = object->onAddedClean(this))) {
            return code;
        }
        switch (object->coreType()) {
            case DrawRulesBase::typeKey: {
                DrawRules* rules = reinterpret_cast<DrawRules*>(object);
                Core* component = resolve(rules->parentId());
                if (component != nullptr) {
                    componentDrawRules[component] = rules;
                } else {
                    fprintf(stderr,
                            "Artboard::initialize - Draw rule targets missing "
                            "component width id %d\n",
                            rules->parentId());
                }
                break;
            }
            case NestedArtboardBase::typeKey:
                m_NestedArtboards.push_back(object->as<NestedArtboard>());
                break;
        }
    }

    if (!isInstance()) {
        for (auto object : m_Animations) {
            if (!canContinue(code = object->onAddedClean(this))) {
                return code;
            }
        }

        for (auto object : m_StateMachines) {
            if (!canContinue(code = object->onAddedClean(this))) {
                return code;
            }
        }
    }

    // Multi-level references have been built up, now we can
    // actually mark what's dependent on what.
    for (auto object : m_Objects) {
        if (object == nullptr) {
            continue;
        }
        if (object->is<Component>()) {
            object->as<Component>()->buildDependencies();
        }
        if (object->is<Drawable>()) {
            Drawable* drawable = object->as<Drawable>();
            m_Drawables.push_back(drawable);

            for (ContainerComponent* parent = drawable; parent != nullptr;
                 parent = parent->parent()) {
                auto itr = componentDrawRules.find(parent);
                if (itr != componentDrawRules.end()) {
                    drawable->flattenedDrawRules = itr->second;
                    break;
                }
            }
        }
    }

    sortDependencies();

    DrawTarget root;
    // Build up the draw order. Look for draw targets and build
    // their dependencies.
    for (auto object : m_Objects) {
        if (object == nullptr) {
            continue;
        }
        if (object->is<DrawTarget>()) {
            DrawTarget* target = object->as<DrawTarget>();
            root.addDependent(target);

            auto dependentRules = target->drawable()->flattenedDrawRules;
            if (dependentRules != nullptr) {
                // Because we don't store targets on rules, we need
                // to find the targets that belong to this rule
                // here.
                for (auto object : m_Objects) {
                    if (object != nullptr && object->is<DrawTarget>()) {
                        DrawTarget* dependentTarget = object->as<DrawTarget>();
                        if (dependentTarget->parent() == dependentRules) {
                            dependentTarget->addDependent(target);
                        }
                    }
                }
            }
        }
    }
    DependencySorter sorter;
    std::vector<Component*> drawTargetOrder;
    sorter.sort(&root, drawTargetOrder);
    auto itr = drawTargetOrder.begin();
    itr++;
    while (itr != drawTargetOrder.end()) {
        m_DrawTargets.push_back(reinterpret_cast<DrawTarget*>(*itr++));
    }

    return StatusCode::Ok;
}

void Artboard::sortDrawOrder() {
    for (auto target : m_DrawTargets) {
        target->first = target->last = nullptr;
    }

    m_FirstDrawable = nullptr;
    Drawable* lastDrawable = nullptr;
    for (auto drawable : m_Drawables) {
        auto rules = drawable->flattenedDrawRules;
        if (rules != nullptr && rules->activeTarget() != nullptr) {

            auto target = rules->activeTarget();
            if (target->first == nullptr) {
                target->first = target->last = drawable;
                drawable->prev = drawable->next = nullptr;
            } else {
                target->last->next = drawable;
                drawable->prev = target->last;
                target->last = drawable;
                drawable->next = nullptr;
            }
        } else {
            drawable->prev = lastDrawable;
            drawable->next = nullptr;
            if (lastDrawable == nullptr) {
                lastDrawable = m_FirstDrawable = drawable;
            } else {
                lastDrawable->next = drawable;
                lastDrawable = drawable;
            }
        }
    }

    for (auto rule : m_DrawTargets) {
        if (rule->first == nullptr) {
            continue;
        }
        auto targetDrawable = rule->drawable();
        switch (rule->placement()) {
            case DrawTargetPlacement::before: {
                if (targetDrawable->prev != nullptr) {
                    targetDrawable->prev->next = rule->first;
                    rule->first->prev = targetDrawable->prev;
                }
                if (targetDrawable == m_FirstDrawable) {
                    m_FirstDrawable = rule->first;
                }
                targetDrawable->prev = rule->last;
                rule->last->next = targetDrawable;
                break;
            }
            case DrawTargetPlacement::after: {
                if (targetDrawable->next != nullptr) {
                    targetDrawable->next->prev = rule->last;
                    rule->last->next = targetDrawable->next;
                }
                if (targetDrawable == lastDrawable) {
                    lastDrawable = rule->last;
                }
                targetDrawable->next = rule->first;
                rule->first->prev = targetDrawable;
                break;
            }
        }
    }

    m_FirstDrawable = lastDrawable;
}

void Artboard::sortDependencies() {
    DependencySorter sorter;
    sorter.sort(this, m_DependencyOrder);
    unsigned int graphOrder = 0;
    for (auto component : m_DependencyOrder) {
        component->m_GraphOrder = graphOrder++;
    }
    m_Dirt |= ComponentDirt::Components;
}

void Artboard::addObject(Core* object) { m_Objects.push_back(object); }

void Artboard::addAnimation(LinearAnimation* object) { m_Animations.push_back(object); }

void Artboard::addStateMachine(StateMachine* object) { m_StateMachines.push_back(object); }

void Artboard::addNestedArtboard(NestedArtboard* artboard) {
    m_NestedArtboards.push_back(artboard);
}

Core* Artboard::resolve(uint32_t id) const {
    if (id >= static_cast<int>(m_Objects.size())) {
        return nullptr;
    }
    return m_Objects[id];
}

uint32_t Artboard::idOf(Core* object) const {
    auto it = std::find(m_Objects.begin(), m_Objects.end(), object);

    if (it != m_Objects.end()) {
        return it - m_Objects.begin();
    } else {
        return 0;
    }
}

void Artboard::onComponentDirty(Component* component) {
    m_Dirt |= ComponentDirt::Components;

    /// If the order of the component is less than the current dirt
    /// depth, update the dirt depth so that the update loop can break
    /// out early and re-run (something up the tree is dirty).
    if (component->graphOrder() < m_DirtDepth) {
        m_DirtDepth = component->graphOrder();
    }
}

void Artboard::onDirty(ComponentDirt dirt) { m_Dirt |= ComponentDirt::Components; }

void Artboard::update(ComponentDirt value) {
    if (hasDirt(value, ComponentDirt::DrawOrder)) {
        sortDrawOrder();
    }
    if (hasDirt(value, ComponentDirt::Path)) {
        m_ClipPath->reset();
        if (m_FrameOrigin) {
            m_ClipPath->addRect(0.0f, 0.0f, width(), height());
        } else {
            m_ClipPath->addRect(-width() * originX(), -height() * originY(), width(), height());
        }
        m_BackgroundPath->addRect(-width() * originX(), -height() * originY(), width(), height());
    }
}

bool Artboard::updateComponents() {
    if (hasDirt(ComponentDirt::Components)) {
        const int maxSteps = 100;
        int step = 0;
        auto count = m_DependencyOrder.size();
        while (hasDirt(ComponentDirt::Components) && step < maxSteps) {
            m_Dirt = m_Dirt & ~ComponentDirt::Components;

            // Track dirt depth here so that if something else marks
            // dirty, we restart.
            for (unsigned int i = 0; i < count; i++) {
                auto component = m_DependencyOrder[i];
                m_DirtDepth = i;
                auto d = component->m_Dirt;
                if (d == ComponentDirt::None) {
                    continue;
                }
                component->m_Dirt = ComponentDirt::None;
                component->update(d);

                // If the update changed the dirt depth by adding dirt
                // to something before us (in the DAG), early out and
                // re-run the update.
                if (m_DirtDepth < i) {
                    // We put this in here just to know if we need to
                    // keep this around...
                    assert(false);
                    break;
                }
            }
            step++;
        }
        return true;
    }
    return false;
}

bool Artboard::advance(double elapsedSeconds) {
    for (auto nestedArtboard : m_NestedArtboards) {
        nestedArtboard->advance(elapsedSeconds);
    }
    return updateComponents();
}

Core* Artboard::hitTest(HitInfo* hinfo, const Mat2D* xform) {
    if (clip()) {
        // TODO: can we get the rawpath for the clip?
    }

    auto mx = xform ? *xform : Mat2D();
    if (m_FrameOrigin) {
        mx *= Mat2D::fromTranslate(width() * originX(), height() * originY());
    }

    Drawable* last = m_FirstDrawable;
    if (last) {
        // walk to the end, so we can visit in reverse-order
        while (last->prev) {
            last = last->prev;
        }
    }
    for (auto drawable = last; drawable; drawable = drawable->next) {
        if (drawable->isHidden()) {
            continue;
        }
        if (auto c = drawable->hitTest(hinfo, mx)) {
            return c;
        }
    }

    // TODO: should we hit-test the background?

    return nullptr;
}

void Artboard::draw(Renderer* renderer, DrawOption option) {
    renderer->save();
    if (clip()) {
        renderer->clipPath(m_ClipPath->renderPath());
    }

    if (m_FrameOrigin) {
        Mat2D artboardTransform;
        artboardTransform[4] = width() * originX();
        artboardTransform[5] = height() * originY();
        renderer->transform(artboardTransform);
    }

    if (option != DrawOption::kHideBG) {
        for (auto shapePaint : m_ShapePaints) {
            shapePaint->draw(renderer, backgroundPath());
        }
    }

    if (option != DrawOption::kHideFG) {
        for (auto drawable = m_FirstDrawable; drawable != nullptr; drawable = drawable->prev) {
            if (drawable->isHidden()) {
                continue;
            }
            drawable->draw(renderer);
        }
    }

    renderer->restore();
}

AABB Artboard::bounds() const { return AABB(0.0f, 0.0f, width(), height()); }

bool Artboard::isTranslucent(const LinearAnimation* anim) const {
    // For now we're conservative/lazy -- if we see that any of our paints are
    // animated we assume that might make it non-opaque, so we early out
    for (const auto& obj : anim->m_KeyedObjects) {
        const auto ptr = this->resolve(obj->objectId());
        for (const auto sp : m_ShapePaints) {
            if (ptr == sp) {
                return true;
            }
        }
    }

    // If we get here, we have no animations, so just check our paints for
    // opacity

    for (const auto sp : m_ShapePaints) {
        if (!sp->isTranslucent()) {
            return false; // one opaque fill is sufficient to be opaque
        }
    }
    return true;
}

bool Artboard::isTranslucent(const LinearAnimationInstance* inst) const {
    return this->isTranslucent(inst->animation());
}

std::string Artboard::animationNameAt(size_t index) const {
    auto la = this->animation(index);
    return la ? la->name() : nullptr;
}

std::string Artboard::stateMachineNameAt(size_t index) const {
    auto sm = this->stateMachine(index);
    return sm ? sm->name() : nullptr;
}

LinearAnimation* Artboard::firstAnimation() const {
    if (m_Animations.empty()) {
        return nullptr;
    }
    return m_Animations.front();
}

LinearAnimation* Artboard::animation(const std::string& name) const {
    for (auto animation : m_Animations) {
        if (animation->name() == name) {
            return animation;
        }
    }
    return nullptr;
}

LinearAnimation* Artboard::animation(size_t index) const {
    if (index >= m_Animations.size()) {
        return nullptr;
    }
    return m_Animations[index];
}

StateMachine* Artboard::firstStateMachine() const {
    if (m_StateMachines.empty()) {
        return nullptr;
    }
    return m_StateMachines.front();
}

StateMachine* Artboard::stateMachine(const std::string& name) const {
    for (auto machine : m_StateMachines) {
        if (machine->name() == name) {
            return machine;
        }
    }
    return nullptr;
}

StateMachine* Artboard::stateMachine(size_t index) const {
    if (index >= m_StateMachines.size()) {
        return nullptr;
    }
    return m_StateMachines[index];
}

StateMachine* Artboard::defaultStateMachine() const {
    if (defaultStateMachineId() > m_StateMachines.size()) {
        return nullptr;
    }
    return m_StateMachines[defaultStateMachineId()];
}

std::unique_ptr<ArtboardInstance> Artboard::instance() const {
    std::unique_ptr<ArtboardInstance> artboardClone(new ArtboardInstance);
    artboardClone->copy(*this);

    artboardClone->m_Factory = m_Factory;
    artboardClone->m_FrameOrigin = m_FrameOrigin;
    artboardClone->m_IsInstance = true;

    std::vector<Core*>& cloneObjects = artboardClone->m_Objects;
    cloneObjects.push_back(artboardClone.get());

    if (!m_Objects.empty()) {
        // Skip first object (artboard).
        auto itr = m_Objects.begin();
        while (++itr != m_Objects.end()) {
            auto object = *itr;
            cloneObjects.push_back(object == nullptr ? nullptr : object->clone());
        }
    }

    for (auto animation : m_Animations) {
        artboardClone->m_Animations.push_back(animation);
    }
    for (auto stateMachine : m_StateMachines) {
        artboardClone->m_StateMachines.push_back(stateMachine);
    }

    if (artboardClone->initialize() != StatusCode::Ok) {
        artboardClone = nullptr;
    }

    assert(artboardClone->isInstance());
    return artboardClone;
}

void Artboard::frameOrigin(bool value) {
    if (value == m_FrameOrigin) {
        return;
    }
    m_FrameOrigin = value;
    addDirt(ComponentDirt::Path);
}

StatusCode Artboard::import(ImportStack& importStack) {
    auto backboardImporter = importStack.latest<BackboardImporter>(Backboard::typeKey);
    if (backboardImporter == nullptr) {
        return StatusCode::MissingObject;
    }

    StatusCode result = Super::import(importStack);
    if (result == StatusCode::Ok) {
        backboardImporter->addArtboard(this);
    } else {
        backboardImporter->addMissingArtboard();
    }
    return result;
}

////////// ArtboardInstance

#include "rive/animation/linear_animation_instance.hpp"
#include "rive/animation/state_machine_instance.hpp"

std::unique_ptr<LinearAnimationInstance> ArtboardInstance::animationAt(size_t index) {
    auto la = this->animation(index);
    return la ? std::make_unique<LinearAnimationInstance>(la, this) : nullptr;
}

std::unique_ptr<LinearAnimationInstance> ArtboardInstance::animationNamed(const std::string& name) {
    auto la = this->animation(name);
    return la ? std::make_unique<LinearAnimationInstance>(la, this) : nullptr;
}

std::unique_ptr<StateMachineInstance> ArtboardInstance::stateMachineAt(size_t index) {
    auto sm = this->stateMachine(index);
    return sm ? std::make_unique<StateMachineInstance>(sm, this) : nullptr;
}

std::unique_ptr<StateMachineInstance> ArtboardInstance::stateMachineNamed(const std::string& name) {
    auto sm = this->stateMachine(name);
    return sm ? std::make_unique<StateMachineInstance>(sm, this) : nullptr;
}
