#ifndef RENDERER_TARGET_HPP
#define RENDERER_TARGET_HPP

#include <memory>
#include <string>

namespace rive {
    class Renderer;
};

// Wraps a factory for rive::Renderer and a singleton target for it to render into (GL window, HTML
// canvas, software buffer, etc.):
//
//   RendererTarget::Init(type);
//   renderer = RendererTarget::Get()->reset(width, height);
//   ...
//
class RendererTarget {
public:
    enum class Type {
        skia_cpu,
        skia_gl
    };

    static void Init(Type);
    static RendererTarget* Get();

    virtual std::unique_ptr<rive::Renderer> reset(int width, int height) = 0;
    virtual void flush() const = 0;
    virtual void dumpToPNG(const std::string& filepath) const = 0;
    virtual ~RendererTarget() {}
};

#endif
