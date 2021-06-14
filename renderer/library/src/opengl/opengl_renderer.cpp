#include "opengl/opengl_renderer.hpp"

using namespace rive;

OpenGLRenderer::~OpenGLRenderer() {}
void OpenGLRenderer::save() {}
void OpenGLRenderer::restore() {}
void OpenGLRenderer::transform(const Mat2D& transform) {}
void OpenGLRenderer::drawPath(RenderPath* path, RenderPaint* paint) {}
void OpenGLRenderer::clipPath(RenderPath* path) {}