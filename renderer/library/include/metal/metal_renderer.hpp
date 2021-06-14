#ifndef _RIVE_METAL_RENDERER_HPP_
#define _RIVE_METAL_RENDERER_HPP_

#include "renderer.hpp"

namespace rive
{
	class MetalRenderer : public Renderer
	{
	public:
		~MetalRenderer();
		void save() override;
		void restore() override;
		void transform(const Mat2D& transform) override;
		void drawPath(RenderPath* path, RenderPaint* paint) override;
		void clipPath(RenderPath* path) override;
	};

} // namespace rive
#endif