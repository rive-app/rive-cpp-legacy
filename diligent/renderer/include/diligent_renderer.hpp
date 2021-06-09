#ifndef _RIVE_DILIGENT_RENDERER_HPP_
#define _RIVE_DILIGENT_RENDERER_HPP_

#include "base_diligent_renderer.hpp"

namespace rive
{
	using namespace Diligent;

	class DiligentRenderer : public BaseDiligentRenderer
	{
	public:
		~DiligentRenderer();
		void save() override;
		void restore() override;
		void transform(const Mat2D& transform) override;
		void drawPath(RenderPath* path, RenderPaint* paint) override;
		void clipPath(RenderPath* path) override;
		void initializePipeline() override;
	};

} // namespace rive
#endif