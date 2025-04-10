cmake_minimum_required(VERSION 3.15)
project(rive VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define options (equivalent to newoption in Premake)
option(WITH_RIVE_TOOLS "Enables tools usually not necessary for runtime" OFF)
option(WITH_RIVE_TEXT "Compiles in text features" OFF)
set(WITH_RIVE_AUDIO "disabled" CACHE STRING "The audio mode to use")
set_property(CACHE WITH_RIVE_AUDIO PROPERTY STRINGS "disabled" "system" "external")
option(WITH_RIVE_LAYOUT "Compiles in layout features" OFF)
option(NO_HARFBUZZ_RENAMES "Disable HarfBuzz renames" OFF)
option(NO_YOGA_RENAMES "Disable Yoga renames" OFF)
set(VARIANT "system" CACHE STRING "Choose a particular variant to build")
set_property(CACHE VARIANT PROPERTY STRINGS "system" "emulator" "runtime")

# Include FetchContent for dependencies
include(FetchContent)

# HarfBuzz
FetchContent_Declare(
    harfbuzz
    GIT_REPOSITORY https://github.com/harfbuzz/harfbuzz.git
    GIT_TAG 8.3.0  # Use a stable version
)
FetchContent_MakeAvailable(harfbuzz)

# SheenBidi
FetchContent_Declare(
    sheenbidi
    GIT_REPOSITORY https://github.com/Tehreer/SheenBidi.git
    GIT_TAG v4.1.1
)
FetchContent_MakeAvailable(sheenbidi)

# MiniAudio
FetchContent_Declare(
    miniaudio
    GIT_REPOSITORY https://github.com/mackron/miniaudio.git
    GIT_TAG 0.11.18
)
FetchContent_MakeAvailable(miniaudio)

# Yoga
FetchContent_Declare(
    yoga
    GIT_REPOSITORY https://github.com/facebook/yoga.git
    GIT_TAG v3.0.4
)
FetchContent_MakeAvailable(yoga)

# Define the static library
add_library(rive STATIC)

# Source files
file(GLOB_RECURSE RIVE_SOURCES "src/*.cpp")
target_sources(rive PRIVATE ${RIVE_SOURCES})

# Include directories
target_include_directories(rive PUBLIC
    "include"
    "${harfbuzz_SOURCE_DIR}/src"
    "${sheenbidi_SOURCE_DIR}/Headers"
    "${miniaudio_SOURCE_DIR}"
    "${yoga_SOURCE_DIR}"
)

# Defines
target_compile_definitions(rive PRIVATE YOGA_EXPORT=)

# Conditional defines based on options
if(WITH_RIVE_TOOLS)
    target_compile_definitions(rive PRIVATE WITH_RIVE_TOOLS)
endif()

if(WITH_RIVE_TEXT)
    target_compile_definitions(rive PRIVATE WITH_RIVE_TEXT)
endif()

if(WITH_RIVE_AUDIO STREQUAL "system")
    target_compile_definitions(rive PRIVATE WITH_RIVE_AUDIO MA_NO_RESOURCE_MANAGER)
elseif(WITH_RIVE_AUDIO STREQUAL "external")
    target_compile_definitions(rive PRIVATE
        WITH_RIVE_AUDIO
        EXTERNAL_RIVE_AUDIO_ENGINE
        MA_NO_DEVICE_IO
        MA_NO_RESOURCE_MANAGER
    )
endif()

if(WITH_RIVE_LAYOUT)
    target_compile_definitions(rive PRIVATE WITH_RIVE_LAYOUT)
endif()

# Platform-specific settings
if(UNIX AND NOT APPLE)  # Linux
    target_compile_definitions(rive PRIVATE MA_NO_RUNTIME_LINKING)
endif()

if(APPLE)  # macOS or iOS
    target_sources(rive PRIVATE "src/text/font_hb_apple.mm")
    if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")  # macOS
        target_compile_options(rive PRIVATE -Wimplicit-float-conversion)
        if(VARIANT STREQUAL "runtime")
            target_compile_options(rive PRIVATE
                -Wimplicit-float-conversion
                -fembed-bitcode
                -arch arm64
                -arch x86_64
                -isysroot $ENV{MACOS_SYSROOT}
            )
        endif()
    endif()
    if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
        target_compile_options(rive PRIVATE -flto=full -Wno-implicit-int-conversion)
        target_sources(rive PRIVATE "src/audio/audio_engine.m")
    endif()
endif()

if(WIN32)
    target_compile_definitions(rive PRIVATE _USE_MATH_DEFINES)
    set_target_properties(rive PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    target_compile_options(rive PRIVATE /W4 /WX)  # Fatal warnings
endif()

# Specific file-based filters
if(NOT WIN32)
    set_source_files_properties("src/audio/audio_engine.cpp"
        PROPERTIES COMPILE_OPTIONS -Wno-implicit-int-conversion
    )
endif()

if(WIN32)
    set_source_files_properties("src/audio/audio_engine.cpp"
        PROPERTIES COMPILE_FLAGS ""  # Remove fatal warnings for this file
    )
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set_source_files_properties("src/audio/audio_engine.cpp"
            PROPERTIES COMPILE_OPTIONS
                "-Wno-nonportable-system-include-path;"
                "-Wno-zero-as-null-pointer-constant;"
                "-Wno-missing-prototypes;"
                "-Wno-cast-qual;"
                "-Wno-format-nonliteral;"
                "-Wno-cast-align;"
                "-Wno-covered-switch-default;"
                "-Wno-comma;"
                "-Wno-tautological-type-limit-compare;"
                "-Wno-extra-semi-stmt;"
                "-Wno-tautological-constant-out-of-range-compare;"
                "-Wno-implicit-fallthrough;"
                "-Wno-implicit-int-conversion;"
                "-Wno-undef;"
                "-Wno-unused-function"
        )
    endif()
endif()

# Handle renames
set(DEPENDENCIES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dependencies")
if(WITH_RIVE_TEXT AND NOT NO_HARFBUZZ_RENAMES)
    target_include_directories(rive PRIVATE ${DEPENDENCIES_DIR})
    target_compile_options(rive PRIVATE -include "rive_harfbuzz_renames.h")
endif()

if(NOT NO_YOGA_RENAMES)
    target_include_directories(rive PRIVATE ${DEPENDENCIES_DIR})
    target_compile_options(rive PRIVATE -include "rive_yoga_renames.h")
endif()

# XCode-specific settings
if(CMAKE_GENERATOR STREQUAL "Xcode")
    target_compile_options(rive PRIVATE -isystem "${yoga_SOURCE_DIR}")
endif()

# Enable fatal warnings globally (except where overridden)
target_compile_options(rive PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Werror>
)
