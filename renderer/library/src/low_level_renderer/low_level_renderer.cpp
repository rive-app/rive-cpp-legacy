#include "low_level/low_level_renderer.hpp"
#include <cstring>

using namespace rive;

LowLevelRenderer::LowLevelRenderer() { m_Stack.emplace_back(RenderState()); }

void LowLevelRenderer::modelViewProjection(float value[16])
{
	std::memcpy(m_ModelViewProjection, value, sizeof(m_ModelViewProjection));
}

void LowLevelRenderer::orthographicProjection(float dst[16],
                                              float left,
                                              float right,
                                              float bottom,
                                              float top,
                                              float near,
                                              float far)
{
	dst[0] = 2.0f / (right - left);
	dst[1] = 0.0f;
	dst[2] = 0.0f;
	dst[3] = 0.0f;

	dst[4] = 0.0f;
	dst[5] = 2.0f / (top - bottom);
	dst[6] = 0.0f;
	dst[7] = 0.0f;

	dst[8] = 0.0f;
	dst[9] = 0.0f;
	dst[10] = 2.0f / (near - far);
	dst[11] = 0.0f;

	dst[12] = (right + left) / (left - right);
	dst[13] = (top + bottom) / (bottom - top);
	dst[14] = (far + near) / (near - far);
	dst[15] = 1.0f;
}

void LowLevelRenderer::save() { m_Stack.push_back(m_Stack.back()); }

void LowLevelRenderer::restore()
{
	assert(m_Stack.size() > 1);
	m_Stack.pop_back();
}

void LowLevelRenderer::transform(const Mat2D& transform)
{
	Mat2D::copy(m_Stack.back().transform, transform);
}
const Mat2D& LowLevelRenderer::transform() { return m_Stack.back().transform; }