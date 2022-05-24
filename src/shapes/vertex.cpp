#include "rive/shapes/vertex.hpp"

using namespace rive;

Vec2D Vertex::renderTranslation() {
    if (hasWeight()) {
        return m_Weight->translation();
    }
    return Vec2D(x(), y());
}

void Vertex::xChanged() { markGeometryDirty(); }
void Vertex::yChanged() { markGeometryDirty(); }

void Vertex::deform(const Mat2D& worldTransform, const float* boneTransforms) {
    m_Weight->translation() = Weight::deform(m_Weight->indices(),
                                             m_Weight->values(),
                                             boneTransforms) * (worldTransform * Vec2D(x(), y()));
}