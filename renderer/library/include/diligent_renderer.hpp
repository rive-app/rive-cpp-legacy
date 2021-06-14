#ifndef _RIVE_DILIGENT_RENDERER_HPP_
#define _RIVE_DILIGENT_RENDERER_HPP_

#include "base_diligent_renderer.hpp"

namespace rive
{
	using namespace Diligent;

	class DiligentRenderer : public BaseDiligentRenderer
	{
	private:
		/// The pipeline state for filling the stencil with contour data.
		RefCntAutoPtr<IPipelineState> m_StencilPSO;

		/// The pipeline state for covering the prepped stencil.
		RefCntAutoPtr<IPipelineState> m_CoverPSO;

		/// Constants (uniforms) for the stencil state.
		RefCntAutoPtr<IBuffer> m_StencilConstants;

		/// Shader resource binding for the shader pipeline state.
		RefCntAutoPtr<IShaderResourceBinding> m_StencilResourceBinding;

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