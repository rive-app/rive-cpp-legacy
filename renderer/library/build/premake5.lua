workspace "rive"
configurations {"debug", "release"}

project "rive_renderer"
kind "StaticLib"
language "C++"
cppdialect "C++17"
targetdir "bin/%{cfg.buildcfg}"
objdir "obj/%{cfg.buildcfg}"
includedirs {"../include", "../../../include"}

newoption {
    trigger = "for-android",
    description = "Builds in utility classes and methods used for low level renderering implementations that work with Android systems"
}

files {"../src/**.cpp"}

buildoptions {"-Wall", "-fno-rtti"}

filter "configurations:debug"
defines {"DEBUG"}
symbols "On"

filter "configurations:release"
defines {"RELEASE"}
optimize "On"

configuration "for-android"
    links {"rive"}
    defines {"RIVE_HAS_OPENGL", "ANDROID", "GL_SILENCE_DEPRECATION"}
    libdirs {"../../../build/bin/%{cfg.buildcfg}"}

configuration "for-macos"
    links {"Cocoa.framework", "rive"}
    defines {"RIVE_HAS_METAL", "RIVE_HAS_OPENGL"}
    defines {"GL_SILENCE_DEPRECATION"}
    includedirs {"../../dependencies/DiligentEngine_build/build/include"}
    files {"../src/**.mm"}
    libdirs {"../../../build/bin/%{cfg.buildcfg}", "../../dependencies/skia/out/Static"}

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
