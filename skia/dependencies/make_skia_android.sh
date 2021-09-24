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
    echo "Cloning Skia stable branch..."
    # TODO:(umberto) --depth 1 test
    git clone -b $SKIA_STABLE_BRANCH --depth 1 $SKIA_REPO
fi

ARCH=$1

if [ "$ARCH" != "x86" ] &&
    [ "$ARCH" != "x64" ] &&
    [ "$ARCH" != "arm" ] &&
    [ "$ARCH" != "arm64" ]; then
    printf "Invalid architecture: '%s'. Choose one between 'x86', 'x64', 'arm', or 'arm64'" "$ARCH"
    exit 1
fi

cd skia

python tools/git-sync-deps

# Useful for debugging:
# bin/gn args --list out/${ARCH}

bin/gn gen out/"${ARCH}" --args=" \
    ndk=\"${NDK_PATH}\" \
    target_cpu=\"${ARCH}\" \
    is_official_build=true \
    skia_use_egl=true \
    skia_use_gl=true \
    skia_gl_standard=\"gles\" 
    \
    skia_use_freetype=false \
    skia_use_libjpeg_turbo_decode=false \
    skia_use_libjpeg_turbo_encode=false \
    skia_use_libpng_decode=false \
    skia_use_libpng_encode=false \
    skia_use_libwebp_decode=false \
    skia_use_libwebp_encode=false \
    skia_use_libgifcodec=false \
    skia_enable_pdf=false \
    skia_enable_svg=false \
    skia_enable_skrive=false \
    skia_enable_skshaper=false \
    skia_enable_sksl=false \
    skia_enable_sktext=false \
    skia_enable_skottie=false \
    skia_enable_skparagraph=false \
    skia_enable_particles=false \
    skia_pdf_subset_harfbuzz=false \
    skia_use_fixed_gamma_text=false \
    skia_use_piex=false \
    skia_use_sfntly=false \
    skia_use_xps=false \
    skia_use_x11=false \
    skia_use_fontconfig=false \
    skia_use_system_expat=false \
    paragraph_tests_enabled=false \
    \
    skia_use_angle=false \
    skia_use_dng_sdk=false \
    skia_use_expat=false \
    skia_use_icu=false \
    skia_use_libheif=false \
    skia_use_lua=false \
    skia_use_zlib=true \
    skia_enable_fontmgr_empty=false \
    skia_enable_spirv_validation=false \
    "

ninja -C out/"${ARCH}"
cd ..
