#!/bin/sh
set -e

source ../../../dependencies/linux/config_directories.sh

if [[ ! -f "$DEPENDENCIES/bin/premake5" ]]; then
    pushd $DEPENDENCIES_SCRIPTS
    ./get_premake5.sh
    popd
fi

if [[ ! -d "$DEPENDENCIES/sokol" ]]; then
    pushd $DEPENDENCIES_SCRIPTS
    ./get_sokol.sh
    popd
fi

if [[ ! -d "$DEPENDENCIES/earcut.hpp" ]]; then
    pushd $DEPENDENCIES_SCRIPTS
    ./get_earcut.sh
    popd
fi

if [[ ! -d "$DEPENDENCIES/libtess2" ]]; then
    pushd $DEPENDENCIES_SCRIPTS
    ./get_libtess2.sh
    popd
fi

export PREMAKE=$DEPENDENCIES/bin/premake5
pushd ..

CONFIG=debug
GRAPHICS=gl
TEST=false
for var in "$@"; do
    if [[ $var = "release" ]]; then
        CONFIG=release
    fi
    if [[ $var = "gl" ]]; then
        GRAPHICS=gl
    fi
    if [[ $var = "test" ]]; then
        TEST=true
    fi
done

$PREMAKE --scripts=../../build --file=./premake5_tess.lua gmake2 --graphics=$GRAPHICS --with_rive_tools

for var in "$@"; do
    if [[ $var = "clean" ]]; then
        make clean
        make config=release clean
    fi
done

# compile shaders
$DEPENDENCIES/bin/sokol-shdc --input ../src/sokol/shader.glsl --output ../src/sokol/generated/shader.h --slang glsl330:hlsl5:metal_macos:metal_ios:metal_sim:glsl300es:glsl100

make config=$CONFIG -j4

if [[ $TEST = "true" ]]; then
    linux/bin/$CONFIG/rive_tess_tests
fi
popd
