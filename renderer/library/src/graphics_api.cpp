#include "graphics_api.hpp"
#include "low_level/low_level_renderer.hpp"

#include <stdio.h>
#include <cassert>

using namespace rive;

static LowLevelRenderer* g_GraphicsApi = nullptr;
LowLevelRenderer* GraphicsApi::activeRenderer() { return g_GraphicsApi; }

LowLevelRenderer* GraphicsApi::makeRenderer(GraphicsApi::Type api)
{
	switch (api)
	{
		case GraphicsApi::unknown:
			fprintf(stderr, "cannot instance unknown api\n");
			break;
		case GraphicsApi::opengl:
#ifdef RIVE_HAS_OPENGL
			printf("Rive: Renderering with OpenGL\n");
			extern LowLevelRenderer* makeRendererOpenGL();
			return (g_GraphicsApi = makeRendererOpenGL());
#else
			fprintf(stderr, "opengl is not supported\n");
#endif
			break;
		case GraphicsApi::metal:

#ifdef RIVE_HAS_METAL
			printf("Rive: Renderering with Metal\n");
			extern LowLevelRenderer* makeRendererMetal();
			return (g_GraphicsApi = makeRendererMetal());
#else
			fprintf(stderr, "metal is not supported\n");
#endif
			break;
		case GraphicsApi::vulkan:
			fprintf(stderr, "vulkan is not supported\n");
			break;
		case GraphicsApi::d3d11:
			fprintf(stderr, "d3d11 is not supported\n");
			break;
		case GraphicsApi::d3d12:
			fprintf(stderr, "d3d12 is not supported\n");
			break;
		default:
			fprintf(stderr, "unhandled graphics api type\n");
			break;
	}

	return nullptr;
}

LowLevelRenderer* GraphicsApi::makeRenderer()
{
#ifdef RIVE_HAS_METAL
	return makeRenderer(GraphicsApi::metal);
#elif defined RIVE_HAS_OPENGL
	return makeRenderer(GraphicsApi::opengl);
#else
	return nullptr;
#endif
}

namespace rive
{
	RenderPaint* makeRenderPaint()
	{
		assert(g_GraphicsApi != nullptr);
		return g_GraphicsApi->makeRenderPaint();
	}

	RenderPath* makeRenderPath()
	{
		assert(g_GraphicsApi != nullptr);
		return g_GraphicsApi->makeRenderPath();
	}
} // namespace rive