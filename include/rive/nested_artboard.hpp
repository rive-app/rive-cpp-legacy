#ifndef _RIVE_NESTED_ARTBOARD_HPP_
#define _RIVE_NESTED_ARTBOARD_HPP_

#include "rive/generated/nested_artboard_base.hpp"
#include "rive/hit_info.hpp"
#include <stdio.h>

namespace rive {
    class ArtboardInstance;
    class NestedAnimation;
    class NestedArtboard : public NestedArtboardBase {

    private:
        Artboard* m_Artboard = nullptr; // might point to m_Instance, and might not
        std::unique_ptr<ArtboardInstance> m_Instance;   // may be null
        std::vector<NestedAnimation*> m_NestedAnimations;

    public:
        NestedArtboard();
        ~NestedArtboard();
        StatusCode onAddedClean(CoreContext* context) override;
        void draw(Renderer* renderer) override;
        Core* hitTest(HitInfo*, const Mat2D&) override;
        void addNestedAnimation(NestedAnimation* nestedAnimation);

        void nest(Artboard* artboard);

        StatusCode import(ImportStack& importStack) override;
        Core* clone() const override;
        bool advance(float elapsedSeconds);
        void update(ComponentDirt value) override;
    };
} // namespace rive

#endif
