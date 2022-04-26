#include "rive/shapes/shape_paint_container.hpp"
#include "rive/artboard.hpp"
#include "rive/factory.hpp"
#include "rive/component.hpp"
#include "rive/renderer.hpp"
#include "rive/shapes/metrics_path.hpp"
#include "rive/shapes/paint/fill.hpp"
#include "rive/shapes/paint/stroke.hpp"
#include "rive/shapes/shape.hpp"

using namespace rive;

ShapePaintContainer* ShapePaintContainer::from(Component* component) {
    switch (component->coreType()) {
        case Artboard::typeKey:
            return component->as<Artboard>();
            break;
        case Shape::typeKey:
            return component->as<Shape>();
            break;
    }
    return nullptr;
}

void ShapePaintContainer::addPaint(ShapePaint* paint) { m_ShapePaints.push_back(paint); }

PathSpace ShapePaintContainer::pathSpace() const {
    PathSpace space = m_DefaultPathSpace;
    for (auto paint : m_ShapePaints) {
        space |= paint->pathSpace();
    }
    return space;
}

void ShapePaintContainer::invalidateStrokeEffects() {
    for (auto paint : m_ShapePaints) {
        if (paint->is<Stroke>()) {
            paint->as<Stroke>()->invalidateEffects();
        }
    }
}

std::unique_ptr<CommandPath> ShapePaintContainer::makeCommandPath(PathSpace space) {
    // Force a render path if we specifically request to use it for clipping or
    // this shape is used for clipping.
    bool needForRender =
        ((space | m_DefaultPathSpace) & PathSpace::Clipping) == PathSpace::Clipping;

    bool needForEffects = false;

    for (auto paint : m_ShapePaints) {
        if (space != PathSpace::Neither && (space & paint->pathSpace()) != space) {
            continue;
        }

        if (paint->is<Stroke>() && paint->as<Stroke>()->hasStrokeEffect()) {
            needForEffects = true;
        } else {
            needForRender = true;
        }
    }

    auto factory = getArtboard()->factory();
    if (needForEffects && needForRender) {
        return std::unique_ptr<CommandPath>(new RenderMetricsPath(factory->makeEmptyRenderPath()));
    } else if (needForEffects) {
        return std::unique_ptr<CommandPath>(new OnlyMetricsPath());
    } else {
        return std::unique_ptr<CommandPath>(factory->makeEmptyRenderPath());
    }
}
