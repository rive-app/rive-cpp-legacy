#ifndef _RIVE_BASE_DILIGENT_RENDERER_HPP_
#define _RIVE_BASE_DILIGENT_RENDERER_HPP_

#include "renderer.hpp"

#include <vector>
#include <string>
#include <memory>

#include "DiligentCore/Platforms/interface/PlatformDefinitions.h"
#include "DiligentCore/Graphics/GraphicsEngine/interface/GraphicsTypes.h"
#include "DiligentCore/Graphics/GraphicsEngine/interface/ShaderResourceBinding.h"
#include "DiligentCore/Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "DiligentCore/Graphics/GraphicsEngine/interface/EngineFactory.h"
#include "DiligentCore/Graphics/GraphicsEngine/interface/SwapChain.h"
#include "DiligentCore/Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "DiligentCore/Common/interface/RefCntAutoPtr.hpp"

namespace rive
{
	using namespace Diligent;

	class BaseDiligentRenderer : public Renderer
	{
	protected:
		RENDER_DEVICE_TYPE m_DeviceType;
		RefCntAutoPtr<IPipelineState> m_pPSO;
		RefCntAutoPtr<IEngineFactory> m_pEngineFactory;
		RefCntAutoPtr<IRenderDevice> m_pDevice;
		RefCntAutoPtr<IDeviceContext> m_pImmediateContext;
		std::vector<RefCntAutoPtr<IDeviceContext>> m_pDeferredContexts;
		RefCntAutoPtr<ISwapChain> m_pSwapChain;
		GraphicsAdapterInfo m_AdapterAttribs;
		std::vector<DisplayModeAttribs> m_DisplayModes;

		int m_ValidationLevel = -1;
		Uint32 m_AdapterId = 0;
		ADAPTER_TYPE m_AdapterType = ADAPTER_TYPE_UNKNOWN;

	public:
		~BaseDiligentRenderer();

		RENDER_DEVICE_TYPE deviceType() const;
		void deviceType(RENDER_DEVICE_TYPE value);
		void initialize(const NativeWindow* window);
		virtual void initializePipeline() = 0;
		virtual void resize(int width, int height);
	};

} // namespace rive
#endif