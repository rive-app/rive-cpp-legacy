#!/bin/sh
set -e

source ../../../dependencies/linux/config_directories.sh

CONFIG=debug
GRAPHICS=gl
RENDERER=tess

for var in "$@"; do
    if [[ $var = "release" ]]; then
        CONFIG=release
    fi
    if [[ $var = "gl" ]]; then
        GRAPHICS=gl
    fi
    if [[ $var = "tess" ]]; then
        RENDERER=tess
    fi
    if [[ $var = "skia" ]]; then
        RENDERER=skia
    fi
done

if [[ ! -f "$DEPENDENCIES/bin/premake5" ]]; then
    pushd $DEPENDENCIES_SCRIPTS
    ./get_premake5.sh
    popd
fi

if [[ ! -d "$DEPENDENCIES/imgui" ]]; then
    pushd $DEPENDENCIES_SCRIPTS
    ./get_imgui.sh
    popd
fi

if [ $RENDERER = "skia" ]; then
    pushd ../../../skia/renderer/build/linux
    ./build_skia_renderer.sh text $@
    popd
fi

if [ $RENDERER = "tess" ]; then
    pushd ../../../tess/build/linux
    ./build_tess.sh $@
    popd
fi

export PREMAKE=$DEPENDENCIES/bin/premake5

pushd ..

$PREMAKE --scripts=../../build --file=./premake5_viewer.lua gmake2 --graphics=$GRAPHICS --renderer=$RENDERER --with_rive_tools --with_rive_text

for var in "$@"; do
    if [[ $var = "clean" ]]; then
        make clean
        make config=release clean
    fi
done

make config=$CONFIG -j4

popd

for var in "$@"; do
    if [[ $var = "run" ]]; then
        bin/$CONFIG/$RENDERER/$GRAPHICS/rive_viewer
    fi
    if [[ $var = "lldb" ]]; then
        lldb bin/$CONFIG/$RENDERER/$GRAPHICS/rive_viewer
    fi
done
