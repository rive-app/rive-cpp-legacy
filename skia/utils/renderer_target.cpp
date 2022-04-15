/*
 * Copyright 2022 Rive
 */

#ifdef TESTING

// Don't compile this file as part of the "tests" project.

#else

#include "renderer_target.hpp"

#include "GLFW/glfw3.h"
#include "skia_renderer.hpp"
#include "skia/include/core/SkImage.h"
#include "skia/include/core/SkPixmap.h"
#include "skia/include/core/SkStream.h"
#include "skia/include/core/SkSurface.h"
#include "skia/include/gpu/GrDirectContext.h"
#include "skia/include/gpu/gl/GrGLInterface.h"
#include "skia/include/gpu/gl/GrGLAssembleInterface.h"
#include "write_png_file.hpp"
#include <vector>

class SkiaRasterTarget : public RendererTarget {
public:
    std::unique_ptr<rive::Renderer> reset(int width, int height) override {
        auto info = SkImageInfo::MakeN32Premul(width, height);
        m_Surface = SkSurface::MakeRaster(info);
        return std::make_unique<rive::SkiaRenderer>(m_Surface->getCanvas());
    }

    void flush() const override { m_Surface->getCanvas()->flush(); }

    void dumpToPNG(const std::string& filepath) const override {
        assert(m_Surface);
        flush();
        auto img = m_Surface->makeImageSnapshot();
        std::vector<uint8_t> pixels(img->height() * img->width() * 4);
        SkColorInfo colorInfo(kRGBA_8888_SkColorType, kPremul_SkAlphaType, nullptr);
        img->readPixels(nullptr,
                        SkPixmap(SkImageInfo::Make({img->width(), img->height()}, colorInfo),
                                 pixels.data(),
                                 img->width() * 4),
                        0,
                        0);
        WritePNGFile(pixels.data(), img->width(), img->height(), false, filepath.c_str());
    }

private:
    sk_sp<SkSurface> m_Surface;
};

class SkiaGLTarget : public RendererTarget {
public:
    static GrGLFuncPtr GetGLProcAddress(void* ctx, const char name[]) {
        return glfwGetProcAddress(name);
    }

    SkiaGLTarget() {
        if (!glfwInit()) {
            fprintf(stderr, "Failed to initialize GLFW.\n");
            exit(-1);
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        m_Window = glfwCreateWindow(256, 256, "Rive", nullptr, nullptr);
        if (!m_Window) {
            glfwTerminate();
            fprintf(stderr, "Failed to create GLFW m_Window.\n");
            exit(-1);
        }

        glfwMakeContextCurrent(m_Window);

        m_GrContext =
            GrDirectContext::MakeGL(GrGLMakeAssembledInterface(nullptr, GetGLProcAddress));
    }

    ~SkiaGLTarget() override {
#if 0
        // FIXME: This hangs on my Windows system ¯\_(ツ)_/¯
        glfwPollEvents();
        glfwDestroyWindow(m_Window);
        glfwTerminate();
#endif
    }

    std::unique_ptr<rive::Renderer> reset(int width, int height) override {
        if (width != m_Width || height != m_Height) {
            glfwSetWindowSize(m_Window, width, height);
        }

        glViewport(0, 0, width, height);

        GrBackendRenderTarget backendRT(
            width, height, 1 /*samples*/, 0 /*stencilBits*/, {0 /*fbo 0*/, GL_RGBA8});

        SkSurfaceProps surfProps(0, kUnknown_SkPixelGeometry);

        m_Surface = SkSurface::MakeFromBackendRenderTarget(m_GrContext.get(),
                                                           backendRT,
                                                           kBottomLeft_GrSurfaceOrigin,
                                                           kRGBA_8888_SkColorType,
                                                           nullptr,
                                                           &surfProps);
        m_Width = width;
        m_Height = height;

        return std::make_unique<rive::SkiaRenderer>(m_Surface->getCanvas());
    }

    void flush() const override { m_Surface->getCanvas()->flush(); }

    void dumpToPNG(const std::string& filepath) const override {
        flush();
        std::vector<uint8_t> pixels(m_Height * m_Width * 4);
        glReadPixels(0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        glfwSwapBuffers(m_Window);
        WritePNGFile(pixels.data(), m_Width, m_Height, true, filepath.c_str());
    }

private:
    GLFWwindow* m_Window = nullptr;
    sk_sp<GrDirectContext> m_GrContext;
    sk_sp<SkSurface> m_Surface;
    int m_Width = 0;
    int m_Height = 0;
};

static std::unique_ptr<RendererTarget> s_RendererTarget;

void RendererTarget::Init(Type type) {
    assert(!s_RendererTarget);
    switch (type) {
        case Type::skia_cpu:
            s_RendererTarget = std::make_unique<SkiaRasterTarget>();
            break;
        case Type::skia_gl:
            s_RendererTarget = std::make_unique<SkiaGLTarget>();
            break;
    }
}

RendererTarget* RendererTarget::Get() {
    assert(s_RendererTarget); // Call Init() first!
    return s_RendererTarget.get();
}

#endif
