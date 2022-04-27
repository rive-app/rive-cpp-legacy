#ifndef _RIVE_AABB_HPP_
#define _RIVE_AABB_HPP_

#include "rive/span.hpp"
#include "rive/math/mat2d.hpp"
#include "rive/math/vec2d.hpp"
#include <cstddef>

namespace rive {
    struct IAABB {
        int32_t left, top, right, bottom;

        constexpr int width() const { return right - left; }
        constexpr int height() const { return bottom - top; }
        constexpr bool empty() const { return width() <= 0 || height() <= 0; }

        IAABB inset(int dx, int dy) const { return {left + dx, top + dy, right - dx, bottom - dy}; }
        IAABB offset(int dx, int dy) const {
            return {left + dx, top + dy, right + dx, bottom + dy};
        }
    };

    class AABB {
    public:
        float minX, minY, maxX, maxY;

        AABB() : minX(0), minY(0), maxX(0), maxY(0) {}
        AABB(const AABB& o) : minX(o.minX), minY(o.minY), maxX(o.maxX), maxY(o.maxY) {}

        AABB(float minX, float minY, float maxX, float maxY) :
            minX(minX), minY(minY), maxX(maxX), maxY(maxY) {}

        AABB(const IAABB& o) : AABB((float)o.left, (float)o.top, (float)o.right, (float)o.bottom) {}

        AABB(Span<Vec2D>); // computes the union of all points, or 0,0,0,0

        bool operator==(const AABB& o) const {
            return minX == o.minX && minY == o.minY && maxX == o.maxX && maxY == o.maxY;
        }
        bool operator!=(const AABB& o) const { return !(*this == o); }

        float left() const { return minX; }
        float top() const { return minY; }
        float right() const { return maxX; }
        float bottom() const { return maxY; }

        float width() const { return maxX - minX; }
        float height() const { return maxY - minY; }
        Vec2D size() const { return {width(), height()}; }
        Vec2D center() const { return {(minX + maxX) * 0.5f, (minY + maxY) * 0.5f}; }

        AABB inset(float dx, float dy) const {
            AABB r = {minX + dx, minY + dy, maxX - dx, maxY - dy};
            assert(r.width() >= 0);
            assert(r.height() >= 0);
            return r;
        }
        AABB offset(float dx, float dy) const {
            return {minX + dx, minY + dy, maxX + dx, maxY + dy};
        }

        IAABB round() const;
    };

} // namespace rive
#endif
