#include "opengl/opengl_render_path.hpp"

using namespace rive;

OpenGLRenderPath::OpenGLRenderPath() { glGenBuffers(1, &m_ContourBuffer); }
OpenGLRenderPath::~OpenGLRenderPath() { glDeleteBuffers(1, &m_ContourBuffer); }
void OpenGLRenderPath::fillRule(FillRule value) { m_FillRule = value; }

void OpenGLRenderPath::stencil(OpenGLRenderer* renderer, const Mat2D& transform)
{
	if (isDirty())
	{
		computeContour();

		glBindBuffer(GL_ARRAY_BUFFER, m_ContourBuffer);
		auto length = m_ContourVertices.size();
		glBufferData(GL_ARRAY_BUFFER,
		             length * sizeof(float),
		             &m_ContourVertices[0],
		             GL_DYNAMIC_DRAW);
	}
}