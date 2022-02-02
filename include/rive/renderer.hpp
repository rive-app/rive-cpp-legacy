#ifndef _RIVE_RENDERER_HPP_
#define _RIVE_RENDERER_HPP_

#include "rive/command_path.hpp"
#include "rive/layout.hpp"
#include "rive/math/aabb.hpp"
#include "rive/math/mat2d.hpp"
#include "rive/refcnt.hpp"
#include "rive/shapes/paint/blend_mode.hpp"
#include "rive/shapes/paint/stroke_cap.hpp"
#include "rive/shapes/paint/stroke_join.hpp"
#include <cmath>
#include <stdio.h>
#include <cstdint>

namespace rive
{
	class Vec2D;

    using ColorInt = uint32_t;

    enum class RenderTileMode {
        clamp,
        repeat,
        mirror,
        decal,  // fill outside the domain with transparent
    };

    /*
     *  Base class for Render objects that specify the src colors.
     *
     *  Shaders are immutable, and sharable between multiple paints, etc.
     *
     *  It is common that a shader may be created with a 'localMatrix'. If this is
     *  not null, then it is applied to the shader's domain before the Renderer's CTM.
     */
    class RenderShader : public RefCnt {};

    extern rcp<RenderShader> makeLinearGradient(float sx, float sy,
                                                float ex, float ey,
                                                const ColorInt colors[],    // [count]
                                                const float stops[],        // [count]
                                                int count,
                                                RenderTileMode,
                                                const Mat2D* localMatrix = nullptr);

    extern rcp<RenderShader> makeRadialGradient(float cx, float cy, float radius,
                                                const ColorInt colors[],    // [count]
                                                const float stops[],        // [count]
                                                int count,
                                                RenderTileMode,
                                                const Mat2D* localMatrix = nullptr);

    extern rcp<RenderShader> makeSweepGradient(float cx, float cy,
                                               const ColorInt colors[],    // [count]
                                               const float stops[],        // [count]
                                               int count,
                                               const Mat2D* localMatrix = nullptr);

      enum class RenderPaintStyle
      {
      		stroke,
       		fill
      };

    class RenderPaint {
    public:
        rcp<RenderShader>   shader;
        ColorInt            color;
        BlendMode           blendMode;

        float               thickness;
        StrokeJoin          join;
        StrokeCap           cap;
        RenderPaintStyle    style;
    };

	class RenderImage
	{
	protected:
		int m_Width = 0;
		int m_Height = 0;

	public:
		virtual ~RenderImage() {}
		virtual bool decode(const uint8_t* bytes, std::size_t size) = 0;
		int width() const { return m_Width; }
		int height() const { return m_Height; }
	};

	class RenderPath : public CommandPath
	{
	public:
		RenderPath* renderPath() override { return this; }
		void addPath(CommandPath* path, const Mat2D& transform) override
		{
			addRenderPath(path->renderPath(), transform);
		}

		virtual void addRenderPath(RenderPath* path,
		                           const Mat2D& transform) = 0;
	};

	class Renderer
	{
	public:
		virtual ~Renderer() {}
		virtual void save() = 0;
		virtual void restore() = 0;
		virtual void transform(const Mat2D& transform) = 0;
		virtual void drawPath(RenderPath* path, RenderPaint* paint) = 0;
		virtual void clipPath(RenderPath* path) = 0;
		virtual void
		drawImage(RenderImage* image, BlendMode value, float opacity) = 0;

		void computeAlignment(Mat2D& result,
		                      Fit fit,
		                      const Alignment& alignment,
		                      const AABB& frame,
		                      const AABB& content)
		{
			float contentWidth = content[2] - content[0];
			float contentHeight = content[3] - content[1];
			float x = -content[0] - contentWidth / 2.0 -
			          (alignment.x() * contentWidth / 2.0);
			float y = -content[1] - contentHeight / 2.0 -
			          (alignment.y() * contentHeight / 2.0);

			float scaleX = 1.0, scaleY = 1.0;

			switch (fit)
			{
				case Fit::fill:
				{
					scaleX = frame.width() / contentWidth;
					scaleY = frame.height() / contentHeight;
					break;
				}
				case Fit::contain:
				{
					float minScale = std::fmin(frame.width() / contentWidth,
					                           frame.height() / contentHeight);
					scaleX = scaleY = minScale;
					break;
				}
				case Fit::cover:
				{
					float maxScale = std::fmax(frame.width() / contentWidth,
					                           frame.height() / contentHeight);
					scaleX = scaleY = maxScale;
					break;
				}
				case Fit::fitHeight:
				{
					float minScale = frame.height() / contentHeight;
					scaleX = scaleY = minScale;
					break;
				}
				case Fit::fitWidth:
				{
					float minScale = frame.width() / contentWidth;
					scaleX = scaleY = minScale;
					break;
				}
				case Fit::none:
				{
					scaleX = scaleY = 1.0;
					break;
				}
				case Fit::scaleDown:
				{
					float minScale = std::fmin(frame.width() / contentWidth,
					                           frame.height() / contentHeight);
					scaleX = scaleY = minScale < 1.0 ? minScale : 1.0;
					break;
				}
			}

			Mat2D translation;
			translation[4] = frame[0] + frame.width() / 2.0 +
			                 (alignment.x() * frame.width() / 2.0);
			translation[5] = frame[1] + frame.height() / 2.0 +
			                 (alignment.y() * frame.height() / 2.0);
			Mat2D scale;
			scale[0] = scaleX;
			scale[3] = scaleY;

			Mat2D translateBack;
			translateBack[4] = x;
			translateBack[5] = y;

			Mat2D::multiply(result, translation, scale);
			Mat2D::multiply(result, result, translateBack);
		}

		void align(Fit fit,
		           const Alignment& alignment,
		           const AABB& frame,
		           const AABB& content)
		{
			Mat2D result;
			computeAlignment(result, fit, alignment, frame, content);
			transform(result);
		}
	};

	extern RenderPath* makeRenderPath();
	extern RenderPaint* makeRenderPaint();
	extern RenderImage* makeRenderImage();
} // namespace rive
#endif
