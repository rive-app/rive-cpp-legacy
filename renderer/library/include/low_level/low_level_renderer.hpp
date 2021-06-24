#ifndef _RIVE_LOW_LEVEL_RENDERER_HPP_
#define _RIVE_LOW_LEVEL_RENDERER_HPP_

#include "renderer.hpp"
#include "graphics_api.hpp"
#include "math/mat2d.hpp"
#include <stdint.h>
#include <list>

namespace rive
{
	struct SubPath
	{
		RenderPath* path;
		Mat2D transform;
	};

	struct RenderState
	{
		Mat2D transform;
		std::list<SubPath> clipPaths;
	};

	///
	/// Low level implementation of a generalized rive::Renderer. It's
	/// specifically tailored for use with low level graphics apis like Metal,
	/// OpenGL, Vulkan, D3D, etc.
	class LowLevelRenderer : public Renderer
	{
	private:
		float m_ModelViewProjection[16] = {0.0f};
		std::list<RenderState> m_Stack;

	public:
		LowLevelRenderer();
		virtual GraphicsApi::Type type() const = 0;

		virtual void startFrame() = 0;
		virtual void endFrame() = 0;

		virtual RenderPaint* makeRenderPaint() = 0;
		virtual RenderPath* makeRenderPath() = 0;
		virtual bool initialize(void* data) = 0;
		bool initialize() { return initialize(nullptr); }

		void modelViewProjection(float value[16]);

		virtual void orthographicProjection(float dst[16],
		                                    float left,
		                                    float right,
		                                    float bottom,
		                                    float top,
		                                    float near,
		                                    float far);

		void save() override;
		void restore() override;
		void transform(const Mat2D& transform) override;
		const Mat2D& transform();
	};

} // namespace rive
#endif