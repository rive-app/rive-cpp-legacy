#ifndef _RIVE_PAINT_COLOR_HPP_
#define _RIVE_PAINT_COLOR_HPP_
#include <cmath>
#include <cstdint>

namespace rive {
    using ColorInt = uint32_t;

    ColorInt colorARGB(int a, int r, int g, int b);

    unsigned int colorRed(ColorInt value);

    unsigned int colorGreen(ColorInt value);

    unsigned int colorBlue(ColorInt value);

    unsigned int colorAlpha(ColorInt value);

    float colorOpacity(unsigned int value);

    ColorInt colorWithAlpha(ColorInt value, unsigned int a);

    ColorInt colorWithOpacity(ColorInt value, float opacity);

    ColorInt colorModulateOpacity(ColorInt value, float opacity);

    ColorInt colorLerp(ColorInt from, ColorInt to, float mix);
} // namespace rive
#endif
