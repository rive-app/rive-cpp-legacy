workspace "rive"
configurations { "debug", "release" }

project "rive_viewer"
kind "ConsoleApp"
language "C++"
cppdialect "C++17"
toolset "clang"
targetdir "bin/%{cfg.buildcfg}"
objdir "obj/%{cfg.buildcfg}"

skia = "dependencies/skia"
gl3w = "dependencies/gl3w/build"
rive = "../../../../"
imgui = "dependencies/imgui"
glfw_build = "dependencies/glfw_build/src"
glfw = "dependencies/glfw"

includedirs { "../include", skia, skia .. "/include/core", skia .. "/include/effects", skia .. "/include/gpu",
       skia .. "/include/config", gl3w .. "/include", rive .. "/include", rive .. "/skia/renderer/include", imgui, glfw .. "/include", "dependencies" }
libdirs { skia .. "/out/static" }

links { "skia", "glfw3", "opengl32" }
libdirs { "../../../build/bin/%{cfg.buildcfg}", glfw_build .. "/Debug",
       "../../dependencies/skia/out/static", "../../renderer/build/bin/%{cfg.buildcfg}" }

files { "../../src/**.cpp", rive .. "/src/**.cpp", rive .. "/skia/renderer/src/**.cpp", gl3w .. "/src/gl3w.c",
       imgui .. "/backends/imgui_impl_glfw.cpp",
       imgui .. "/backends/imgui_impl_opengl3.cpp", imgui .. "/imgui_widgets.cpp",
       imgui .. "/imgui.cpp", imgui .. "/imgui_tables.cpp",
       imgui .. "/imgui_draw.cpp" }

defines { "_USE_MATH_DEFINES", "NOMINMAX" }
architecture("x86_64")
buildoptions { "/MT", "-Wno-c++98-compat", "-Wno-unused-template", "-Wno-c++98-compat-pedantic",
       "-Wno-reserved-macro-identifier", "-Wno-newline-eof", "-Wno-old-style-cast", "-Wno-unused-parameter",
       "-Wno-float-equal", "-Wno-implicit-float-conversion", "-Wno-shadow", "-Wno-sign-conversion",
       "-Wno-inconsistent-missing-destructor-override", "-Wno-nested-anon-types",
       "-Wno-suggest-destructor-override", "-Wno-non-virtual-dtor", "-Wno-unknown-argument",
       "-Wno-gnu-anonymous-struct", "-Wno-extra-semi", "-Wno-cast-qual", "-Wno-ignored-qualifiers",
       "-Wno-double-promotion", "-Wno-tautological-unsigned-zero-compare", "-Wno-unreachable-code-break",
       "-Wno-global-constructors", "-Wno-switch-enum", "-Wno-shorten-64-to-32", "-Wno-missing-prototypes",
       "-Wno-implicit-int-conversion", "-Wno-unused-macros", "-Wno-deprecated-copy-with-user-provided-dtor",
       "-Wno-missing-variable-declarations", "-Wno-ctad-maybe-unsupported", "-Wno-vla-extension",
       "-Wno-float-conversion", "-Wno-gnu-zero-variadic-macro-arguments", "-Wno-undef", "-Wno-documentation",
       "-Wno-documentation-pedantic", "-Wno-documentation-unknown-command", "-Wno-suggest-override",
       "-Wno-unused-exception-parameter", "-Wno-cast-align", "-Wno-deprecated-declarations", "-Wno-shadow-field",
       "-Wno-nonportable-system-include-path", "-Wno-reserved-identifier", "-Wno-thread-safety-negative",
       "-Wno-exit-time-destructors", "-Wno-unreachable-code", "-Wno-zero-as-null-pointer-constant",
       "-Wno-pedantic", "-Wno-sign-compare", "-Wall", "-fno-exceptions", "-fno-rtti", "-g" }
filter "configurations:debug"
defines { "DEBUG" }
symbols "On"

filter "configurations:release"

defines { "RELEASE" }
defines { "NDEBUG" }
optimize "On"
