#ifndef _RIVE_OPENGL_RENDER_PATH_HPP_
#define _RIVE_OPENGL_RENDER_PATH_HPP_

#include "contour_render_path.hpp"
#include "opengl.h"

namespace rive
{
	class OpenGLRenderer;
	class OpenGLRenderPath : public ContourRenderPath
	{
	private:
		FillRule m_FillRule;
		GLuint m_ContourBuffer = 0;

	public:
		OpenGLRenderPath();
		~OpenGLRenderPath();
		void fillRule(FillRule value) override;
		FillRule fillRule() const { return m_FillRule; }

		void stencil(OpenGLRenderer* renderer, const Mat2D& transform);
	};
} // namespace rive
#endif