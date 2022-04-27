#include "rive/math/mat2d.hpp"
#include "rive/math/transform_components.hpp"
#include "rive/math/vec2d.hpp"
#include <cmath>

using namespace rive;

Mat2D Mat2D::fromRotation(float rad) {
    float s = 0, c = 1;
    if (rad != 0) {
        s = sin(rad);
        c = cos(rad);
    }
    return {c, s, -s, c, 0, 0};
}

Mat2D Mat2D::scale(Vec2D vec) const {
    return {
        m_Buffer[0] * vec.x(),
        m_Buffer[1] * vec.x(),
        m_Buffer[2] * vec.y(),
        m_Buffer[3] * vec.y(),
        m_Buffer[4],
        m_Buffer[5],
    };
}

Mat2D Mat2D::multiply(const Mat2D& a, const Mat2D& b) {
    float a0 = a[0], a1 = a[1], a2 = a[2], a3 = a[3], a4 = a[4], a5 = a[5],
          b0 = b[0], b1 = b[1], b2 = b[2], b3 = b[3], b4 = b[4], b5 = b[5];
    return {
        a0 * b0 + a2 * b1,
        a1 * b0 + a3 * b1,
        a0 * b2 + a2 * b3,
        a1 * b2 + a3 * b3,
        a0 * b4 + a2 * b5 + a4,
        a1 * b4 + a3 * b5 + a5,
    };
}

bool Mat2D::invert(Mat2D* result) const {
    float aa = m_Buffer[0], ab  = m_Buffer[1], ac  = m_Buffer[2],
          ad = m_Buffer[3], atx = m_Buffer[4], aty = m_Buffer[5];

    float det = aa * ad - ab * ac;
    if (det == 0.0f) {
        return false;
    }
    det = 1.0f / det;

    *result = {
         ad * det,
        -ab * det,
        -ac * det,
         aa * det,
        (ac * aty - ad * atx) * det,
        (ab * atx - aa * aty) * det,
    };
    return true;
}

TransformComponents Mat2D::decompose() const {
    float m0 = m_Buffer[0], m1 = m_Buffer[1],
          m2 = m_Buffer[2], m3 = m_Buffer[3];

    float rotation = (float)std::atan2(m1, m0);
    float denom = m0 * m0 + m1 * m1;
    float scaleX = (float)std::sqrt(denom);
    float scaleY = (m0 * m3 - m2 * m1) / scaleX;
    float skewX = (float)std::atan2(m0 * m2 + m1 * m3, denom);

    TransformComponents result;
    result.x(m_Buffer[4]);
    result.y(m_Buffer[5]);
    result.scaleX(scaleX);
    result.scaleY(scaleY);
    result.rotation(rotation);
    result.skew(skewX);
    return result;
}

Mat2D Mat2D::compose(const TransformComponents& components) {
    auto result = Mat2D::fromRotation(components.rotation());
    result[4] = components.x();
    result[5] = components.y();
    result = result.scale(components.scale());

    float sk = components.skew();
    if (sk != 0.0f) {
        result[2] = result[0] * sk + result[2];
        result[3] = result[1] * sk + result[3];
    }
    return result;
}

void Mat2D::scaleByValues(float sx, float sy) {
    m_Buffer[0] *= sx;
    m_Buffer[1] *= sx;
    m_Buffer[2] *= sy;
    m_Buffer[3] *= sy;
}
