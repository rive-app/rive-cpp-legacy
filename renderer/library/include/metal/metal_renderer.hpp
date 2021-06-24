#ifndef _RIVE_METAL_RENDERER_HPP_
#define _RIVE_METAL_RENDERER_HPP_

#include "low_level/low_level_renderer.hpp"

#ifndef __OBJC__
#error MetalRenderer can only be included from Objective-C files.
#endif
#ifndef __clang__
#error MetalRenderer can only be compiled with Clang.
#endif

#import <Metal/Metal.h>

namespace rive
{
	class MetalRenderer : public LowLevelRenderer
	{
	public:
		GraphicsApi::Type type() const override { return GraphicsApi::metal; }
		~MetalRenderer();
		void save() override;
		void restore() override;
		void transform(const Mat2D& transform) override;
		void drawPath(RenderPath* path, RenderPaint* paint) override;
		void clipPath(RenderPath* path) override;

		void startFrame() override;
		void endFrame() override;

		RenderPaint* makeRenderPaint() override;
		RenderPath* makeRenderPath() override;
		bool initialize(void* data) override;

		virtual id<MTLDevice> acquireDevice() = 0;
		virtual id<MTLRenderCommandEncoder> currentCommandEncoder() = 0;

	private:
		id<MTLBuffer> m_FillScreenVertexBuffer;
		id<MTLRenderPipelineState> m_Pipeline;
		id<MTLDepthStencilState> m_DepthStencil;

	protected:
		void fillScreen();
	};

} // namespace rive
#endif