#include "low_level/low_level_renderer.hpp"

using namespace rive;

ViewportSize::ViewportSize() : ViewportSize(0, 0) {}
ViewportSize::ViewportSize(uint16_t width, uint16_t height) :
    m_Width(width), m_Height(height)
{
}

void LowLevelRenderer::viewportSize(const ViewportSize& size)
{
	if (m_ViewportSize != size)
	{
		onViewportSizeChanged(m_ViewportSize, size);
		m_ViewportSize = size;
	}
}
