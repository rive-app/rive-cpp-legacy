#ifndef _RIVE_NOOP_FACTORY_HPP_
#define _RIVE_NOOP_FACTORY_HPP_

#include <rive/renderer.hpp>
#include <rive/factory.hpp>

namespace rive {

    class NoOpFactory : public Factory {
        rcp<RenderBuffer> makeBufferU16(Span<const uint16_t>) override;
        rcp<RenderBuffer> makeBufferU32(Span<const uint32_t>) override;
        rcp<RenderBuffer> makeBufferF32(Span<const float>) override;

        rcp<RenderShader> makeLinearGradient(float sx, float sy,
                                                     float ex, float ey,
                                                     const ColorInt colors[],    // [count]
                                                     const float stops[],        // [count]
                                                     size_t count,
                                                     RenderTileMode,
                                                     const Mat2D* localMatrix = nullptr) override;

        rcp<RenderShader> makeRadialGradient(float cx, float cy, float radius,
                                                     const ColorInt colors[],    // [count]
                                                     const float stops[],        // [count]
                                                     size_t count,
                                                     RenderTileMode,
                                                     const Mat2D* localMatrix = nullptr) override;

        std::unique_ptr<RenderPath> makeRenderPath(Span<const Vec2D> points,
                                                   Span<const uint8_t> verbs,
                                                   FillRule) override;

        std::unique_ptr<RenderPath> makeEmptyRenderPath() override;

        std::unique_ptr<RenderPaint> makeRenderPaint() override;

        std::unique_ptr<RenderImage> decodeImage(Span<const uint8_t>) override;
    };

    static NoOpFactory gNoOpFactory;

} // namespace rive
#endif
