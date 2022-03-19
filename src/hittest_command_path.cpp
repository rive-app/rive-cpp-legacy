/*
 * Copyright 2022 Rive
 */

#include "rive/hittest_command_path.hpp"

using namespace rive;

HitTestCommandPath::HitTestCommandPath(const IAABB& area) : m_Area(area) {
    m_Tester.reset(m_Area);
}

bool HitTestCommandPath::wasHit() {
    return m_Tester.test();
}

void HitTestCommandPath::reset() {
    m_Tester.reset(m_Area);
}

void HitTestCommandPath::fillRule(FillRule value) {
    m_FillRule = value;
}

void HitTestCommandPath::addPath(CommandPath* path, const Mat2D& transform) {
    assert(false);
    // not supported
}

RenderPath* HitTestCommandPath::renderPath() {
    assert(false);
    // not supported
    return nullptr;
}

void HitTestCommandPath::moveTo(float x, float y) {
    m_Tester.move(m_Xform * Vec2D(x, y));
}

void HitTestCommandPath::lineTo(float x, float y) {
    m_Tester.line(m_Xform * Vec2D(x, y));
}

void HitTestCommandPath::cubicTo(float ox, float oy, float ix, float iy, float x, float y) {
    m_Tester.cubic(m_Xform * Vec2D(ox, oy),
                   m_Xform * Vec2D(ix, iy),
                   m_Xform * Vec2D( x,  y));
}

void HitTestCommandPath::close() {
    m_Tester.close();
}
