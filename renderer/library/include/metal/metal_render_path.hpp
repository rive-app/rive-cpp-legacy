#ifndef _RIVE_METAL_RENDER_PATH_HPP_
#define _RIVE_METAL_RENDER_PATH_HPP_

#include "rive/contour_render_path.hpp"

namespace rive
{
	class MetalRenderPath : public ContourRenderPath
	{
	private:
		FillRule m_FillRule;

	public:
		void fillRule(FillRule value) override;
		FillRule fillRule() const { return m_FillRule; }
	};
} // namespace rive
#endif