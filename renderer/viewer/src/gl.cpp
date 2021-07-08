#ifdef RIVE_HAS_OPENGL
#include "opengl/opengl_renderer.hpp"
namespace rive
{
	class ViewerGLRenderer : public OpenGLRenderer
	{
	public:
		void startFrame() override
		{
			glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			OpenGLRenderer::startFrame();
		}
	};
	LowLevelRenderer* makeRendererOpenGL() { return new ViewerGLRenderer(); }
} // namespace rive
#endif