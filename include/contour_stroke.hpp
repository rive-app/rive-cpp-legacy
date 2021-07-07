#ifndef _RIVE_CONTOUR_STROKE_HPP_
#define _RIVE_CONTOUR_STROKE_HPP_

#include "renderer.hpp"
#include "math/aabb.hpp"
#include <vector>
#include <cstdint>

namespace rive
{
	class ContourRenderPath;

	///
	/// Builds a triangle strip vertex buffer from a ContourRenderPath.
	///
	class ContourStroke
	{
	protected:
		std::vector<Vec2D> m_TriangleStrip;

	public:
		void extrude(const ContourRenderPath* renderPath,
		             bool isClosed,
		             StrokeJoin join,
		             StrokeCap cap,
		             float strokeWidth);
	};
} // namespace rive
#endif