#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>

#ifdef LOW_LEVEL_RENDERING
#include "rive/contour_stroke.hpp"
#include "rive/contour_render_path.hpp"
#include "rive/math/vec2d.hpp"
#include <assert.h>

using namespace rive;

static const int subdivisionArcLength = 4.0f;

void ContourStroke::reset()
{
	m_TriangleStrip.clear();
	m_Offsets.clear();
}

void ContourStroke::resetRenderOffset() { m_RenderOffset = 0; }

void ContourStroke::nextRenderOffset(std::size_t& start, std::size_t& end)
{
	assert(m_RenderOffset < m_Offsets.size());
	start = m_RenderOffset == 0 ? 0 : m_Offsets[m_RenderOffset - 1];
	end = m_Offsets[m_RenderOffset++];
}

void ContourStroke::extrude(const ContourRenderPath* renderPath,
                            bool isClosed,
                            StrokeJoin join,
                            StrokeCap cap,
                            float strokeWidth,
                            const Mat2D& transform)
{
	// TODO: if transform is identity, no need to copy and transform
	// contourPoints->points.

	const std::vector<Vec2D>& contourPoints = renderPath->contourVertices();
	std::vector<Vec2D> points(contourPoints);
	auto pointCount = points.size();
	if (pointCount < 6)
	{
		return;
	}
	for (int i = 4; i < pointCount; i++)
	{
		Vec2D& point = points[i];
		Vec2D::transform(point, point, transform);
	}
	auto startOffset = m_TriangleStrip.size();
	Vec2D lastPoint = points[4];
	Vec2D lastDiff;
	Vec2D::subtract(lastDiff, points[5], lastPoint);
	float lastLength = Vec2D::length(lastDiff);
	Vec2D lastDiffNormalized;
	Vec2D::scale(lastDiffNormalized, lastDiff, 1.0f / lastLength);

	Vec2D lastA, lastB;
	Vec2D perpendicularStrokeDiff = Vec2D(lastDiffNormalized[1] * -strokeWidth,
	                                      lastDiffNormalized[0] * strokeWidth);
	Vec2D::add(lastA, lastPoint, perpendicularStrokeDiff);
	Vec2D::subtract(lastB, lastPoint, perpendicularStrokeDiff);

	if (!isClosed)
	{
		switch (cap)
		{
			case StrokeCap::square:
			{
				Vec2D squareA, squareB;
				Vec2D strokeDiff = Vec2D(lastDiffNormalized[0] * strokeWidth,
				                         lastDiffNormalized[1] * strokeWidth);
				Vec2D::subtract(squareA, lastA, strokeDiff);
				Vec2D::subtract(squareB, lastB, strokeDiff);
				m_TriangleStrip.push_back(squareA);
				m_TriangleStrip.push_back(squareB);
				break;
			}
			case StrokeCap::round:
			{
				Vec2D capDirection =
				    Vec2D(-lastDiffNormalized[1], lastDiffNormalized[0]);
				float arcLength = std::abs(M_PI * strokeWidth);
				int steps = (int)std::ceil(arcLength / subdivisionArcLength);
				float angleTo = std::atan2(capDirection[1], capDirection[0]);
				float inc = M_PI / steps;
				float angle = angleTo;
				// make sure to draw the full cap due triangle strip
				for (int j = 0; j <= steps; j++)
				{
					m_TriangleStrip.push_back(lastPoint);
					m_TriangleStrip.push_back(
					    Vec2D(lastPoint[0] + std::cos(angle) * strokeWidth,
					          lastPoint[1] + std::sin(angle) * strokeWidth));
					angle += inc;
				}
				break;
			}
			default:
				break;
		}
	}
	m_TriangleStrip.push_back(lastA);
	m_TriangleStrip.push_back(lastB);

	pointCount -= isClosed ? 6 : 5;
	std::size_t adjustedPointCount = isClosed ? pointCount + 1 : pointCount;

	for (std::size_t i = 1; i < adjustedPointCount; i++)
	{
		const Vec2D& point = points[(i % pointCount) + 4];
		Vec2D diff, diffNormalized, next;
		float length;
		if (i < adjustedPointCount - 1 || isClosed)
		{
			Vec2D::subtract(
			    diff, (next = points[((i + 1) % pointCount) + 4]), point);
			length = Vec2D::length(diff);
			Vec2D::scale(diffNormalized, diff, 1.0f / length);
		}
		else
		{
			diff = lastDiff;
			next = point;
			length = lastLength;
			diffNormalized = lastDiffNormalized;
		}

		// perpendicular dx
		float pdx0 = -lastDiffNormalized[1];
		float pdy0 = lastDiffNormalized[0];
		float pdx1 = -diffNormalized[1];
		float pdy1 = diffNormalized[0];

		// Compute bisector without a normalization by averaging perpendicular
		// diffs.
		Vec2D bisector((pdx0 + pdx1) * 0.5f, (pdy0 + pdy1) * 0.5f);
		float cross = diff[0] * lastDiff[1] - lastDiff[0] * diff[1];

		float dot = Vec2D::dot(bisector, bisector);

		float lengthLimit = std::min(length, lastLength);
		bool bevelInner = false;
		bool bevel = join == StrokeJoin::miter ? dot < 0.1f : dot < 0.999f;

		// Scale bisector to match stroke size.
		if (dot > 0.000001f)
		{
			float scale = 1.0f / dot * strokeWidth;
			float limit = lengthLimit / strokeWidth;
			if (dot * limit * limit < 1.0f)
			{
				bevelInner = true;
			}
			bisector[0] *= scale;
			bisector[1] *= scale;
		}
		else
		{
			bisector[0] *= strokeWidth;
			bisector[1] *= strokeWidth;
		}

		if (!bevel)
		{
			Vec2D c, d;
			Vec2D::add(c, point, bisector);
			Vec2D::subtract(d, point, bisector);

			if (!bevelInner)
			{
				// Normal mitered edge.
				m_TriangleStrip.push_back(c);
				m_TriangleStrip.push_back(d);
			}
			else if (cross <= 0)
			{
				// Overlap the inner (in this case right) edge (sometimes called
				// miter inner).
				Vec2D c1, c2;
				Vec2D::add(c1,
				           point,
				           Vec2D(lastDiffNormalized[1] * -strokeWidth,
				                 lastDiffNormalized[0] * strokeWidth));
				Vec2D::add(c2,
				           point,
				           Vec2D(diffNormalized[1] * -strokeWidth,
				                 diffNormalized[0] * strokeWidth));

				m_TriangleStrip.push_back(c1);
				m_TriangleStrip.push_back(d);
				m_TriangleStrip.push_back(c2);
				m_TriangleStrip.push_back(d);
			}
			else
			{
				// Overlap the inner (in this case left) edge (sometimes called
				// miter inner).
				Vec2D d1, d2;
				Vec2D::subtract(d1,
				                point,
				                Vec2D(lastDiffNormalized[1] * -strokeWidth,
				                      lastDiffNormalized[0] * strokeWidth));
				Vec2D::subtract(d2,
				                point,
				                Vec2D(diffNormalized[1] * -strokeWidth,
				                      diffNormalized[0] * strokeWidth));

				m_TriangleStrip.push_back(c);
				m_TriangleStrip.push_back(d1);
				m_TriangleStrip.push_back(c);
				m_TriangleStrip.push_back(d2);
			}
		}
		else
		{
			Vec2D ldPStroke = Vec2D(lastDiffNormalized[1] * -strokeWidth,
			                        lastDiffNormalized[0] * strokeWidth);
			Vec2D dPStroke = Vec2D(diffNormalized[1] * -strokeWidth,
			                       diffNormalized[0] * strokeWidth);
			if (cross <= 0)
			{
				// Bevel the outer (left in this case) edge.
				Vec2D a1;
				Vec2D a2;

				if (bevelInner)
				{
					Vec2D::add(a1, point, ldPStroke);
					Vec2D::add(a2, point, dPStroke);
				}
				else
				{
					Vec2D::add(a1, point, bisector);
					a2 = a1;
				}

				Vec2D b;
				Vec2D::subtract(b, point, ldPStroke);
				Vec2D bn;
				Vec2D::subtract(bn, point, dPStroke);

				m_TriangleStrip.push_back(a1);
				m_TriangleStrip.push_back(b);
				if (join == StrokeJoin::round)
				{
					const Vec2D& pivot = bevelInner ? point : a1;
					Vec2D toPrev;
					Vec2D::subtract(toPrev, bn, point);
					Vec2D toNext;
					Vec2D::subtract(toNext, b, point);
					float angleFrom = std::atan2(toPrev[1], toPrev[0]);
					float angleTo = std::atan2(toNext[1], toNext[0]);
					if (angleTo > angleFrom)
					{
						angleTo -= M_2_PI;
					}
					float range = angleTo - angleFrom;
					float arcLength = std::abs(range * strokeWidth);
					int steps = std::ceil(arcLength / subdivisionArcLength);

					float inc = range / steps;
					float angle = angleTo - inc;
					for (int j = 0; j < steps - 1; j++)
					{
						m_TriangleStrip.push_back(pivot);
						m_TriangleStrip.emplace_back(
						    Vec2D(point[0] + std::cos(angle) * strokeWidth,
						          point[1] + std::sin(angle) * strokeWidth));

						angle -= inc;
					}
				}
				m_TriangleStrip.push_back(a2);
				m_TriangleStrip.push_back(bn);
			}
			else
			{
				// Bevel the outer (right in this case) edge.
				Vec2D b1;
				Vec2D b2;
				if (bevelInner)
				{
					Vec2D::subtract(b1, point, ldPStroke);
					Vec2D::subtract(b2, point, dPStroke);
				}
				else
				{
					Vec2D::subtract(b1, point, bisector);
					b2 = b1;
				}

				Vec2D a;
				Vec2D::add(a, point, ldPStroke);
				Vec2D an;

				Vec2D::add(an, point, dPStroke);

				m_TriangleStrip.push_back(a);
				m_TriangleStrip.push_back(b1);

				if (join == StrokeJoin::round)
				{
					const Vec2D& pivot = bevelInner ? point : b1;
					Vec2D toPrev;
					Vec2D::subtract(toPrev, a, point);
					Vec2D toNext;
					Vec2D::subtract(toNext, an, point);
					float angleFrom = std::atan2(toPrev[1], toPrev[0]);
					float angleTo = std::atan2(toNext[1], toNext[0]);
					if (angleTo > angleFrom)
					{
						angleTo -= M_2_PI;
					}

					float range = angleTo - angleFrom;
					float arcLength = std::abs(range * strokeWidth);
					int steps = std::ceil(arcLength / subdivisionArcLength);
					float inc = range / steps;

					float angle = angleFrom + inc;
					for (int j = 0; j < steps - 1; j++)
					{
						m_TriangleStrip.emplace_back(
						    Vec2D(point[0] + std::cos(angle) * strokeWidth,
						          point[1] + std::sin(angle) * strokeWidth));
						m_TriangleStrip.push_back(pivot);
						angle += inc;
					}
				}
				m_TriangleStrip.push_back(an);
				m_TriangleStrip.push_back(b2);
			}
		}

		lastPoint = point;
		lastDiff = diff;
		lastDiffNormalized = diffNormalized;
	}

	if (isClosed)
	{
		auto last = m_TriangleStrip.size() - 1;
		m_TriangleStrip[startOffset] = m_TriangleStrip[last - 1];
		m_TriangleStrip[startOffset + 1] = m_TriangleStrip[last];
	}
	else
	{
		switch (cap)
		{
			case StrokeCap::square:
			{
				auto l = m_TriangleStrip.size();
				Vec2D squareA, squareB;
				Vec2D strokeDiff = Vec2D(lastDiffNormalized[0] * strokeWidth,
				                         lastDiffNormalized[1] * strokeWidth);
				Vec2D::add(squareA, m_TriangleStrip[l - 2], strokeDiff);
				Vec2D::add(squareB, m_TriangleStrip[l - 1], strokeDiff);
				m_TriangleStrip.push_back(squareA);
				m_TriangleStrip.push_back(squareB);
				break;
			}
			case StrokeCap::round:
			{
				Vec2D capDirection =
				    Vec2D(-lastDiffNormalized[1], lastDiffNormalized[0]);
				float arcLength = std::abs(M_PI * strokeWidth);
				int steps = (int)std::ceil(arcLength / subdivisionArcLength);
				float angleTo = std::atan2(capDirection[1], capDirection[0]);
				float inc = M_PI / steps;
				float angle = angleTo;
				// make sure to draw the full cap due triangle strip
				for (int j = 0; j <= steps; j++)
				{
					m_TriangleStrip.push_back(lastPoint);
					m_TriangleStrip.push_back(
					    Vec2D(lastPoint[0] + std::cos(angle) * strokeWidth,
					          lastPoint[1] + std::sin(angle) * strokeWidth));
					angle -= inc;
				}
				break;
			}
			default:
				break;
		}
	}

	m_Offsets.push_back(m_TriangleStrip.size());
}
#endif
