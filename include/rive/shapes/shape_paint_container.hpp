#ifndef _RIVE_SHAPE_PAINT_CONTAINER_HPP_
#define _RIVE_SHAPE_PAINT_CONTAINER_HPP_
#include "rive/shapes/paint/blend_mode.hpp"
#include "rive/shapes/path_space.hpp"
#include <vector>

namespace rive {
    class Artboard;
    class ShapePaint;
    class Component;

    class CommandPath;

    class ShapePaintContainer {
        friend class ShapePaint;

    protected:
        // Need this to access our artboard. We are treated as a mixin, either
        // as a Shape or Artboard, so both of those will override this.
        virtual Artboard* getArtboard() = 0;
    
        PathSpace m_DefaultPathSpace = PathSpace::Neither;
        std::vector<ShapePaint*> m_ShapePaints;
        void addPaint(ShapePaint* paint);

        // TODO: void draw(Renderer* renderer, PathComposer& composer);
    public:
        static ShapePaintContainer* from(Component* component);

        PathSpace pathSpace() const;

        void invalidateStrokeEffects();

        std::unique_ptr<CommandPath> makeCommandPath(PathSpace space);
    };
} // namespace rive

#endif
