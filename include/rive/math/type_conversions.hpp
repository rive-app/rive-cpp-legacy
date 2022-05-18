/*
 * Copyright 2022 Rive
 */

#ifndef _RIVE_TYPE_CONVERSIONS_HPP_
#define _RIVE_TYPE_CONVERSIONS_HPP_

#include "rive/rive_types.hpp"

namespace rive {

template <typename T> bool fitsIn(intmax_t x) {
    return x >= std::numeric_limits<T>::min() &&
           x <= std::numeric_limits<T>::max();
}

template <typename T> T castTo(intmax_t x) {
    assert(sizeof(x) <= 32);    // don't use with 64bit types
    assert(fitsIn<T>(x));
    return static_cast<T>(x);
}

/*
#define CHECK_CAST(x, type)     \
    assert(fitsIn<type>(x);     \
    return static_cast<type>(x)

constexpr int8_t   toS8(intmax_t x)   { return CHECK_CAST(x, int8_t); }
constexpr uint8_t  toU8(intmax_t x)   { return CHECK_CAST(x, uint8_t); }
constexpr int16_t  toS16(intmax_t x)  { return CHECK_CAST(x, int16_t); }
constexpr uint16_t toU16(intmax_t x)  { return CHECK_CAST(x, uint16_t); }
constexpr int32_t  toS32(intmax_t x)  { return CHECK_CAST(x, int32_t); }
constexpr uint32_t toU32(intmax_t x)  { return CHECK_CAST(x, uint32_t); }
constexpr int      toInt(int64_t x)   { return CHECK_CAST(x, int); }
constexpr unsigned toUInt(int64_t x)  { return CHECK_CAST(x, unsigned); }

#undef CHECK_CAST
 */

} // namespace

#endif
