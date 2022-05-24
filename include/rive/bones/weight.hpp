#ifndef _RIVE_WEIGHT_HPP_
#define _RIVE_WEIGHT_HPP_
#include "rive/generated/bones/weight_base.hpp"
#include "rive/math/vec2d.hpp"
#include <stdio.h>

namespace rive {
    class Weight : public WeightBase {
    private:
        Vec2D m_Translation;

    public:
        Vec2D& translation() { return m_Translation; }

        StatusCode onAddedDirty(CoreContext* context) override;

        static Mat2D deform(unsigned int indices,
                            unsigned int weights,
                            const float* boneTransforms);
    };
} // namespace rive

#endif