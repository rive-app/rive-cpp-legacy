#include "rive/shapes/path_composer.hpp"
#include "rive/artboard.hpp"
#include "rive/renderer.hpp"
#include "rive/shapes/path.hpp"
#include "rive/shapes/shape.hpp"

using namespace rive;

static Mat2D identity;

PathComposer::PathComposer(Shape* shape) : m_Shape(shape) {}

void PathComposer::buildDependencies() {
    assert(m_Shape != nullptr);
    m_Shape->addDependent(this);
    for (auto path : m_Shape->paths()) {
        path->addDependent(this);
    }
}

void PathComposer::update(ComponentDirt value) {
    if (hasDirt(value, ComponentDirt::Path)) {
        auto space = m_Shape->pathSpace();
        if ((space & PathSpace::Local) == PathSpace::Local) {
            if (m_LocalPath == nullptr) {
                m_LocalPath = m_Shape->makeCommandPath(PathSpace::Local);
            } else {
                m_LocalPath->reset();
            }
            auto world = m_Shape->worldTransform();
            Mat2D inverseWorld;
            if (!Mat2D::invert(inverseWorld, world)) {
                inverseWorld = Mat2D();
            }
            // Get all the paths into local shape space.
            for (auto path : m_Shape->paths()) {
                const auto localTransform = inverseWorld * path->pathTransform();
                m_LocalPath->addPath(path->commandPath(), localTransform);
            }
        }
        if ((space & PathSpace::World) == PathSpace::World) {
            if (m_WorldPath == nullptr) {
                m_WorldPath = m_Shape->makeCommandPath(PathSpace::World);
            } else {
                m_WorldPath->reset();
            }
            for (auto path : m_Shape->paths()) {
                const Mat2D& transform = path->pathTransform();
                m_WorldPath->addPath(path->commandPath(), transform);
            }
        }
    }
}
