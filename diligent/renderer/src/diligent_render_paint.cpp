#include "diligent_render_paint.hpp"
using namespace rive;

void DiligentRenderPaint::style(RenderPaintStyle style)
{
	m_PaintStyle = style;
}

void DiligentRenderPaint::color(unsigned int value) {}

void DiligentRenderPaint::thickness(float value) {}

void DiligentRenderPaint::join(StrokeJoin value) {}

void DiligentRenderPaint::cap(StrokeCap value) {}

void DiligentRenderPaint::blendMode(BlendMode value) {}

void DiligentRenderPaint::linearGradient(float sx, float sy, float ex, float ey)
{
}

void DiligentRenderPaint::radialGradient(float sx, float sy, float ex, float ey)
{
}

void DiligentRenderPaint::addStop(unsigned int color, float stop) {}

void DiligentRenderPaint::completeGradient() {}

DiligentRenderPaint::~DiligentRenderPaint() {}