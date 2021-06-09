workspace "rive"
configurations {"debug", "release"}

BASE_DIR = path.getabsolute("../../../build")
location("./")
dofile(path.join(BASE_DIR, "premake5.lua"))

BASE_DIR = path.getabsolute("../../renderer/build")
location("./")
dofile(path.join(BASE_DIR, "premake5.lua"))

project "rive_diligent_viewer"
kind "ConsoleApp"
language "C++"
cppdialect "C++17"
targetdir "bin/%{cfg.buildcfg}"
objdir "obj/%{cfg.buildcfg}"
includedirs {"../include", "../../../include", "../../renderer/include", "../../dependencies/glfw/include",
             "../../dependencies/glfw/include"}

if os.host() == "macosx" then
    links {"Cocoa.framework", "IOKit.framework", "CoreVideo.framework", "OpenGL.framework"}
    libdirs {"../../dependencies/DiligentEngine_build/build/lib/DiligentCore/Debug"}
    links {"DiligentCore", "GenericCodeGen", "glew-static", "glslang", "HLSL", "MachineIndependent", "OGLCompiler",
           "OSDependent", "spirv-cross-core", "SPIRV-Tools-opt", "SPIRV-Tools", "SPIRV", "GraphicsEngineOpenGL"}
    defines {"PLATFORM_MACOS", "GL_SUPPORTED"}
    includedirs {"../../dependencies/DiligentEngine_build/build/include"}
end

links {"rive", "rive_diligent_renderer", "glfw3"}
libdirs {"../../../build/bin/%{cfg.buildcfg}", "../../dependencies/glfw_build/src",
         "../../renderer/build/bin/%{cfg.buildcfg}"}

files {"../src/**.cpp"}

buildoptions {"-Wall"}

filter "configurations:debug"
defines {"DEBUG"}
symbols "On"

filter "configurations:release"
defines {"RELEASE"}
defines {"NDEBUG"}
optimize "On"

-- Clean Function --
newaction {
    trigger = "clean",
    description = "clean the build",
    execute = function()
        print("clean the build...")
        os.rmdir("./bin")
        os.rmdir("./obj")
        os.remove("Makefile")
        -- no wildcards in os.remove, so use shell
        os.execute("rm *.make")
        print("build cleaned")
    end
}
