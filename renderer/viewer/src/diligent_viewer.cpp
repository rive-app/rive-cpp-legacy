#include "diligent_viewer.hpp"

using namespace rive;
using namespace Diligent;

namespace Diligent
{
	GLFWDemo* CreateGLFWApp() { return new DiligentViewer{}; }
} // namespace Diligent

bool DiligentViewer::Initialize()
{
	try
	{
		// GetEngineFactory()->CreateDefaultShaderSourceStreamFactory(
		//     nullptr, &m_pShaderSourceFactory);
		// CHECK_THROW(m_pShaderSourceFactory);

		// GenerateMap();
		// CreateSDFMap();
		// CreatePipelineState();
		// InitPlayer();
		// BindResources();

		return true;
	}
	catch (...)
	{
		return false;
	}
}

void DiligentViewer::Update(float dt) {}

void DiligentViewer::Draw()
{
	auto* pContext = GetContext();
	auto* pSwapchain = GetSwapChain();

	ITextureView* pRTV = pSwapchain->GetCurrentBackBufferRTV();
	pContext->SetRenderTargets(
	    1, &pRTV, nullptr, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

	const float ClearColor[] = {0.350f, 0.350f, 0.350f, 1.0f};
	pContext->ClearRenderTarget(
	    pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_VERIFY);

	// pContext->Flush();
	// pSwapchain->Present();
}

void DiligentViewer::KeyEvent(Key key, KeyState state) {}

void DiligentViewer::MouseEvent(float2 pos) {}
