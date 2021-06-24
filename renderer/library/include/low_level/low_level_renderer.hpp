#ifndef _RIVE_LOW_LEVEL_RENDERER_HPP_
#define _RIVE_LOW_LEVEL_RENDERER_HPP_

#include "renderer.hpp"
#include "graphics_api.hpp"
#include <stdint.h>

namespace rive
{
	class ViewportSize
	{
	private:
		uint16_t m_Width = 0;
		uint16_t m_Height = 0;

	public:
		ViewportSize();
		ViewportSize(uint16_t width, uint16_t height);

		uint16_t width() const { return m_Width; }
		uint16_t height() const { return m_Height; }
	};

	inline bool operator==(const ViewportSize& lhs, const ViewportSize& rhs)
	{
		return lhs.width() == rhs.width() && lhs.height() == lhs.height();
	}

	inline bool operator!=(const ViewportSize& lhs, const ViewportSize& rhs)
	{
		return lhs.width() != rhs.width() || lhs.height() != lhs.height();
	}

	///
	/// Low level implementation of a generalized rive::Renderer. It's
	/// specifically tailored for use with low level graphics apis like Metal,
	/// OpenGL, Vulkan, D3D, etc.
	class LowLevelRenderer : public Renderer
	{
	private:
		ViewportSize m_ViewportSize;

	public:
		virtual GraphicsApi::Type type() const = 0;
		void viewportSize(const ViewportSize& size);
		const ViewportSize& viewportSize() { return m_ViewportSize; }

		virtual void onViewportSizeChanged(ViewportSize from,
		                                   ViewportSize to) = 0;

		virtual void startFrame() = 0;
		virtual void endFrame() = 0;

		virtual RenderPaint* makeRenderPaint() = 0;
		virtual RenderPath* makeRenderPath() = 0;
		virtual bool initialize() = 0;
	};

} // namespace rive
#endif