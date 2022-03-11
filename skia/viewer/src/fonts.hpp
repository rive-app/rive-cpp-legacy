#ifndef _RIVE_FONT_HPP_
#define _RIVE_FONT_HPP_

#include "rive/rive_types.hpp"
#include "SkData.h"

#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <freetype/ftadvanc.h>
#include <freetype/ftimage.h>
#include <freetype/ftbitmap.h>
#ifdef FT_COLOR_H  // 2.10.0
#   include <freetype/ftcolor.h>
#endif
#include <freetype/freetype.h>
#include <freetype/ftlcdfil.h>
#include <freetype/ftmodapi.h>
#include <freetype/ftmm.h>
#include <freetype/ftoutln.h>
#include <freetype/ftsizes.h>
#include <freetype/ftsystem.h>
#include <freetype/tttables.h>
#include <freetype/t1tables.h>
#include <freetype/ftfntfmt.h>

typedef struct FT_LibraryRec_* FT_Library;
typedef struct FT_FaceRec_* FT_Face;
typedef struct FT_StreamRec_* FT_Stream;
typedef signed long FT_Pos;
typedef struct FT_BBox_ FT_BBox;

struct PathSink {
    virtual void move(float x, float y) = 0;
    virtual void line(float x, float y) = 0;
    virtual void quad(float x0, float y0, float x1, float y1) = 0;
    virtual void cubic(float x0, float y0, float x1, float y1, float x2, float y2) = 0;
    virtual void close() = 0;
};

struct FTLib {
    FT_Library m_Lib;

    FTLib();
    ~FTLib();

    operator bool() const { return m_Lib != nullptr; }
};

constexpr uint32_t tag(unsigned a, unsigned b, unsigned c, unsigned d) {
    assert(a <= 255 && b <= 255 && c <= 255 && d <= 255);
    return (a << 24) | (b << 16) | (c << 8) | d;
}

struct FTAxis {
    uint32_t    m_Tag;
    float       m_Min, m_Def, m_Max;
};

struct FTCoord {
    uint32_t m_Tag;
    float    m_Value;
};

struct FTFace {
    sk_sp<SkData> m_Data;
    FT_Face m_Face;
    std::vector<FTAxis> m_Axes;
    
    FTFace() : m_Face(nullptr) {}
    ~FTFace();

    bool load(FT_Library lib, sk_sp<SkData> data);

    int axisCount() const { return (int)m_Axes.size(); }
    const FTAxis* axes() const { return m_Axes.data(); }

    operator bool() const { return m_Face != nullptr; }
    
    int upem() const;
    bool setSize(int size);
    void setCoords(const FTCoord[], int count);

    bool getPath(uint16_t glyph, PathSink* sink);
    
private:
    void parseAxes(FT_Library);
};

struct TestSink : public PathSink {
    void move(float x, float y) override {
        printf("Move %g %g\n", x, y);
    }
    void line(float x, float y) override {
        printf("line %g %g\n", x, y);
    }
    void quad(float x0, float y0, float x1, float y1) override {
        printf("quad %g %g  %g %g\n", x0, y0, x1, y1);
    }
    void cubic(float x0, float y0, float x1, float y1, float x2, float y2) override {
        printf("cube %g %g  %g %g  %g %g\n", x0, y0, x1, y1, x2, y2);
    }
    void close() override {
        printf("close\n");
    }
};

#endif
