#ifdef RIVE_HAS_METAL
#include "metal/metal_renderer.hpp"

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#import <AppKit/NSWindow.h>

/// Implemented in viewer.cpp, must be available before instancing the
/// MetalRenderer.
extern void* viewerNativeWindowHandle;

namespace rive
{
	class ViewerMetalRenderer : public MetalRenderer
	{
	private:
		id<CAMetalDrawable> m_FrameDrawable;
		id<MTLCommandBuffer> m_FrameCommandBuffer;
		id<MTLRenderCommandEncoder> m_FrameCommandEncoder;
		id<MTLCommandQueue> m_CommandQueue;
		CAMetalLayer* m_MetalLayer;

	public:
		id<MTLDevice> acquireDevice() override
		{
			const id<MTLDevice> gpu = MTLCreateSystemDefaultDevice();
			m_MetalLayer = [CAMetalLayer layer];
			m_MetalLayer.device = gpu;
			m_MetalLayer.opaque = YES;
			NSWindow* nswindow =
			    static_cast<NSWindow*>(viewerNativeWindowHandle);
			nswindow.contentView.layer = m_MetalLayer;
			nswindow.contentView.wantsLayer = YES;
			m_CommandQueue = [gpu newCommandQueue];

			return gpu;
		}

		id<MTLRenderCommandEncoder> currentCommandEncoder() override
		{
			return m_FrameCommandEncoder;
		}

		void startFrame() override
		{
			m_FrameDrawable = [m_MetalLayer nextDrawable];
			// Get the command buffer from the command queue.
			m_FrameCommandBuffer = [m_CommandQueue commandBuffer];

			// Get the texture for the next frame.
			id<MTLTexture> texture = m_FrameDrawable.texture;

			MTLRenderPassDescriptor* passDescriptor =
			    [MTLRenderPassDescriptor renderPassDescriptor];
			passDescriptor.colorAttachments[0].texture = texture;
			passDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
			passDescriptor.colorAttachments[0].storeAction =
			    MTLStoreActionStore;
			passDescriptor.colorAttachments[0].clearColor =
			    MTLClearColorMake(0.0, 0.0, 0.0, 0.0);
			// Get the encoder which we will use for all draw calls on the main
			// buffer.
			m_FrameCommandEncoder = [m_FrameCommandBuffer
			    renderCommandEncoderWithDescriptor:passDescriptor];
			fillScreen();
		}

		void endFrame() override
		{
			[m_FrameCommandEncoder endEncoding];

			[m_FrameCommandBuffer presentDrawable:m_FrameDrawable];
			[m_FrameCommandBuffer commit];
		}
	};

	LowLevelRenderer* makeRendererMetal() { return new ViewerMetalRenderer(); }
} // namespace rive
#endif