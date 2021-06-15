#include "opengl/opengl_renderer.hpp"

using namespace rive;

OpenGLRenderer::OpenGLRenderer() { glClearColor(1.0f, 0.0f, 0.0f, 1.0f); }
OpenGLRenderer::~OpenGLRenderer() {}
void OpenGLRenderer::save() {}
void OpenGLRenderer::restore() {}
void OpenGLRenderer::transform(const Mat2D& transform) {}
void OpenGLRenderer::drawPath(RenderPath* path, RenderPaint* paint) {}
void OpenGLRenderer::clipPath(RenderPath* path) {}

void OpenGLRenderer::onViewportSizeChanged(ViewportSize from, ViewportSize to)
{
}

void OpenGLRenderer::clear() { glClear(GL_COLOR_BUFFER_BIT); }
void OpenGLRenderer::frame() {}