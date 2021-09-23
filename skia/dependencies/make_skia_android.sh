#!/bin/bash

set -e

# Requires depot_tools and git:
#   https://skia.org/user/download
# Build notes:
#   https://skia.org/user/build

SKIA_REPO=https://github.com/google/skia
# New Skia version is compatible with Android NDK > v19
SKIA_STABLE_BRANCH=chrome/m95

# -----------------------------
# Get & Build Skia
# -----------------------------
if [ ! -d skia ]; then
    echo "Cloning Skia stable branch."
    # TODO:(umberto) --depth 1
    git clone -b $SKIA_STABLE_BRANCH $SKIA_REPO
fi

cd skia

python tools/git-sync-deps

# TODO: Pass these as arguments.
# ARCH=x86
ARCH=x64
# ARCH=arm
# ARCH=arm64

# Useful for debugging the build process.
# bin/gn args --list out/${ARCH}
bin/gn gen out/${ARCH} --args=" \
    ndk=\"${NDK_PATH}\" \
    target_cpu=\"${ARCH}\" \
    is_official_build=false \
    skia_use_egl=true \
    skia_use_gl=true \
    skia_gl_standard=\"gles\" \
    "
ninja -C out/${ARCH}
cd ..
