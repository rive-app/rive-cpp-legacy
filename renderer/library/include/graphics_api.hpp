#ifndef _RIVE_GRAPHICS_API_HPP_
#define _RIVE_GRAPHICS_API_HPP_

#include "rive/renderer.hpp"

namespace rive
{
	class LowLevelRenderer;
	class GraphicsApi
	{
	public:
		///
		/// The Rive rendering apis that are (or will be) available.
		///
		enum Type : unsigned int
		{
			unknown = 0,
			opengl = 1,
			metal = 2,
			vulkan = 3,
			d3d11 = 4,
			d3d12 = 5
		};
		///
		/// Get the currently active GraphicsApi.
		///
		LowLevelRenderer* activeRenderer();

		static LowLevelRenderer* makeRenderer(GraphicsApi::Type api);
		static LowLevelRenderer* makeRenderer();

		static const char* name(GraphicsApi::Type type)
		{
			switch (type)
			{
				case GraphicsApi::unknown:
					return "Unknown";
				case GraphicsApi::opengl:
					return "OpenGL";
				case GraphicsApi::metal:
					return "Metal";
				case GraphicsApi::vulkan:
					return "Vulkan";
				case GraphicsApi::d3d11:
					return "Direct3D 11";
				case GraphicsApi::d3d12:
					return "Direct3D 12";
			}
		}
	};

} // namespace rive
#endif