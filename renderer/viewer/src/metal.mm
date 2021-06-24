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
	public:
		id<MTLDevice> acquireDevice() override
		{
			const id<MTLDevice> gpu = MTLCreateSystemDefaultDevice();
			CAMetalLayer* swapchain = [CAMetalLayer layer];
			swapchain.device = gpu;
			swapchain.opaque = YES;
			NSWindow* nswindow =
			    static_cast<NSWindow*>(viewerNativeWindowHandle);
			nswindow.contentView.layer = swapchain;
			nswindow.contentView.wantsLayer = YES;
			return gpu;
		}
	};

	LowLevelRenderer* makeRendererMetal() { return new ViewerMetalRenderer(); }
} // namespace rive
#endif