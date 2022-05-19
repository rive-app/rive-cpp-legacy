#include "rive/constraints/transform_constraint.hpp"
#include "rive/transform_component.hpp"
#include "rive/math/mat2d.hpp"
#include "rive/math/math_types.hpp"

using namespace rive;

void TransformConstraint::constrain(TransformComponent* component) {
    if (m_Target == nullptr) {
        return;
    }

    const Mat2D& transformA = component->worldTransform();
    Mat2D transformB(m_Target->worldTransform());
    if (sourceSpace() == TransformSpace::local) {
        const Mat2D& targetParentWorld = getParentWorld(*m_Target);

        Mat2D inverse;
        if (!targetParentWorld.invert(&inverse)) {
            return;
        }
        transformB = inverse * transformB;
    }
    if (destSpace() == TransformSpace::local) {
        const Mat2D& targetParentWorld = getParentWorld(*component);
        transformB = targetParentWorld * transformB;
    }

    m_ComponentsA = transformA.decompose();
    m_ComponentsB = transformB.decompose();

    float angleA = std::fmod(m_ComponentsA.rotation(), math::PI * 2);
    float angleB = std::fmod(m_ComponentsB.rotation(), math::PI * 2);
    float diff = angleB - angleA;
    if (diff > math::PI) {
        diff -= math::PI * 2;
    } else if (diff < -math::PI) {
        diff += math::PI * 2;
    }

    float t = strength();
    float ti = 1.0f - t;

    m_ComponentsB.rotation(angleA + diff * t);
    m_ComponentsB.x(m_ComponentsA.x() * ti + m_ComponentsB.x() * t);
    m_ComponentsB.y(m_ComponentsA.y() * ti + m_ComponentsB.y() * t);
    m_ComponentsB.scaleX(m_ComponentsA.scaleX() * ti + m_ComponentsB.scaleX() * t);
    m_ComponentsB.scaleY(m_ComponentsA.scaleY() * ti + m_ComponentsB.scaleY() * t);
    m_ComponentsB.skew(m_ComponentsA.skew() * ti + m_ComponentsB.skew() * t);

    component->mutableWorldTransform() = Mat2D::compose(m_ComponentsB);
}
