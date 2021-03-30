#include "artboard.hpp"
#include "animation/animation.hpp"
#include "dependency_sorter.hpp"
#include "draw_rules.hpp"
#include "draw_target.hpp"
#include "draw_target_placement.hpp"
#include "drawable.hpp"
#include "node.hpp"
#include "renderer.hpp"
#include "shapes/paint/shape_paint.hpp"
#include <unordered_map>

using namespace rive;

Artboard::~Artboard()
{
	for (auto object : m_Objects)
	{
		// First object is artboard
		if (object == this)
		{
			continue;
		}
		delete object;
	}
	for (auto object : m_Animations)
	{
		delete object;
	}
	delete m_ClipPath;
	delete m_BackgroundPath;
}

static bool canContinue(StatusCode code) 
{
	// We currently only cease loading on invalid object.
	return code != StatusCode::InvalidObject;
}

StatusCode Artboard::initialize()
{
	StatusCode code;

	m_BackgroundPath = makeCommandPath(PathSpace::Neither);
	m_ClipPath = makeCommandPath(PathSpace::Neither);

	// onAddedDirty guarantees that all objects are now available so they can be
	// looked up by index/id. This is where nodes find their parents, but they
	// can't assume that their parent's parent will have resolved yet.
	for (auto object : m_Objects)
	{
		if(object == nullptr) 
		{
			// objects can be null if they were not understood by this runtime.
			continue;
		}
		if (!canContinue(code = object->onAddedDirty(this)))
		{
			return code;
		}
	}

	for (auto object : m_Animations)
	{
		if (!canContinue(code = object->onAddedDirty(this)))
		{
			return code;
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
	for (auto object : m_Objects)
	{
		if(object == nullptr) 
		{
			continue;
		}
		if (!canContinue(code = object->onAddedClean(this)))
		{
			return code;
		}
		if (object->is<DrawRules>())
		{
			DrawRules* rules = reinterpret_cast<DrawRules*>(object);
			Core* component = resolve(rules->parentId());
			if (component != nullptr)
			{
				componentDrawRules[component] = rules;
			}
			else 
			{
				fprintf(stderr, "Artboard::initialize - Draw rule targets missing component width id %d\n", rules->parentId());
			}
		}
	}
	for (auto object : m_Animations)
	{
		if (!canContinue(code = object->onAddedClean(this)))
		{
			return code;
		}
	}
	// Multi-level references have been built up, now we can actually mark
	// what's dependent on what.
	for (auto object : m_Objects)
	{
		if(object == nullptr) 
		{
			continue;
		}
		if (object->is<Component>())
		{
			object->as<Component>()->buildDependencies();
		}
		if (object->is<Drawable>())
		{
			Drawable* drawable = object->as<Drawable>();
			m_Drawables.push_back(drawable);

			for (ContainerComponent* parent = drawable; parent != nullptr;
			     parent = parent->parent())
			{
				auto itr = componentDrawRules.find(parent);
				if (itr != componentDrawRules.end())
				{
					drawable->flattenedDrawRules = itr->second;
					break;
				}
			}
		}
	}

	sortDependencies();

	DrawTarget root;
	// Build up the draw order. Look for draw targets and build their
	// dependencies.
	for (auto object : m_Objects)
	{
		if(object == nullptr) 
		{
			continue;
		}
		if (object->is<DrawTarget>())
		{
			DrawTarget* target = object->as<DrawTarget>();
			root.addDependent(target);

			auto dependentRules = target->drawable()->flattenedDrawRules;
			if (dependentRules != nullptr)
			{
				// Because we don't store targets on rules, we need to find the
				// targets that belong to this rule here.
				for (auto object : m_Objects)
				{
					if (object->is<DrawTarget>())
					{
						DrawTarget* dependentTarget = object->as<DrawTarget>();
						if (dependentTarget->parent() == dependentRules)
						{
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
	while (itr != drawTargetOrder.end())
	{
		m_DrawTargets.push_back(reinterpret_cast<DrawTarget*>(*itr++));
	}

	return StatusCode::Ok;
}

void Artboard::sortDrawOrder()
{
	for (auto target : m_DrawTargets)
	{
		target->first = target->last = nullptr;
	}

	m_FirstDrawable = nullptr;
	Drawable* lastDrawable = nullptr;
	for (auto drawable : m_Drawables)
	{
		auto rules = drawable->flattenedDrawRules;
		if (rules != nullptr && rules->activeTarget() != nullptr)
		{

			auto target = rules->activeTarget();
			if (target->first == nullptr)
			{
				target->first = target->last = drawable;
				drawable->prev = drawable->next = nullptr;
			}
			else
			{
				target->last->next = drawable;
				drawable->prev = target->last;
				target->last = drawable;
				drawable->next = nullptr;
			}
		}
		else
		{
			drawable->prev = lastDrawable;
			drawable->next = nullptr;
			if (lastDrawable == nullptr)
			{
				lastDrawable = m_FirstDrawable = drawable;
			}
			else
			{
				lastDrawable->next = drawable;
				lastDrawable = drawable;
			}
		}
	}

	for (auto rule : m_DrawTargets)
	{
		if (rule->first == nullptr)
		{
			continue;
		}
		auto targetDrawable = rule->drawable();
		switch (rule->placement())
		{
			case DrawTargetPlacement::before:
			{
				if (targetDrawable->prev != nullptr)
				{
					targetDrawable->prev->next = rule->first;
					rule->first->prev = targetDrawable->prev;
				}
				if (targetDrawable == m_FirstDrawable)
				{
					m_FirstDrawable = rule->first;
				}
				targetDrawable->prev = rule->last;
				rule->last->next = targetDrawable;
				break;
			}
			case DrawTargetPlacement::after:
			{
				if (targetDrawable->next != nullptr)
				{
					targetDrawable->next->prev = rule->last;
					rule->last->next = targetDrawable->next;
				}
				if (targetDrawable == lastDrawable)
				{
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

void Artboard::sortDependencies()
{
	DependencySorter sorter;
	sorter.sort(this, m_DependencyOrder);
	unsigned int graphOrder = 0;
	for (auto component : m_DependencyOrder)
	{
		component->m_GraphOrder = graphOrder++;
	}
	m_Dirt |= ComponentDirt::Components;
}

void Artboard::addObject(Core* object) { m_Objects.push_back(object); }

void Artboard::addAnimation(Animation* object)
{
	m_Animations.push_back(object);
}

Core* Artboard::resolve(int id) const
{
	if (id < 0 || id >= static_cast<int>(m_Objects.size()))
	{
		return nullptr;
	}
	return m_Objects[id];
}

void Artboard::onComponentDirty(Component* component)
{
	m_Dirt |= ComponentDirt::Components;

	/// If the order of the component is less than the current dirt depth,
	/// update the dirt depth so that the update loop can break out early
	/// and re-run (something up the tree is dirty).
	if (component->graphOrder() < m_DirtDepth)
	{
		m_DirtDepth = component->graphOrder();
	}
}

void Artboard::onDirty(ComponentDirt dirt)
{
	m_Dirt |= ComponentDirt::Components;
}

void Artboard::update(ComponentDirt value)
{
	if (hasDirt(value, ComponentDirt::DrawOrder))
	{
		sortDrawOrder();
	}
	if (hasDirt(value, ComponentDirt::Path))
	{
		m_ClipPath->reset();
		m_ClipPath->addRect(0.0f, 0.0f, width(), height());
		m_BackgroundPath->addRect(-width() * originX(), -height() * originY(), width(), height());
	}
}

bool Artboard::updateComponents()
{
	if (hasDirt(ComponentDirt::Components))
	{
		const int maxSteps = 100;
		int step = 0;
		int count = (int)m_DependencyOrder.size();
		while (hasDirt(ComponentDirt::Components) && step < maxSteps)
		{
			// m_Dirt = m_Dirt & ~ComponentDirt::Components;

			// Track dirt depth here so that if something else marks
			// dirty, we restart.
			for (int i = 0; i < count; i++)
			{
				auto component = m_DependencyOrder[i];
				m_DirtDepth = i;
				auto d = component->m_Dirt;
				if (d == ComponentDirt::None)
				{
					continue;
				}
				component->m_Dirt = ComponentDirt::None;
				component->update(d);

				// If the update changed the dirt depth by adding dirt to
				// something before us (in the DAG), early out and re-run
				// the update.
				if (m_DirtDepth < (unsigned int) i)
				{
					// We put this in here just to know if we need to keep
					// this around...
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

bool Artboard::advance(double elapsedSeconds) { return updateComponents(); }

void Artboard::draw(Renderer* renderer)
{
	renderer->save();
	renderer->clipPath(m_ClipPath->renderPath());

	Mat2D artboardTransform;
	artboardTransform[4] = width() * originX();
	artboardTransform[5] = height() * originY();
	renderer->transform(artboardTransform);
	for (auto shapePaint : m_ShapePaints)
	{
		shapePaint->draw(renderer, m_BackgroundPath);
	}

	for (auto drawable = m_FirstDrawable; drawable != nullptr;
	     drawable = drawable->prev)
	{
		drawable->draw(renderer);
	}

	renderer->restore();
}

AABB Artboard::bounds() const { return AABB(0.0f, 0.0f, width(), height()); }
