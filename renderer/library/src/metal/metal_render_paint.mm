#include "metal/metal_render_paint.hpp"
using namespace rive;

void MetalRenderPaint::style(RenderPaintStyle style) { m_PaintStyle = style; }

void MetalRenderPaint::color(unsigned int value) {}

void MetalRenderPaint::thickness(float value) {}

void MetalRenderPaint::join(StrokeJoin value) {}

void MetalRenderPaint::cap(StrokeCap value) {}

void MetalRenderPaint::blendMode(BlendMode value) {}

void MetalRenderPaint::linearGradient(float sx, float sy, float ex, float ey) {}

void MetalRenderPaint::radialGradient(float sx, float sy, float ex, float ey) {}

void MetalRenderPaint::addStop(unsigned int color, float stop) {}

void MetalRenderPaint::completeGradient() {}

void MetalRenderPaint::invalidateStroke() {}

MetalRenderPaint::~MetalRenderPaint() {}