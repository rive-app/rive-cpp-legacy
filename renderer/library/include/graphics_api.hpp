#ifndef _RIVE_GRAPHICS_API_HPP_
#define _RIVE_GRAPHICS_API_HPP_

#include "renderer.hpp"

namespace rive
{
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
		GraphicsApi::Type activeRenderer();

		static Renderer* makeRenderer(GraphicsApi::Type api);
		static Renderer* makeRenderer();
	};

} // namespace rive
#endif