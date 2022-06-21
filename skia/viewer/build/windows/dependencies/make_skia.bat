@echo off
cd skia
set clang_win = "C:\Program Files\LLVM"
set COMPILER_FLAGS = ^
    \"-Wno-c++98-compat\",^
    \"-Wno-c++98-compat-pedantic\",^
    \"-Wno-reserved-macro-identifier\",^
    \"-Wno-newline-eof\",^
    \"-Wno-old-style-cast\",^
    \"-Wno-unused-parameter\",^
    \"-Wno-float-equal\",^
    \"-Wno-implicit-float-conversion\",^
    \"-Wno-shadow\",^
    \"-Wno-sign-conversion\",^
    \"-Wno-inconsistent-missing-destructor-override\",^
    \"-Wno-nested-anon-types\",^
    \"-Wno-suggest-destructor-override\",^
    \"-Wno-non-virtual-dtor\",^
    \"-Wno-unknown-argument\",^
    \"-Wno-gnu-anonymous-struct\",^
    \"-Wno-extra-semi\",^
    \"-Wno-cast-qual\",^
    \"-Wno-ignored-qualifiers\",^
    \"-Wno-double-promotion\",^
    \"-Wno-tautological-unsigned-zero-compare\",^
    \"-Wno-unreachable-code-break\",^
    \"-Wno-global-constructors\",^
    \"-Wno-switch-enum\",^
    \"-Wno-shorten-64-to-32\",^
    \"-Wno-missing-prototypes\",^
    \"-Wno-implicit-int-conversion\",^
    \"-Wno-unused-macros\",^
    \"-Wno-deprecated-copy-with-user-provided-dtor\",^
    \"-Wno-missing-variable-declarations\",^
    \"-Wno-ctad-maybe-unsupported\",^
    \"-Wno-vla-extension\",^
    \"-Wno-float-conversion\",^
    \"-Wno-gnu-zero-variadic-macro-arguments\",^
    \"-Wno-undef\",^
    \"-Wno-documentation\",^
    \"-Wno-documentation-pedantic\",^
    \"-Wno-documentation-unknown-command\",^
    \"-Wno-suggest-override\",^
    \"-Wno-unused-exception-parameter\",^
    \"-Wno-cast-align\",^
    \"-Wno-deprecated-declarations\",^
    \"-Wno-shadow-field\",^
    \"-Wno-nonportable-system-include-path\",^
    \"-Wno-reserved-identifier\",^
    \"-Wno-thread-safety-negative\",^
    \"-Wno-exit-time-destructors\",^
    \"-Wno-unreachable-code\",^
    \"-Wno-zero-as-null-pointer-constant\",^
    \"-Wno-pedantic\",^
    \"-Wno-sign-compare\",^
    \"-fno-rtti\",^
    \"-DSK_DISABLE_LEGACY_SHADERCONTEXT\", ^
    \"-DSK_DISABLE_LOWP_RASTER_PIPELINE\", ^
    \"-DSK_FORCE_RASTER_PIPELINE_BLITTER\", ^
    \"-DSK_DISABLE_AAA\", ^
    \"-DSK_DISABLE_EFFECT_DESERIALIZATION\", ^
    \"/MT"

set ARGS=extra_cflags=[%COMPILER_FLAGS%] ^
    is_official_build=false^
    skia_use_gl=true^
    skia_use_zlib=true^
    skia_enable_gpu=true^
    skia_enable_fontmgr_empty=false^
    skia_use_libpng_encode=false^
    skia_use_libpng_decode=true^
    skia_enable_skgpu_v1=true^
    skia_use_dng_sdk=false^
    skia_use_egl=false^
    skia_use_expat=false^
    skia_use_fontconfig=false^
    skia_use_freetype=false^
    skia_use_icu=false^
    skia_use_libheif=false^
    skia_use_system_libpng=false^
    skia_use_libjpeg_turbo_encode=false^
    skia_use_libjpeg_turbo_decode=false^
    skia_use_libwebp_encode=false^
    skia_use_libwebp_decode=false^
    skia_use_lua=false^
    skia_use_piex=false^
    skia_use_vulkan=false^
    skia_use_metal=false^
    skia_use_angle=false^
    skia_use_direct3d=false^
    skia_use_system_zlib=false^
    skia_enable_spirv_validation=false^
    skia_enable_pdf=false^
    skia_enable_skottie=false^
    skia_enable_tools=false

bin\gn gen out\static --type=static_library --args="%ARGS%"
ninja -C out\static
cd ..