#include "fonts.hpp"

#include <cmath>
#include <string>

template <typename T> T pin(T v, T mn, T mx) {
    return std::min(std::max(v, mn), mx);
}

struct FTPathSinkCaller {
private:
    PathSink* m_Sink;
    // we have to manually close contours
    bool m_ReadyToClose = false;

    static inline float dot6tofloat(int32_t x) {
        return x * (1.0f/64);
    }

    static int Move(const FT_Vector* pt, void* ctx) {
        auto caller = (FTPathSinkCaller*)ctx;
        if (caller->m_ReadyToClose) {
            caller->m_Sink->close();
            caller->m_ReadyToClose = false;
        }
        caller->m_Sink->move(dot6tofloat(pt->x), -dot6tofloat(pt->y));
        return 0;
    }

    static int Line(const FT_Vector* pt, void* ctx) {
        auto caller = (FTPathSinkCaller*)ctx;
        caller->m_Sink->line(dot6tofloat(pt->x), -dot6tofloat(pt->y));
        caller->m_ReadyToClose = true;
        return 0;
    }

    static int Quad(const FT_Vector* pt0, const FT_Vector* pt1, void* ctx) {
        auto caller = (FTPathSinkCaller*)ctx;
        caller->m_Sink->quad(dot6tofloat(pt0->x), -dot6tofloat(pt0->y),
                             dot6tofloat(pt1->x), -dot6tofloat(pt1->y));
        caller->m_ReadyToClose = true;
        return 0;
    }

    static int Cubic(const FT_Vector* pt0, const FT_Vector* pt1, const FT_Vector* pt2, void* ctx) {
        auto caller = (FTPathSinkCaller*)ctx;
        caller->m_Sink->cubic(dot6tofloat(pt0->x), -dot6tofloat(pt0->y),
                              dot6tofloat(pt1->x), -dot6tofloat(pt1->y),
                              dot6tofloat(pt2->x), -dot6tofloat(pt2->y));
        caller->m_ReadyToClose = true;
        return 0;
    }

public:
    FTPathSinkCaller(PathSink* sink) : m_Sink(sink) {}

    inline static constexpr const FT_Outline_Funcs Funcs{
        FTPathSinkCaller::Move,
        FTPathSinkCaller::Line,
        FTPathSinkCaller::Quad,
        FTPathSinkCaller::Cubic,
        0, // shift
        0,  // delta
    };
};

FTLib::FTLib() : m_Lib(nullptr) {
    int err = FT_Init_FreeType(&m_Lib);
    if (err) {
        printf("FT_Init_FreeType returned %d\n", err);
        return;
    }

    FT_Add_Default_Modules(m_Lib);
    FT_Set_Default_Properties(m_Lib);
}

FTLib::~FTLib() {
    if (m_Lib) {
        FT_Done_Library(m_Lib);
    }
}

FTFace::~FTFace() {
    if (m_Face) {
        FT_Done_Face(m_Face);
    }
}

static std::string tagname(uint32_t x) {
    std::string str("1234");
    str[0] = (x >> 24) & 0xFF;
    str[1] = (x >> 16) & 0xFF;
    str[2] = (x >>  8) & 0xFF;
    str[3] = (x >>  0) & 0xFF;
    return str;
}

static float fixed2float(int32_t x) {
    return x * (1.0f/65536);
}

static int32_t float2fixed(float x) {
    return (int)std::floor(x * 65536 + 0.5f);
}

void FTFace::parseAxes(FT_Library lib) {
    m_Axes.clear();
    
    FT_MM_Var* var;
    int err = FT_Get_MM_Var(m_Face, &var);
    if (err) {
        printf("failed getting variations %d\n", err);
        return;
    }

    assert(m_Face->face_flags & FT_FACE_FLAG_MULTIPLE_MASTERS);

    printf("%d axes\n", var->num_axis);
    for (unsigned i = 0; i < var->num_axis; ++i) {
        m_Axes.push_back({
            (uint32_t)var->axis[i].tag,
            fixed2float(var->axis[i].minimum),
            fixed2float(var->axis[i].def),
            fixed2float(var->axis[i].maximum),
        });
        printf("  %s %g %g %g\n",
               tagname(var->axis[i].tag).c_str(),
               m_Axes[i].m_Min, m_Axes[i].m_Def, m_Axes[i].m_Max);
    }
    FT_Done_MM_Var(lib, var);
}

bool FTFace::load(FT_Library lib, sk_sp<SkData> data) {
    int face_index = 0; // todo
    int err = FT_New_Memory_Face(lib, (const FT_Byte*)data->data(),
                                 data->size(), face_index, &m_Face);
    if (err) {
        printf("FT_New_Memory_Face returned %d\n", err);
        return false;
    }
    m_Data = std::move(data);
    
    this->parseAxes(lib);
    
    return true;
}

void FTFace::setCoords(const FTCoord user[], int count) {
    std::vector<FT_Fixed> ftc(m_Axes.size());
    for (size_t i = 0; i < m_Axes.size(); ++i) {
        const auto& a = m_Axes[i];
        float value = a.m_Def;
        for (int j = 0; j < count; ++j) {
            if (a.m_Tag == user[j].m_Tag) {
                value = user[j].m_Value;
            }
        }
        ftc[i] = float2fixed(pin(value, a.m_Min, a.m_Max));
    }
    
    int err = FT_Set_Var_Design_Coordinates(m_Face, ftc.size(), ftc.data());
    if (err) {
        printf("set design coords failed %d\n", err);
    }
}

int FTFace::upem() const {
    assert(m_Face);
    return m_Face->units_per_EM;
}

bool FTFace::setSize(int size) {
    assert(m_Face);
    int err = FT_Set_Char_Size(m_Face, size * 64, 0, 72, 72);
    if (err) {
        printf("failed to set size %d\n", size);
        return false;
    }
    return true;
}

bool FTFace::getPath(uint16_t glyph, PathSink* sink) {
    unsigned flags = FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP;
    flags &= ~FT_LOAD_RENDER;

    int err = FT_Load_Glyph(m_Face, glyph, flags);
    assert(err == 0);

    FTPathSinkCaller caller(sink);
    err = FT_Outline_Decompose(&m_Face->glyph->outline,
                               &FTPathSinkCaller::Funcs,
                               &caller);
    if (err) {
        printf("failed calling decompose %d\n", err);
        return false;
    }
    sink->close();
    return true;
}
