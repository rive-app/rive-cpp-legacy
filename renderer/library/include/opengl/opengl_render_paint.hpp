#ifndef _RIVE_OPENGL_RENDER_PAINT_HPP_
#define _RIVE_OPENGL_RENDER_PAINT_HPP_

#include "renderer.hpp"
#include <vector>

namespace rive
{
	class OpenGLRenderer;
	class OpenGLRenderPaint;
	class OpenGLRenderPath;

	class OpenGLGradient
	{
		friend class OpenGLRenderPaint;

	private:
		float m_Position[4];
		int m_Type = 0;
		std::vector<float> m_Colors;
		std::vector<float> m_Stops;
		bool m_IsVisible = false;

	public:
		OpenGLGradient(int type);
		void position(float sx, float sy, float ex, float ey);
		void addStop(unsigned int color, float stop);
		void bind(OpenGLRenderer* renderer);
	};

	class OpenGLRenderPaint : public RenderPaint
	{
	private:
		RenderPaintStyle m_PaintStyle;
		float m_Color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
		OpenGLGradient* m_Gradient = nullptr;

	public:
		void style(RenderPaintStyle style) override;
		RenderPaintStyle style() const { return m_PaintStyle; }
		void color(unsigned int value) override;
		void thickness(float value) override;
		void join(StrokeJoin value) override;
		void cap(StrokeCap value) override;
		void blendMode(BlendMode value) override;

		void linearGradient(float sx, float sy, float ex, float ey) override;
		void radialGradient(float sx, float sy, float ex, float ey) override;
		void addStop(unsigned int color, float stop) override;
		void completeGradient() override;
		~OpenGLRenderPaint();

		bool doesDraw() const;
		void draw(OpenGLRenderer* renderer,
		          const Mat2D& transform,
		          OpenGLRenderPath* path);
	};
} // namespace rive
#endif