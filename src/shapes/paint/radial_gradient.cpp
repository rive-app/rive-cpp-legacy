#include "rive/shapes/paint/radial_gradient.hpp"
#include "rive/renderer.hpp"

using namespace rive;

void RadialGradient::makeGradient(const Vec2D& start, const Vec2D& end,
								const ColorInt colors[], const float stops[], size_t count)
{
	RenderTileMode tileMode = RenderTileMode::clamp;	// todo
	const Mat2D* localMatrix = nullptr;					// todo
	renderPaint()->shader = makeLinearGradient(start[0], start[1], end[0], end[1],
										       colors, stops, count,
								   			   tileMode, localMatrix);
}
