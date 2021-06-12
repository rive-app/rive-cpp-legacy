#include "diligent_renderer.hpp"
#include "diligent_render_paint.hpp"
#include "diligent_render_path.hpp"
#include "DiligentCore/Graphics/GraphicsTools/interface/MapHelper.hpp"
#include "DiligentCore/Graphics/GraphicsTools/interface/GraphicsUtilities.h"
#include "DiligentCore/Common/interface/BasicMath.hpp"

using namespace rive;

static const char* stencilVertexSource = R"(
cbuffer Constants
{
    float4x4 worldViewProj;
};

struct VertexInput
{
    float2 position : ATTRIB0;
};

struct FragmentInput 
{ 
    float4 position : SV_POSITION; 
};

void main(in  VertexInput input,
          out FragmentInput ouput) 
{
    ouput.position = mul( float4(input.position,0.0, 1.0), worldViewProj );
}

)";

static const char* stencilFragmentSource = R"(

struct FragmentInput 
{ 
    float4 position : SV_POSITION; 
};

struct FragmentOutput
{ 
    float4 color : SV_TARGET; 
};

void main(in  FragmentInput  input,
          out FragmentOutput ouput)
{
    ouput.color = float4(1.0, 1.0, 1.0, 1.0);
}
)";

DiligentRenderer::~DiligentRenderer() {}
void DiligentRenderer::save() {}
void DiligentRenderer::restore() {}
void DiligentRenderer::transform(const Mat2D& transform) {}
void DiligentRenderer::clipPath(RenderPath* path) {}
void DiligentRenderer::initializePipeline()
{
	// Pipeline state object encompasses configuration of all GPU stages
	GraphicsPipelineStateCreateInfo coverPSO;

	// Pipeline state name is used by the engine to report issues.
	// It is always a good idea to give objects descriptive names.
	coverPSO.PSODesc.Name = "Cover PSO";

	// This is a graphics pipeline
	coverPSO.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

	auto& graphics = coverPSO.GraphicsPipeline;
	graphics.NumRenderTargets = 1;
	// Set render target format which is the format of the swap chain's color
	// buffer
	graphics.RTVFormats[0] = m_pSwapChain->GetDesc().ColorBufferFormat;
	// Use the depth buffer format from the swap chain
	graphics.DSVFormat = m_pSwapChain->GetDesc().DepthBufferFormat;
	// Primitive topology defines what kind of primitives will be rendered by
	// this pipeline state
	graphics.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	// No back face culling for this tutorial
	graphics.RasterizerDesc.CullMode = CULL_MODE_NONE;
	// Disable depth testing
	graphics.DepthStencilDesc.DepthEnable = False;

	ShaderCreateInfo shaderCI;
	// Tell the system that the shader source code is in HLSL.
	// For OpenGL, the engine will convert this into GLSL under the hood.
	shaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
	// OpenGL backend requires emulated combined HLSL texture samplers
	// (g_Texture + g_Texture_sampler combination)
	shaderCI.UseCombinedTextureSamplers = true;
	// Create a vertex shader
	RefCntAutoPtr<IShader> vertexShader;
	{
		shaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
		shaderCI.EntryPoint = "main";
		shaderCI.Desc.Name = "Stencil vertex shader";
		shaderCI.Source = stencilVertexSource;
		m_pDevice->CreateShader(shaderCI, &vertexShader);
	}

	// Create a pixel shader
	RefCntAutoPtr<IShader> fragmentShader;
	{
		shaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
		shaderCI.EntryPoint = "main";
		shaderCI.Desc.Name = "Stencil fragment shader";
		shaderCI.Source = stencilFragmentSource;
		m_pDevice->CreateShader(shaderCI, &fragmentShader);
	}

	// Finally, create the pipeline state
	coverPSO.pVS = vertexShader;
	coverPSO.pPS = fragmentShader;
	m_pDevice->CreateGraphicsPipelineState(coverPSO, &m_CoverPSO);

	BufferDesc CBDesc;
	CBDesc.Name = "VS constants CB";
	CBDesc.uiSizeInBytes = sizeof(float4x4);
	CBDesc.Usage = USAGE_DYNAMIC;
	CBDesc.BindFlags = BIND_UNIFORM_BUFFER;
	CBDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
	m_pDevice->CreateBuffer(CBDesc, nullptr, &m_StencilConstants);

	// Tell the stencile pipeline that we have constants.
	m_CoverPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")
	    ->Set(m_StencilConstants);

	// Create a shader resource binding object and bind all static resources in
	// it
	m_CoverPSO->CreateShaderResourceBinding(&m_StencilResourceBinding, true);
}

void DiligentRenderer::drawPath(RenderPath* path, RenderPaint* paint)
{
	// Currently only support fills.
	auto diligentRenderPaint = reinterpret_cast<DiligentRenderPaint*>(paint);
	if (diligentRenderPaint->style() == RenderPaintStyle::stroke)
	{
		return;
	}
}
namespace rive
{
	RenderPath* makeRenderPath() { return new DiligentRenderPath(); }
	RenderPaint* makeRenderPaint() { return new DiligentRenderPaint(); }
} // namespace rive