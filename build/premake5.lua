workspace "rive"
configurations {"debug", "release"}

project "rive"
kind "StaticLib"
language "C++"
cppdialect "C++17"
targetdir "bin/%{cfg.buildcfg}"
objdir "obj/%{cfg.buildcfg}"
includedirs {"../include"}

files {"../src/**.cpp"}

buildoptions {"-Wall", "-fno-exceptions", "-fno-rtti"}

newoption {
    trigger = "with-low-level-rendering",
    description = "Builds in utility classes and methods used for low level renderering implementations."
}

filter "system:windows"
defines {"_USE_MATH_DEFINES"}

filter "configurations:debug"
defines {"DEBUG"}
symbols "On"

filter "configurations:release"
defines {"RELEASE"}
defines {"NDEBUG"}
optimize "On"

configuration "with-low-level-rendering"
defines("LOW_LEVEL_RENDERING")

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
