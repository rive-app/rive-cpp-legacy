#include "base_diligent_renderer.hpp"

#if PLATFORM_MACOS
#include "DiligentCore/Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h"
#endif

// Mostly from
// https://github.com/DiligentGraphics/DiligentSamples/blob/master/SampleBase/src/SampleApp.cpp

using namespace rive;

BaseDiligentRenderer::~BaseDiligentRenderer() {}

RENDER_DEVICE_TYPE BaseDiligentRenderer::deviceType() const
{
	return m_DeviceType;
}

void BaseDiligentRenderer::deviceType(RENDER_DEVICE_TYPE value)
{
	m_DeviceType = value;
}

void BaseDiligentRenderer::initialize(const NativeWindow* window)
{
	SwapChainDesc SCDesc;

#if PLATFORM_MACOS
	// We need at least 3 buffers on Metal to avoid massive
	// peformance degradation in full screen mode.
	// https://github.com/KhronosGroup/MoltenVK/issues/808
	SCDesc.BufferCount = 3;
#endif

	std::vector<IDeviceContext*> ppContexts;
	int NumImmediateContexts = 0;
	switch (m_DeviceType)
	{
#if D3D11_SUPPORTED
		case RENDER_DEVICE_TYPE_D3D11:
		{
			EngineD3D11CreateInfo EngineCI;

			if (m_ValidationLevel >= 1)
			{
				EngineCI.DebugFlags =
				    D3D11_DEBUG_FLAG_CREATE_DEBUG_DEVICE |
				    D3D11_DEBUG_FLAG_VERIFY_COMMITTED_SHADER_RESOURCES |
				    D3D11_DEBUG_FLAG_VERIFY_COMMITTED_RESOURCE_RELEVANCE;
			}
			else if (m_ValidationLevel == 0)
			{
				EngineCI.DebugFlags = D3D11_DEBUG_FLAG_NONE;
			}

#if ENGINE_DLL
			// Load the dll and import GetEngineFactoryD3D11() function
			auto GetEngineFactoryD3D11 = LoadGraphicsEngineD3D11();
#endif
			auto* pFactoryD3D11 = GetEngineFactoryD3D11();
			m_pEngineFactory = pFactoryD3D11;
			Uint32 NumAdapters = 0;
			pFactoryD3D11->EnumerateAdapters(
			    EngineCI.MinimumFeatureLevel, NumAdapters, 0);
			std::vector<GraphicsAdapterInfo> Adapters(NumAdapters);
			if (NumAdapters > 0)
			{
				pFactoryD3D11->EnumerateAdapters(
				    EngineCI.MinimumFeatureLevel, NumAdapters, Adapters.data());
			}
			else
			{
			}

			if (m_AdapterType == ADAPTER_TYPE_SOFTWARE)
			{
				for (Uint32 i = 0; i < Adapters.size(); ++i)
				{
					if (Adapters[i].AdapterType == m_AdapterType)
					{
						m_AdapterId = i;
						break;
					}
				}
			}

			m_AdapterAttribs = Adapters[m_AdapterId];
			if (m_AdapterType != ADAPTER_TYPE_SOFTWARE)
			{
				Uint32 NumDisplayModes = 0;
				pFactoryD3D11->EnumerateDisplayModes(
				    EngineCI.MinimumFeatureLevel,
				    m_AdapterId,
				    0,
				    TEX_FORMAT_RGBA8_UNORM_SRGB,
				    NumDisplayModes,
				    nullptr);
				m_DisplayModes.resize(NumDisplayModes);
				pFactoryD3D11->EnumerateDisplayModes(
				    EngineCI.MinimumFeatureLevel,
				    m_AdapterId,
				    0,
				    TEX_FORMAT_RGBA8_UNORM_SRGB,
				    NumDisplayModes,
				    m_DisplayModes.data());
			}

			EngineCI.AdapterId = m_AdapterId;
			ppContexts.resize(1 + EngineCI.NumDeferredContexts);
			pFactoryD3D11->CreateDeviceAndContextsD3D11(
			    EngineCI, &m_pDevice, ppContexts.data());

			if (window != nullptr)
				pFactoryD3D11->CreateSwapChainD3D11(m_pDevice,
				                                    ppContexts[0],
				                                    SCDesc,
				                                    FullScreenModeDesc{},
				                                    *window,
				                                    &m_pSwapChain);
		}
		break;
#endif

#if D3D12_SUPPORTED
		case RENDER_DEVICE_TYPE_D3D12:
		{
			EngineD3D12CreateInfo EngineCI;

			if (m_ValidationLevel >= 1)
			{
				EngineCI.EnableDebugLayer = true;
				if (m_ValidationLevel >= 2)
					EngineCI.EnableGPUBasedValidation = true;
			}
			else if (m_ValidationLevel == 0)
			{
				EngineCI.EnableDebugLayer = false;
			}

#if ENGINE_DLL
			// Load the dll and import GetEngineFactoryD3D12() function
			auto GetEngineFactoryD3D12 = LoadGraphicsEngineD3D12();
#endif
			auto* pFactoryD3D12 = GetEngineFactoryD3D12();
			if (!pFactoryD3D12->LoadD3D12())
			{
			}

			m_pEngineFactory = pFactoryD3D12;
			Uint32 NumAdapters = 0;
			pFactoryD3D12->EnumerateAdapters(
			    EngineCI.MinimumFeatureLevel, NumAdapters, 0);
			std::vector<GraphicsAdapterInfo> Adapters(NumAdapters);
			if (NumAdapters > 0)
			{
				pFactoryD3D12->EnumerateAdapters(
				    EngineCI.MinimumFeatureLevel, NumAdapters, Adapters.data());
			}
			else
			{
#if D3D11_SUPPORTED
				m_DeviceType = RENDER_DEVICE_TYPE_D3D11;
				InitializeEngine(window);
				return;
#endif
			}

			if (m_AdapterType == ADAPTER_TYPE_SOFTWARE)
			{
				for (Uint32 i = 0; i < Adapters.size(); ++i)
				{
					if (Adapters[i].AdapterType == m_AdapterType)
					{
						m_AdapterId = i;
						break;
					}
				}
			}

			m_AdapterAttribs = Adapters[m_AdapterId];
			if (m_AdapterType != ADAPTER_TYPE_SOFTWARE)
			{
				Uint32 NumDisplayModes = 0;
				pFactoryD3D12->EnumerateDisplayModes(
				    EngineCI.MinimumFeatureLevel,
				    m_AdapterId,
				    0,
				    TEX_FORMAT_RGBA8_UNORM_SRGB,
				    NumDisplayModes,
				    nullptr);
				m_DisplayModes.resize(NumDisplayModes);
				pFactoryD3D12->EnumerateDisplayModes(
				    EngineCI.MinimumFeatureLevel,
				    m_AdapterId,
				    0,
				    TEX_FORMAT_RGBA8_UNORM_SRGB,
				    NumDisplayModes,
				    m_DisplayModes.data());
			}

			EngineCI.AdapterId = m_AdapterId;
			ppContexts.resize(1 + EngineCI.NumDeferredContexts);
			pFactoryD3D12->CreateDeviceAndContextsD3D12(
			    EngineCI, &m_pDevice, ppContexts.data());

			if (!m_pSwapChain && window != nullptr)
				pFactoryD3D12->CreateSwapChainD3D12(m_pDevice,
				                                    ppContexts[0],
				                                    SCDesc,
				                                    FullScreenModeDesc{},
				                                    *window,
				                                    &m_pSwapChain);
		}
		break;
#endif

#if GL_SUPPORTED || GLES_SUPPORTED
		case RENDER_DEVICE_TYPE_GL:
		case RENDER_DEVICE_TYPE_GLES:
		{
#if !PLATFORM_MACOS
			VERIFY_EXPR(window != nullptr);
#endif
#if EXPLICITLY_LOAD_ENGINE_GL_DLL
			// Load the dll and import GetEngineFactoryOpenGL() function
			auto GetEngineFactoryOpenGL = LoadGraphicsEngineOpenGL();
#endif
			// auto* pFactoryOpenGL = GetEngineFactoryOpenGL();
			// m_pEngineFactory = pFactoryOpenGL;
			// EngineGLCreateInfo CreationAttribs;
			// CreationAttribs.Window = *window;
			// if (CreationAttribs.NumDeferredContexts != 0)
			// {
			// 	CreationAttribs.NumDeferredContexts = 0;
			// }
			// ppContexts.resize(1 + CreationAttribs.NumDeferredContexts);
			// pFactoryOpenGL->CreateDeviceAndSwapChainGL(CreationAttribs,
			//                                            &m_pDevice,
			//                                            ppContexts.data(),
			//                                            SCDesc,
			//                                            &m_pSwapChain);
			auto* pFactoryOpenGL = GetEngineFactoryOpenGL();
			m_pEngineFactory = pFactoryOpenGL;

			EngineGLCreateInfo EngineCI;
			EngineCI.Window = *window;

			if (m_ValidationLevel >= 0)
				EngineCI.SetValidationLevel(
				    static_cast<VALIDATION_LEVEL>(m_ValidationLevel));

			EngineCI.Features = DeviceFeatures{DEVICE_FEATURE_STATE_OPTIONAL};

			// if (m_bForceNonSeprblProgs)
			// 	EngineCI.Features.SeparablePrograms =
			// 	    DEVICE_FEATURE_STATE_DISABLED;
			if (EngineCI.NumDeferredContexts != 0)
			{
				LOG_ERROR_MESSAGE(
				    "Deferred contexts are not supported in OpenGL mode");
				EngineCI.NumDeferredContexts = 0;
			}

			NumImmediateContexts = 1;
			ppContexts.resize(NumImmediateContexts +
			                  EngineCI.NumDeferredContexts);
			pFactoryOpenGL->CreateDeviceAndSwapChainGL(
			    EngineCI, &m_pDevice, ppContexts.data(), SCDesc, &m_pSwapChain);
			if (!m_pDevice)
			{
				LOG_ERROR_AND_THROW(
				    "Unable to initialize Diligent Engine in OpenGL mode. The "
				    "API may not be available, "
				    "or required features may not be supported by this "
				    "GPU/driver/OS version.");
			}
		}
		break;
#endif

#if VULKAN_SUPPORTED
		case RENDER_DEVICE_TYPE_VULKAN:
		{
#if EXPLICITLY_LOAD_ENGINE_VK_DLL
			// Load the dll and import GetEngineFactoryVk() function
			auto GetEngineFactoryVk = LoadGraphicsEngineVk();
#endif
			EngineVkCreateInfo EngVkAttribs;

			if (m_ValidationLevel >= 1)
			{
				EngVkAttribs.EnableValidation = true;
			}
			else if (m_ValidationLevel == 0)
			{
				EngVkAttribs.EnableValidation = false;
			}

			ppContexts.resize(1 + EngVkAttribs.NumDeferredContexts);
			auto* pFactoryVk = GetEngineFactoryVk();
			m_pEngineFactory = pFactoryVk;
			pFactoryVk->CreateDeviceAndContextsVk(
			    EngVkAttribs, &m_pDevice, ppContexts.data());
			if (!m_pDevice)
			{
			}
			if (!m_pSwapChain && window != nullptr)
				pFactoryVk->CreateSwapChainVk(
				    m_pDevice, ppContexts[0], SCDesc, *window, &m_pSwapChain);
		}
		break;
#endif

#if METAL_SUPPORTED
		case RENDER_DEVICE_TYPE_METAL:
		{
			EngineMtlCreateInfo MtlAttribs;

			ppContexts.resize(1 + MtlAttribs.NumDeferredContexts);
			auto* pFactoryMtl = GetEngineFactoryMtl();
			pFactoryMtl->CreateDeviceAndContextsMtl(
			    MtlAttribs, &m_pDevice, ppContexts.data());

			if (!m_pSwapChain && window != nullptr)
				pFactoryMtl->CreateSwapChainMtl(
				    m_pDevice, ppContexts[0], SCDesc, *window, &m_pSwapChain);
		}
		break;
#endif

		default:
			break;
	}

	m_pImmediateContext.Attach(ppContexts[0]);
	auto NumDeferredCtx = ppContexts.size() - 1;
	m_pDeferredContexts.resize(NumDeferredCtx);
	for (Uint32 ctx = 0; ctx < NumDeferredCtx; ++ctx)
	{
		m_pDeferredContexts[ctx].Attach(ppContexts[1 + ctx]);
	}
	// printf("WI %i %i\n", SCDesc.Width, SCDesc.Height);

	// auto SCWidth = m_pSwapChain->GetDesc().Width;
	// auto SCHeight = m_pSwapChain->GetDesc().Height;
	// m_TheSample->WindowResize(SCWidth, SCHeight);
}

void BaseDiligentRenderer::resize(int width, int height)
{
	m_pSwapChain->Resize(width, height);
}