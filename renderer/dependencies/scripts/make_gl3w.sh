#!/bin/sh

set -e

if ! command -v cmake &> /dev/null
then
    echo "cmake is required"
    exit
fi

mkdir -p ../local/sources
cd ../local/sources

GL3W_REPO=https://github.com/skaslev/gl3w
GL3W_STABLE_BRANCH=master

if [ ! -d gl3w ]; then
	echo "Cloning gl3w."
    git clone $GL3W_REPO
fi

cd gl3w && git checkout $GL3W_STABLE_BRANCH && git fetch && git pull

mkdir -p build
mkdir -p ../../../../local
cd build
cmake ../ -DCMAKE_INSTALL_PREFIX=../../../../local
make
make install