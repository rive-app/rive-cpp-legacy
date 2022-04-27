#include "rive/constraints/rotation_constraint.hpp"
#include "rive/transform_component.hpp"
#include "rive/math/mat2d.hpp"
#include <cmath>

using namespace rive;

void RotationConstraint::constrain(TransformComponent* component) {
    const Mat2D& transformA = component->worldTransform();
    Mat2D transformB;
    m_ComponentsA = transformA.decompose();
    if (m_Target == nullptr) {
        transformB = transformA;
        m_ComponentsB = m_ComponentsA;
    } else {
        transformB = m_Target->worldTransform();
        if (sourceSpace() == TransformSpace::local) {
            Mat2D inverse;

            if (!getParentWorld(*m_Target).invert(&inverse)) {
                return;
            }
            transformB = inverse * transformB;
        }

        m_ComponentsB = transformB.decompose();

        if (!doesCopy()) {
            m_ComponentsB.rotation(destSpace() == TransformSpace::local ? 0.0f
                                                                        : m_ComponentsA.rotation());
        } else {
            m_ComponentsB.rotation(m_ComponentsB.rotation() * copyFactor());
            if (offset()) {
                m_ComponentsB.rotation(m_ComponentsB.rotation() + component->rotation());
            }
        }

        if (destSpace() == TransformSpace::local) {
            // Destination space is in parent transform coordinates. Recompose
            // the parent local transform and get it in world, then decompose
            // the world for interpolation.

            transformB = Mat2D::compose(m_ComponentsB);
            transformB = getParentWorld(*component) * transformB;
            m_ComponentsB = transformB.decompose();
        }
    }
    bool clampLocal = minMaxSpace() == TransformSpace::local;
    if (clampLocal) {
        // Apply min max in local space, so transform to local coordinates
        // first.
        transformB = Mat2D::compose(m_ComponentsB);
        Mat2D inverse;
        if (!getParentWorld(*component).invert(&inverse)) {
            return;
        }
        m_ComponentsB = (inverse * transformB).decompose();
    }
    if (max() && m_ComponentsB.rotation() > maxValue()) {
        m_ComponentsB.rotation(maxValue());
    }
    if (min() && m_ComponentsB.rotation() < minValue()) {
        m_ComponentsB.rotation(minValue());
    }
    if (clampLocal) {
        // Transform back to world.
        transformB = Mat2D::compose(m_ComponentsB);
        transformB = getParentWorld(*component) * transformB;
        m_ComponentsB = transformB.decompose();
    }

    float angleA = std::fmod(m_ComponentsA.rotation(), (float)M_PI * 2);
    float angleB = std::fmod(m_ComponentsB.rotation(), (float)M_PI * 2);
    float diff = angleB - angleA;

    if (diff > M_PI) {
        diff -= M_PI * 2;
    } else if (diff < -M_PI) {
        diff += M_PI * 2;
    }

    m_ComponentsB.rotation(m_ComponentsA.rotation() + diff * strength());
    m_ComponentsB.x(m_ComponentsA.x());
    m_ComponentsB.y(m_ComponentsA.y());
    m_ComponentsB.scaleX(m_ComponentsA.scaleX());
    m_ComponentsB.scaleY(m_ComponentsA.scaleY());
    m_ComponentsB.skew(m_ComponentsA.skew());

    component->mutableWorldTransform() = Mat2D::compose(m_ComponentsB);
}
