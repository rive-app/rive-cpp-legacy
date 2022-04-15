/*
 * Copyright 2022 Rive
 */

#ifdef TESTING

// Don't compile this file as part of the "tests" project.

#else

#include "write_png_file.hpp"

#include "skia/third_party/externals/libpng/png.h"
#include <stdio.h>
#include <vector>

void WritePNGFile(uint8_t* pixels, int width, int height, bool flipY, const char* file_name) {
    FILE* fp = fopen(file_name, "wb");
    if (!fp) {
        fprintf(stderr, "WritePNGFile: File %s could not be opened for writing", file_name);
        return;
    }

    auto png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png) {
        fprintf(stderr, "WritePNGFile: png_create_write_struct failed");
        return;
    }

    // Use pngcrush method 12 for better compression performance.
    // https://stackoverflow.com/questions/22167397/improve-performance-with-libpng
    png_set_compression_level(png, 2);
    png_set_compression_strategy(png, 2);
    png_set_filter(png, 0, PNG_FILTER_SUB);

    auto info = png_create_info_struct(png);
    if (!info) {
        fprintf(stderr, "WritePNGFile: png_create_info_struct failed");
        return;
    }

    if (setjmp(png_jmpbuf(png))) {
        fprintf(stderr, "WritePNGFile: Error during init_io");
        return;
    }

    png_init_io(png, fp);

    // Write header.
    if (setjmp(png_jmpbuf(png))) {
        fprintf(stderr, "WritePNGFile: Error during writing header");
        return;
    }

    png_set_IHDR(png,
                 info,
                 width,
                 height,
                 8,
                 PNG_COLOR_TYPE_RGB_ALPHA,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE,
                 PNG_FILTER_TYPE_BASE);

    png_write_info(png, info);

    // Write bytes.
    if (setjmp(png_jmpbuf(png))) {
        fprintf(stderr, "WritePNGFile: Error during writing bytes");
        return;
    }

    std::vector<uint8_t*> rows(height);
    for (int y = 0; y < height; ++y) {
        rows[y] = pixels + (flipY ? height - 1 - y : y) * width * 4;
    }
    png_write_image(png, rows.data());

    // End write.
    if (setjmp(png_jmpbuf(png))) {
        fprintf(stderr, "WritePNGFile: Error during end of write");
        return;
    }

    png_write_end(png, NULL);

    fclose(fp);
}

#endif
