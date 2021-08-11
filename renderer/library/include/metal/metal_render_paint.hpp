#ifndef _RIVE_METAL_RENDER_PAINT_HPP_
#define _RIVE_METAL_RENDER_PAINT_HPP_

#include "rive/renderer.hpp"

namespace rive
{
	class MetalRenderPaint : public RenderPaint
	{
	private:
		RenderPaintStyle m_PaintStyle;

	public:
		void style(RenderPaintStyle style) override;
		RenderPaintStyle style() const { return m_PaintStyle; }
		void color(unsigned int value) override;
		void thickness(float value) override;
		void join(StrokeJoin value) override;
		void cap(StrokeCap value) override;
		void blendMode(BlendMode value) override;

		void linearGradient(float sx, float sy, float ex, float ey) override;
		void radialGradient(float sx, float sy, float ex, float ey) override;
		void addStop(unsigned int color, float stop) override;
		void completeGradient() override;
		void invalidateStroke() override;
		~MetalRenderPaint();
	};
} // namespace rive
#endif