#ifndef _RIVE_DILIGENT_RENDER_PATH_HPP_
#define _RIVE_DILIGENT_RENDER_PATH_HPP_

#include "contour_render_path.hpp"

namespace rive
{
	class DiligentRenderPath : public ContourRenderPath
	{
	private:
		FillRule m_FillRule;

	public:
		void fillRule(FillRule value) override;
		FillRule fillRule() const { return m_FillRule; }
	};
} // namespace rive
#endif