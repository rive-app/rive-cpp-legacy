#ifndef _RIVE_OPENGL_RENDERER_HPP_
#define _RIVE_OPENGL_RENDERER_HPP_

#include "low_level/low_level_renderer.hpp"
#include "math/mat2d.hpp"
#include "opengl.h"
#include <vector>

namespace rive
{
	class OpenGLRenderer : public LowLevelRenderer
	{
	private:
		Mat2D m_Projection;
		GLuint m_VertexShader = 0, m_FragmentShader = 0;
		GLuint m_Program = 0;
		GLuint m_IndexBuffer = 0;
		std::vector<unsigned short> m_Indices;

	public:
		const GLuint indexBuffer() const { return m_IndexBuffer; }
		GraphicsApi::Type type() const override { return GraphicsApi::opengl; }
		OpenGLRenderer();
		~OpenGLRenderer();
		void drawPath(RenderPath* path, RenderPaint* paint) override;
		void clipPath(RenderPath* path) override;

		void startFrame() override;
		void endFrame() override;

		RenderPaint* makeRenderPaint() override;
		RenderPath* makeRenderPath() override;

		bool initialize(void* data) override;

		void updateIndexBuffer(std::size_t contourLength);
	};

} // namespace rive
#endif