/*
 * Copyright 2022 Rive
 */

#ifndef WRITE_PNG_FILE_HPP
#define WRITE_PNG_FILE_HPP

#include <stdint.h>

void WritePNGFile(uint8_t* pixels, int width, int height, bool flipY, const char* file_name);

#endif
