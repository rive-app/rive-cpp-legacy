#ifndef _RIVE_METAL_RENDERER_HPP_
#define _RIVE_METAL_RENDERER_HPP_

#include "low_level/low_level_renderer.hpp"

#ifdef __OBJC__
#import <Metal/Metal.h>
#endif

namespace rive
{
	class MetalRenderer : public LowLevelRenderer
	{
	public:
		~MetalRenderer();
		void save() override;
		void restore() override;
		void transform(const Mat2D& transform) override;
		void drawPath(RenderPath* path, RenderPaint* paint) override;
		void clipPath(RenderPath* path) override;

		void onViewportSizeChanged(ViewportSize from, ViewportSize to) override;
		void clear() override;
		void frame() override;
	};

} // namespace rive
#endif