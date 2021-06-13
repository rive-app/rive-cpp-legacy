#ifndef _RIVE_DILIGENT_VIEWER_HPP_
#define _RIVE_DILIGENT_VIEWER_HPP_

#include "third_party/GLFWDemo.hpp"

namespace rive
{
	class DiligentViewer : public Diligent::GLFWDemo
	{
	public:
		virtual bool Initialize() override;
		virtual void Update(float dt) override;
		virtual void Draw() override;
		virtual void KeyEvent(Key key, KeyState state) override;
		virtual void MouseEvent(Diligent::float2 pos) override;
	};

} // namespace rive

#endif