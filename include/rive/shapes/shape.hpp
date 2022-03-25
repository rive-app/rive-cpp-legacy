#ifndef _RIVE_SHAPE_HPP_
#define _RIVE_SHAPE_HPP_

#include "rive/hit_info.hpp"
#include "rive/generated/shapes/shape_base.hpp"
#include "rive/shapes/path_composer.hpp"
#include "rive/shapes/shape_paint_container.hpp"
#include <vector>

namespace rive {
    class Path;
    class PathComposer;
    class Shape : public ShapeBase, public ShapePaintContainer {
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
        Core* hitTest(HitInfo*, const Mat2D&) override;

        PathComposer* pathComposer() const { return (PathComposer*)&m_PathComposer; }

        void pathChanged();
        void addDefaultPathSpace(PathSpace space);
        StatusCode onAddedDirty(CoreContext* context) override;
    };
} // namespace rive

#endif
