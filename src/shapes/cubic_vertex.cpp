#include "rive/shapes/cubic_vertex.hpp"
#include "rive/bones/cubic_weight.hpp"
using namespace rive;

const Vec2D& CubicVertex::renderIn() {
    if (hasWeight()) {
        return weight<CubicWeight>()->inTranslation();
    } else {
        return inPoint();
    }
}

const Vec2D& CubicVertex::renderOut() {
    if (hasWeight()) {
        return weight<CubicWeight>()->outTranslation();
    } else {
        return outPoint();
    }
}

const Vec2D& CubicVertex::inPoint() {
    if (!m_InValid) {
        computeIn();
        m_InValid = true;
    }
    return m_InPoint;
}

const Vec2D& CubicVertex::outPoint() {
    if (!m_OutValid) {
        computeOut();
        m_OutValid = true;
    }
    return m_OutPoint;
}

void CubicVertex::outPoint(const Vec2D& value) {
    m_OutPoint = value;
    m_OutValid = true;
}

void CubicVertex::inPoint(const Vec2D& value) {
    m_InPoint = value;
    m_InValid = true;
}

void CubicVertex::xChanged() {
    Super::xChanged();
    m_InValid = m_OutValid = false;
}
void CubicVertex::yChanged() {
    Super::yChanged();
    m_InValid = m_OutValid = false;
}

void CubicVertex::deform(Mat2D& worldTransform, float* boneTransforms) {
    Super::deform(worldTransform, boneTransforms);

    auto cubicWeight = weight<CubicWeight>();

    auto inp = inPoint();
    Weight::deform(inp[0],
                   inp[1],
                   cubicWeight->inIndices(),
                   cubicWeight->inValues(),
                   worldTransform,
                   boneTransforms,
                   cubicWeight->inTranslation());

    auto outp = outPoint();
    Weight::deform(outp[0],
                   outp[1],
                   cubicWeight->outIndices(),
                   cubicWeight->outValues(),
                   worldTransform,
                   boneTransforms,
                   cubicWeight->outTranslation());
}
