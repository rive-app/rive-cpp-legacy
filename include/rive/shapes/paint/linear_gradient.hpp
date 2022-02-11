#ifndef _RIVE_LINEAR_GRADIENT_HPP_
#define _RIVE_LINEAR_GRADIENT_HPP_
#include "rive/generated/shapes/paint/linear_gradient_base.hpp"
#include "rive/math/vec2d.hpp"
#include "rive/shapes/paint/shape_paint_mutator.hpp"
#include <vector>
namespace rive
{
    class Node;
    class GradientStop;

    class LinearGradient : public LinearGradientBase, public ShapePaintMutator
    {
    private:
        std::vector<GradientStop*> m_Stops;
        Node* m_ShapePaintContainer = nullptr;

    public:
        StatusCode onAddedDirty(CoreContext* context) override;
        void addStop(GradientStop* stop);
        void update(ComponentDirt value) override;
        void markGradientDirty();
        void markStopsDirty();

    protected:
        void buildDependencies() override;
        void startXChanged() override;
        void startYChanged() override;
        void endXChanged() override;
        void endYChanged() override;
        void opacityChanged() override;
        void renderOpacityChanged() override;
        virtual void makeGradient(const Vec2D& start, const Vec2D& end);
        bool internalIsTranslucent() const override;
    };
} // namespace rive

#endif
