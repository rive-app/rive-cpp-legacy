#!/bin/sh

set -e

# Diligent requires CMake

DILIGENT_REPO=https://github.com/DiligentGraphics/DiligentEngine.git


if [ ! -d DiligentEngine ]; then
	echo "Cloning Diligent."
    git clone --recurse-submodules $DILIGENT_REPO
else
    echo "Already have Diligent, update it."
    cd DiligentEngine && git fetch && git merge master
    cd ..
fi

mkdir DiligentEngine_build
cd DiligentEngine_build
cmake ../DiligentEngine -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=./build
make
make install