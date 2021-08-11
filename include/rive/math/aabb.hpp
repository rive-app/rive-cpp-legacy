#ifndef _RIVE_AABB_HPP_
#define _RIVE_AABB_HPP_

#include "rive/math/mat2d.hpp"
#include "rive/math/vec2d.hpp"
#include <cstddef>
#include <limits>

namespace rive
{
	class AABB
	{
	public:
		union
		{
			float buffer[4];
			struct
			{
				float minX, minY, maxX, maxY;
			};
		};

	public:
		AABB();
		AABB(const AABB& copy);
		AABB(float minX, float minY, float maxX, float maxY);

		inline const float* values() const { return buffer; }

		float& operator[](std::size_t idx) { return buffer[idx]; }
		const float& operator[](std::size_t idx) const { return buffer[idx]; }

		static void center(Vec2D& out, const AABB& a);
		static void size(Vec2D& out, const AABB& a);
		static void extents(Vec2D& out, const AABB& a);
		static void combine(AABB& out, const AABB& a, const AABB& b);
		static bool contains(const AABB& a, const AABB& b);
		static bool isValid(const AABB& a);
		static bool testOverlap(const AABB& a, const AABB& b);
		static bool areIdentical(const AABB& a, const AABB& b);
		static void transform(AABB& out, const AABB& a, const Mat2D& matrix);
		static void copy(AABB& out, const AABB& a);

		///
		/// Grow the AABB to fit the point.
		///
		static void expandTo(AABB& out, const Vec2D& point);
		static void expandTo(AABB& out, float x, float y);

		float width() const;
		float height() const;
		float perimeter() const;

		///
		/// Initialize an AABB to values that represent an invalid/collapsed
		/// AABB that can then expand to points that are added to it.
		///
		inline static AABB forExpansion()
		{
			return AABB(std::numeric_limits<float>::max(),
			            std::numeric_limits<float>::max(),
			            -std::numeric_limits<float>::max(),
			            -std::numeric_limits<float>::max());
		}
	};
} // namespace rive
#endif
