#include "metal/metal_renderer.hpp"
#include "metal/metal_render_path.hpp"
#include "metal/metal_render_paint.hpp"

using namespace rive;
MetalRenderer::~MetalRenderer() {}
void MetalRenderer::save() {}
void MetalRenderer::restore() {}
void MetalRenderer::transform(const Mat2D& transform) {}
void MetalRenderer::drawPath(RenderPath* path, RenderPaint* paint) {}
void MetalRenderer::drawImage(RenderImage* image,
                              BlendMode value,
                              float opacity)
{
}
void MetalRenderer::clipPath(RenderPath* path) {}

void MetalRenderer::startFrame() {}
void MetalRenderer::endFrame() {}

RenderPaint* MetalRenderer::makeRenderPaint() { return new MetalRenderPaint(); }
RenderPath* MetalRenderer::makeRenderPath() { return new MetalRenderPath(); }
RenderImage* MetalRenderer::makeRenderImage() { return new MetalRenderImage(); }

static const char kShaderSource[] =
    "#include <metal_stdlib>\n"
    "using namespace metal;\n"
    "struct Vertex\n"
    "{\n"
    "    packed_float2 position;\n"
    "};\n"
    "struct VSOutput\n"
    "{\n"
    "    float4 pos [[position]];\n"
    "};\n"
    "struct FSOutput\n"
    "{\n"
    "    half4 frag_data [[color(0)]];\n"
    "};\n"
    "vertex VSOutput vertexMain(device Vertex* vertexBuffer "
    "[[buffer(0)]], uint vertexId [[vertex_id]])\n"
    "{\n"
    "    VSOutput out = { float4(vertexBuffer[vertexId].position, 0, 1),};\n"
    "    return out;\n"
    "}\n"
    "fragment FSOutput fragmentMain(VSOutput input [[stage_in]])\n"
    "{\n"
    "    FSOutput out = { half4(1, 1, 0, 1)};\n"
    "    return out;\n"
    "}\n";

bool MetalRenderer::initialize(void* data)
{
	auto metalDevice = acquireDevice();

	// Create shaders
	NSError* error = nil;
	NSString* srcStr = [[NSString alloc] initWithBytes:kShaderSource
	                                            length:sizeof(kShaderSource)
	                                          encoding:NSASCIIStringEncoding];
	id<MTLLibrary> shaderLibrary = [metalDevice newLibraryWithSource:srcStr
	                                                         options:nil
	                                                           error:&error];
	if (error != nil)
	{
		NSString* desc = [error localizedDescription];
		NSString* reason = [error localizedFailureReason];
		::fprintf(stderr,
		          "%s\n%s\n\n",
		          desc ? [desc UTF8String] : "<unknown>",
		          reason ? [reason UTF8String] : "");
	}

	id<MTLFunction> vertexFunction =
	    [shaderLibrary newFunctionWithName:@"vertexMain"];
	id<MTLFunction> fragmentFunction =
	    [shaderLibrary newFunctionWithName:@"fragmentMain"];

	MTLRenderPipelineDescriptor* pipeDesc =
	    [[MTLRenderPipelineDescriptor alloc] init];
	// Let's assume we're rendering into BGRA8Unorm...
	pipeDesc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;

	// pipeDesc.depthAttachmentPixelFormat =
	// MTLPixelFormatDepth32Float_Stencil8;
	// pipeDesc.stencilAttachmentPixelFormat =
	// MTLPixelFormatDepth32Float_Stencil8;

	pipeDesc.sampleCount = 1;
	pipeDesc.colorAttachments[0].blendingEnabled = NO;

	pipeDesc.vertexFunction = vertexFunction;
	pipeDesc.fragmentFunction = fragmentFunction;

	m_Pipeline = [metalDevice newRenderPipelineStateWithDescriptor:pipeDesc
	                                                         error:&error];
	if (error != nil)
	{
		::fprintf(stderr,
		          "Metal: Error creating pipeline state: %s\n%s\n",
		          [[error localizedDescription] UTF8String],
		          [[error localizedFailureReason] UTF8String]);
		error = nil;
	}

	MTLDepthStencilDescriptor* depthDesc =
	    [[MTLDepthStencilDescriptor alloc] init];
	depthDesc.depthCompareFunction = MTLCompareFunctionAlways;
	depthDesc.depthWriteEnabled = false;
	m_DepthStencil = [metalDevice newDepthStencilStateWithDescriptor:depthDesc];

	float fillScreenVertices[] = {1.0f,
	                              1.0f,

	                              1.0f,
	                              -1.0f,

	                              -1.0f,
	                              1.0f,

	                              -1.0f,
	                              -1.0f};
	m_FillScreenVertexBuffer =
	    [metalDevice newBufferWithBytes:fillScreenVertices
	                             length:sizeof(fillScreenVertices)
	                            options:MTLResourceOptionCPUCacheModeDefault];
	return true;
}

void MetalRenderer::fillScreen()
{
	auto encoder = currentCommandEncoder();
	[encoder setRenderPipelineState:m_Pipeline];
	[encoder setVertexBuffer:m_FillScreenVertexBuffer offset:0 atIndex:0];

	[encoder drawPrimitives:MTLPrimitiveTypeTriangleStrip
	            vertexStart:0
	            vertexCount:4
	          instanceCount:1];
}