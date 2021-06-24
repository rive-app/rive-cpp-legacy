#ifdef RIVE_HAS_OPENGL
#include "opengl/opengl_renderer.hpp"
namespace rive
{
	LowLevelRenderer* makeRendererOpenGL() { return new OpenGLRenderer(); }
} // namespace rive
#endif