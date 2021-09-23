#!/bin/bash
set -e
cd ../..
./build.sh "$@"

cd renderer/library

cd build

OPTION=$1

if [ "$OPTION" = 'help' ]; then
    echo build.sh - build debug library
    echo build.sh clean - clean the build
    echo build.sh release - build release library
elif [ "$OPTION" = "clean" ]; then
    echo Cleaning renderer_library project ...
    premake5 clean
elif [ "$OPTION" = "release" ]; then
    premake5 --for-macos gmake && make config=release -j7
elif [ "$OPTION" = "android" ]; then
    premake5 --for-android gmake && make config=release -j7
else
    premake5 --for-macos gmake && make -j7
fi
