#!/bin/bash
set -e

# pushd ../../
# ./build.sh $@
# popd

# pushd ../library
# ./build.sh $@
# popd

cd build

OPTION=$1

if [ "$OPTION" = 'help' ]; then
    echo build.sh - build debug library
    echo build.sh clean - clean the build
    echo build.sh release - build release library
elif [ "$OPTION" = "clean" ]; then
    echo Cleaning project ...
    premake5 gmake && make clean
elif [ "$OPTION" = "release" ]; then
    premake5 --with-low-level-rendering gmake && make config=release -j7
else
    premake5 --with-low-level-rendering gmake && make -j7
fi
