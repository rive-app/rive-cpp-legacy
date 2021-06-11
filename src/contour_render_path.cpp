#ifdef LOW_LEVEL_RENDERING
#include "contour_render_path.hpp"

using namespace rive;

PathCommand::PathCommand(PathCommandType type) : m_Type(type) {}
PathCommand::PathCommand(PathCommandType type, float x, float y) :
    m_Type(type), m_Point(x, y)
{
}
PathCommand::PathCommand(PathCommandType type,
                         float outX,
                         float outY,
                         float inX,
                         float inY,
                         float x,
                         float y) :
    m_Type(type), m_OutPoint(outX, outY), m_InPoint(inX, inY), m_Point(x, y)
{
}

ContourSubPath::ContourSubPath(RenderPath* path, const Mat2D& transform) :
    m_Path(path), m_Transform(transform)
{
}

RenderPath* ContourSubPath::path() const { return m_Path; }
const Mat2D& ContourSubPath::transform() const { return m_Transform; }

void ContourRenderPath::addRenderPath(RenderPath* path, const Mat2D& transform)
{
	m_SubPaths.emplace_back(ContourSubPath(path, transform));
}

void ContourRenderPath::reset()
{
	m_SubPaths.clear();
	m_ContourVertices.clear();
	m_Commands.clear();
	m_IsDirty = true;
}

void ContourRenderPath::moveTo(float x, float y)
{
	m_Commands.emplace_back(PathCommand(PathCommandType::move, x, y));
}

void ContourRenderPath::lineTo(float x, float y)
{
	m_Commands.emplace_back(PathCommand(PathCommandType::line, x, y));
}

void ContourRenderPath::cubicTo(
    float ox, float oy, float ix, float iy, float x, float y)
{
	m_Commands.emplace_back(
	    PathCommand(PathCommandType::cubic, ox, oy, ix, iy, x, y));
}
void ContourRenderPath::close()
{
	m_Commands.emplace_back(PathCommand(PathCommandType::close));
}

bool ContourRenderPath::isContainer() const { return !m_SubPaths.empty(); }

#endif