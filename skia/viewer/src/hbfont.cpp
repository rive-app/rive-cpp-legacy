/*
 * Copyright 2022 Rive
 */

#include "rive/factory.hpp"
#include "rive/render_text.hpp"

#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ot.h>

class HBRenderFont : public rive::RenderFont {
    hb_font_t* m_Font;
    hb_draw_funcs_t* m_DrawFuncs;

public:

    // We assume ownership of font!
    HBRenderFont(hb_font_t* font);
    ~HBRenderFont() override;

    std::vector<Axis> getAxes() const override;
    std::vector<Coord> getCoords() const override;
    rive::rcp<rive::RenderFont> makeAtCoords(rive::Span<const Coord>) const override;
    rive::RawPath getPath(rive::GlyphID) const override;

    static rive::rcp<rive::RenderFont> DecodeFont(rive::Span<const uint8_t>);
    std::vector<rive::RenderGlyphRun> ShapeText(rive::Span<const rive::Unichar> text,
                                                rive::Span<const rive::RenderTextRun> runs);
};

rive::rcp<rive::RenderFont> rive_make_harfbuzz_renderfont(rive::Span<const uint8_t> span) {
    return HBRenderFont::DecodeFont(span);
}

//////////////

constexpr int kStdScale = 2048;
constexpr float gInvScale = 64.0f / kStdScale;

extern "C" {
void rpath_move_to(hb_draw_funcs_t*, void* rpath, hb_draw_state_t*, float x, float y, void*) {
    ((rive::RawPath*)rpath)->moveTo(x, y);
}
void rpath_line_to(hb_draw_funcs_t*, void* rpath, hb_draw_state_t*, float x1, float y1, void*) {
    ((rive::RawPath*)rpath)->lineTo(x1, y1);
}
void rpath_quad_to(hb_draw_funcs_t*, void* rpath, hb_draw_state_t*, float x1, float y1, float x2, float y2, void*) {
    ((rive::RawPath*)rpath)->quadTo(x1, y1, x2, y2);
}
void rpath_cubic_to(hb_draw_funcs_t*, void* rpath, hb_draw_state_t*,
                    float x1, float y1, float x2, float y2, float x3, float y3, void*) {
    ((rive::RawPath*)rpath)->cubicTo(x1, y1, x2, y2, x3, y3);
}
void rpath_close(hb_draw_funcs_t*, void* rpath, hb_draw_state_t*, void*) {
    ((rive::RawPath*)rpath)->close();
}
}

HBRenderFont::HBRenderFont(hb_font_t* font) : m_Font(font) {
    // we assume ownership -- no need to call reference()
    hb_font_set_scale(m_Font, kStdScale, kStdScale);

    m_DrawFuncs = hb_draw_funcs_create();
    hb_draw_funcs_set_move_to_func(m_DrawFuncs, rpath_move_to, nullptr, nullptr);
    hb_draw_funcs_set_line_to_func(m_DrawFuncs, rpath_line_to, nullptr, nullptr);
    hb_draw_funcs_set_quadratic_to_func(m_DrawFuncs, rpath_quad_to, nullptr, nullptr);
    hb_draw_funcs_set_cubic_to_func(m_DrawFuncs, rpath_cubic_to, nullptr, nullptr);
    hb_draw_funcs_set_close_path_func(m_DrawFuncs, rpath_close, nullptr, nullptr);
    hb_draw_funcs_make_immutable(m_DrawFuncs);
}

HBRenderFont::~HBRenderFont() {
    hb_draw_funcs_destroy(m_DrawFuncs);
    hb_font_destroy(m_Font);
}

rive::rcp<rive::RenderFont> HBRenderFont::DecodeFont(rive::Span<const uint8_t> span) {
    auto blob = hb_blob_create_or_fail((const char*)span.data(), (unsigned)span.size(),
                                             HB_MEMORY_MODE_DUPLICATE, nullptr, nullptr);
    if (blob) {
       auto face = hb_face_create(blob, 0);
        hb_blob_destroy(blob);
        if (face) {
            auto font = hb_font_create(face);
            hb_face_destroy(face);
            if (font) {
                return rive::rcp<rive::RenderFont>(new HBRenderFont(font));
            }
        }
    }
    return nullptr;
}


std::vector<rive::RenderFont::Axis> HBRenderFont::getAxes() const {
    auto face = hb_font_get_face(m_Font);
    std::vector<rive::RenderFont::Axis> axes;

    const int count = hb_ot_var_get_axis_count(face);
    if (count > 0) {
        axes.resize(count);

        hb_ot_var_axis_info_t info;
        for (int i = 0; i < count; ++i) {
            unsigned n = 1;
            hb_ot_var_get_axis_infos(face, i, &n, &info);
            assert(n == 1);
            axes[i] = { info.tag, info.min_value, info.default_value, info.max_value };
        }
    }
    return axes;
}

std::vector<rive::RenderFont::Coord> HBRenderFont::getCoords() const {
    auto axes = this->getAxes();
    const int count = (int)axes.size();

    unsigned length;
    const float* values = hb_font_get_var_coords_design(m_Font, &length);
    assert((unsigned)count == length);

    std::vector<rive::RenderFont::Coord> coords(count);
    for (int i = 0; i < count; ++i) {
        assert(values[i] >= axes[i].min && values[i] >= axes[i].max);
        coords[i] = { axes[i].tag, values[i] };
    }
    return coords;
}

rive::rcp<rive::RenderFont> HBRenderFont::makeAtCoords(rive::Span<const Coord> coords) const {
    const int count = (int)coords.size();
    std::vector<hb_variation_t> vars(count);
    for (int i = 0; i < count; ++i) {
        vars[i].tag = coords[i].axis;
        vars[i].value = coords[i].value;
    }

    auto font = hb_font_create_sub_font(m_Font);
    hb_font_set_variations(font, vars.data(), count);
    return rive::rcp<rive::RenderFont>(new HBRenderFont(font));
}

rive::RawPath HBRenderFont::getPath(rive::GlyphID glyph) const {
    rive::RawPath rpath;
    hb_font_get_glyph_shape(m_Font, glyph, m_DrawFuncs, &rpath);
    return rpath;
}

///////////////////////////////////////////////////////////
#if 0
static float shapeRun(rive::RenderGlyphRun* grun, const rive::RenderTextRun& trun,
                      const rive::Unichar text[], size_t textOffset, float origin) {
    grun->font = trun.font;
    grun->size = trun.size;
    grun->startTextIndex = textOffset;
    grun->glyphs.resize(trun.unicharCount);
    grun->xpos.resize(trun.unicharCount + 1);

    const int count = SkToInt(trun.unicharCount);

    SkiaRenderFont* rfont = static_cast<SkiaRenderFont*>(trun.font.get());
    rfont->m_Typeface->unicharsToGlyphs(text + textOffset, count, grun->glyphs.data());

    SkFont font(rfont->m_Typeface, grun->size);
    setupFont(&font);

    // We get 'widths' from skia, but then turn them into xpos
    // this will write count values, but xpos has count+1 slots
    font.getWidths(grun->glyphs.data(), count, grun->xpos.data());
    for (auto& xp : grun->xpos) {
        auto width = xp;
        xp = origin;
        origin += width;
    }

    return grun->xpos.data()[count];
}

std::vector<rive::RenderGlyphRun>
SkiaRiveFontMgr::shapeText(rive::Span<const rive::Unichar> text,
                           rive::Span<const rive::RenderTextRun> truns) {
    std::vector<rive::RenderGlyphRun> gruns;

    // sanity check
    size_t count = 0;
    for (const auto& tr : truns) {
        count += tr.unicharCount;
    }
    if (count > text.size()) {
        return gruns;   // not enough text, so abort
    }

    gruns.resize(truns.size());
    int i = 0;
    size_t offset = 0;
    float origin = 0;
    for (const auto& tr : truns) {
        origin = shapeRun(&gruns[i], tr, text.data(), offset, origin);
        offset += tr.unicharCount;
        i += 1;
    }

    return gruns;
}
#endif
