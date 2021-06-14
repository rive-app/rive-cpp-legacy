#!/bin/sh

set -e

if ! command -v cmake &> /dev/null
then
    echo "cmake is required"
    exit
fi

mkdir -p ../local/sources
cd ../local/sources

GLFW_REPO=https://github.com/glfw/glfw
GLFW_STABLE_BRANCH=master

if [ ! -d glfw ]; then
	echo "Cloning GLFW."
    git clone $GLFW_REPO
fi

cd glfw && git checkout $GLFW_STABLE_BRANCH && git fetch && git pull

mkdir -p build
mkdir -p ../../../../local
cd build
cmake ../ -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=../../../../local
make
make install