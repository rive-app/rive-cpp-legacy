#ifndef _RIVE_TYPES_H_
#define _RIVE_TYPES_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
    #define RIVE_C_PLUS_PLUS_BEGIN_GUARD    extern "C" {
    #define RIVE_C_PLUS_PLUS_END_GUARD      }
#else
    #include <stdbool.h>
    #define RIVE_C_PLUS_PLUS_BEGIN_GUARD
    #define RIVE_C_PLUS_PLUS_END_GUARD
#endif

RIVE_C_PLUS_PLUS_BEGIN_GUARD

typedef uint32_t rive_colorint_t;

typedef struct {
    float x, y;
} rive_vec2d_t;

typedef struct {
    float left, top, right, bottom;
} rive_aabb_t;

typedef struct {
    float mat[6];
} rive_mat2d_t;

typedef struct {
    void* buffer;
    size_t size;
} rive_span_t;

typedef struct rive_file_t rive_file_t;
typedef struct rive_artboard_t rive_artboard_t;
typedef struct rive_scene_t rive_scene_t;
typedef struct rive_animation_t rive_animation_t;
typedef struct rive_statemachine_t rive_statemachine_t;

typedef struct rive_factory_t rive_factory_t;
typedef struct rive_renderer_t rive_renderer_t;

RIVE_C_PLUS_PLUS_END_GUARD

#endif
