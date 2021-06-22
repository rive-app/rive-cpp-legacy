workspace "rive"
configurations {"debug", "release"}

BASE_DIR = path.getabsolute("../../../build")
location("./")
dofile(path.join(BASE_DIR, "premake5.lua"))

BASE_DIR = path.getabsolute("../../library/build")
location("./")
dofile(path.join(BASE_DIR, "premake5.lua"))

DEPENDENCIES_DIR = "../../dependencies/local/";

project "rive_low_level_viewer"
kind "ConsoleApp"
language "C++"
cppdialect "C++17"
targetdir "bin/%{cfg.buildcfg}"
objdir "obj/%{cfg.buildcfg}"
includedirs {"../include", "../../../include", "../../library/include", "%{DEPENDENCIES_DIR}/include"}

if os.host() == "macosx" then
    links {"Cocoa.framework", "IOKit.framework", "CoreVideo.framework", "Metal.framework", "QuartzCore.framework",
           "OpenGL.framework", "glfw3"}
    defines {"RIVE_HAS_OPENGL", "RIVE_HAS_METAL"}
    defines {"GL_SILENCE_DEPRECATION"}
    includedirs {"%{DEPENDENCIES_DIR}/include/gl3w"}
    files {"../src/**.mm"}
end

links {"rive", "rive_renderer"}
libdirs {"../../../build/bin/%{cfg.buildcfg}", "../../library/build/bin/%{cfg.buildcfg}", "%{DEPENDENCIES_DIR}/lib"}

files {"../src/**.cpp"}

buildoptions {"-Wall", "-fno-rtti"}

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
