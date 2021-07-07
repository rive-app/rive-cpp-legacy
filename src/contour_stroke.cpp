#ifdef LOW_LEVEL_RENDERING
#include "contour_stroke.hpp"
#include "contour_render_path.hpp"
#include "math/vec2d.hpp"

using namespace rive;

void ContourStroke::extrude(const ContourRenderPath* renderPath,
                            bool isClosed,
                            StrokeJoin join,
                            StrokeCap cap,
                            float strokeWidth)
{
	m_TriangleStrip.clear();

	const std::vector<Vec2D>& points = renderPath->contourVertices();
	auto size = points.size();
	if (size < 2)
	{
		return;
	}
	const Vec2D& lastPoint = points[0];
	Vec2D lastDiff;
	Vec2D::subtract(lastDiff, points[1], lastPoint);
	float lastLength = Vec2D::length(lastDiff);
	Vec2D lastDiffNormalized;
	Vec2D::scale(lastDiffNormalized, lastDiff, 1.0f / lastLength);

	Vec2D lastA, lastB;
	Vec2D perpendicularStrokeDiff = Vec2D(lastDiffNormalized[1] * -strokeWidth,
	                                      lastDiffNormalized[0] * strokeWidth);
	Vec2D::add(lastA, lastPoint, perpendicularStrokeDiff);
	Vec2D::subtract(lastB, lastPoint, perpendicularStrokeDiff);
}
#endif