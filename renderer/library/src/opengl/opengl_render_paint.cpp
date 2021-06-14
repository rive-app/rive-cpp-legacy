#include "opengl/opengl_render_paint.hpp"
using namespace rive;

void OpenGLRenderPaint::style(RenderPaintStyle style) { m_PaintStyle = style; }

void OpenGLRenderPaint::color(unsigned int value) {}

void OpenGLRenderPaint::thickness(float value) {}

void OpenGLRenderPaint::join(StrokeJoin value) {}

void OpenGLRenderPaint::cap(StrokeCap value) {}

void OpenGLRenderPaint::blendMode(BlendMode value) {}

void OpenGLRenderPaint::linearGradient(float sx, float sy, float ex, float ey)
{
}

void OpenGLRenderPaint::radialGradient(float sx, float sy, float ex, float ey)
{
}

void OpenGLRenderPaint::addStop(unsigned int color, float stop) {}

void OpenGLRenderPaint::completeGradient() {}

OpenGLRenderPaint::~OpenGLRenderPaint() {}