#ifndef _RIVE_SHAPE_HPP_
#define _RIVE_SHAPE_HPP_
#include "generated/shapes/shape_base.hpp"
#include "shapes/path_composer.hpp"
#include "shapes/shape_paint_container.hpp"
#include <vector>

namespace rive
{
	class Path;
	class PathComposer;
	class Shape : public ShapeBase, public ShapePaintContainer
	{
	private:
		PathComposer m_PathComposer;
		std::vector<Path*> m_Paths;

		bool m_WantDifferencePath = false;

	public:
		Shape();
		void buildDependencies() override;
		void addPath(Path* path);
		std::vector<Path*>& paths() { return m_Paths; }

		bool wantDifferencePath() const { return m_WantDifferencePath; }

		void update(ComponentDirt value) override;
		void draw(Renderer* renderer) override;

		PathComposer* pathComposer() const
		{
			return (PathComposer*)&m_PathComposer;
		}

		void pathChanged();
		void addDefaultPathSpace(PathSpace space);
		StatusCode onAddedDirty(CoreContext* context) override;
	};
} // namespace rive

#endif