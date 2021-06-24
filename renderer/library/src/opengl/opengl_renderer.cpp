#include "opengl/opengl_renderer.hpp"
#include "opengl/opengl_render_path.hpp"
#include "opengl/opengl_render_paint.hpp"

using namespace rive;

OpenGLRenderer::OpenGLRenderer() {}
OpenGLRenderer::~OpenGLRenderer() {}

bool OpenGLRenderer::initialize()
{
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	return true;
}
void OpenGLRenderer::save() {}
void OpenGLRenderer::restore() {}
void OpenGLRenderer::transform(const Mat2D& transform) {}
void OpenGLRenderer::drawPath(RenderPath* path, RenderPaint* paint) {}
void OpenGLRenderer::clipPath(RenderPath* path) {}

void OpenGLRenderer::onViewportSizeChanged(ViewportSize from, ViewportSize to)
{
}

void OpenGLRenderer::startFrame() { glClear(GL_COLOR_BUFFER_BIT); }
void OpenGLRenderer::endFrame() {}

RenderPaint* OpenGLRenderer::makeRenderPaint()
{
	return new OpenGLRenderPaint();
}
RenderPath* OpenGLRenderer::makeRenderPath() { return new OpenGLRenderPath(); }