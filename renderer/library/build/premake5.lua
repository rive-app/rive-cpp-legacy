workspace "rive"
configurations {"debug", "release"}

project "rive_renderer"
kind "StaticLib"
language "C++"
cppdialect "C++17"
targetdir "bin/%{cfg.buildcfg}"
objdir "obj/%{cfg.buildcfg}"
includedirs {"../include", "../../../include"}

if os.host() == "macosx" then
    links {"Cocoa.framework", "rive"}
    defines {"PLATFORM_MACOS", "RIVE_HAS_METAL", "RIVE_HAS_OPENGL"}
    defines {"GL_SILENCE_DEPRECATION"}
    includedirs {"../../dependencies/DiligentEngine_build/build/include"}
    files {"../src/**.mm"}
else
    links {"rive"}
end

libdirs {"../../../build/bin/%{cfg.buildcfg}", "../../dependencies/skia/out/Static"}

files {"../src/**.cpp"}

buildoptions {"-Wall", "-fno-rtti"}

filter "configurations:debug"
defines {"DEBUG"}
symbols "On"

filter "configurations:release"
defines {"RELEASE"}
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
