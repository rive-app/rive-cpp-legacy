#include "graphics_api.hpp"

#ifdef RIVE_HAS_METAL
#include "metal/metal_renderer.hpp"
#include "metal/metal_render_paint.hpp"
#include "metal/metal_render_path.hpp"
#endif
#ifdef RIVE_HAS_OPENGL
#include "opengl/opengl_renderer.hpp"
#include "opengl/opengl_render_paint.hpp"
#include "opengl/opengl_render_path.hpp"
#endif

#include <stdio.h>

using namespace rive;

static GraphicsApi::Type g_GraphicsApi;
GraphicsApi::Type GraphicsApi::activeRenderer() { return g_GraphicsApi; }

Renderer* GraphicsApi::makeRenderer(GraphicsApi::Type api)
{
	switch (api)
	{
		case GraphicsApi::unknown:
			fprintf(stderr, "cannot instance unknown api\n");
			break;
		case GraphicsApi::opengl:
#ifdef RIVE_HAS_OPENGL
			g_GraphicsApi = GraphicsApi::opengl;
			return new OpenGLRenderer();
#else
			fprintf(stderr, "opengl is not supported\n");
#endif
			break;
		case GraphicsApi::metal:

#ifdef RIVE_HAS_METAL
			g_GraphicsApi = GraphicsApi::metal;
			return new MetalRenderer();
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

Renderer* GraphicsApi::makeRenderer()
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
		switch (g_GraphicsApi)
		{
#ifdef RIVE_HAS_OPENGL
			case GraphicsApi::opengl:
				return new OpenGLRenderPaint();
#endif
#ifdef RIVE_HAS_METAL
			case GraphicsApi::metal:
				return new MetalRenderPaint();
#endif
#ifdef RIVE_HAS_VULKAN
			case GraphicsApi::vulkan:
				return nullptr;
#endif
#ifdef RIVE_HAS_D3D11
			case GraphicsApi::d3d11:
				return nullptr;
#endif
#ifdef RIVE_HAS_D3D12
			case GraphicsApi::d3d12:
				return nullptr;
#endif
			default:
				return nullptr;
		}
	}
	RenderPath* makeRenderPath()
	{
		switch (g_GraphicsApi)
		{
#ifdef RIVE_HAS_OPENGL
			case GraphicsApi::opengl:
				return new OpenGLRenderPath();
#endif
#ifdef RIVE_HAS_METAL
			case GraphicsApi::metal:
				return new MetalRenderPath();
#endif
#ifdef RIVE_HAS_VULKAN
			case GraphicsApi::vulkan:
				return nullptr;
#endif
#ifdef RIVE_HAS_D3D11
			case GraphicsApi::d3d11:
				return nullptr;
#endif
#ifdef RIVE_HAS_D3D12
			case GraphicsApi::d3d12:
				return nullptr;
#endif
			default:
				return nullptr;
		}
	}
} // namespace rive