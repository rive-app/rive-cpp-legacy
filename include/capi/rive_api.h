#ifndef _RIVE_FILE_H_
#define _RIVE_FILE_H_

#include "capi/rive_types.h"

RIVE_C_PLUS_PLUS_BEGIN_GUARD

rive_file_t* rive_file_import(rive_span_t, rive_factory_t*);

int32_t rive_file_artboard_count(rive_file_t*);
rive_artboard_t* rive_file_artboard_default(rive_file_t*);
rive_artboard_t* rive_file_artboard_at(rive_file_t*, int32_t index);
rive_artboard_t* rive_file_artboard_named(rive_file_t*, const char name[]);

rive_statemachine_t* rive_artboard_statemachine_default(rive_artboard_t*);
rive_scene_t* rive_artboard_scene_default(rive_artboard_t*);

int32_t rive_artboard_animation_count(rive_artboard_t*);
rive_animation_t* rive_artboard_animation_at(rive_artboard_t*, int32_t index);
rive_animation_t* rive_artboard_animation_named(rive_artboard_t*, const char name[]);

int32_t rive_artboard_count_statemachines(rive_artboard_t*);
rive_statemachine_t* rive_artboard_statemachine_at(rive_artboard_t*, int32_t index);
rive_statemachine_t* rive_artboard_statemachine_named(rive_artboard_t*, const char name[]);

rive_aabb_t rive_scene_bounds(rive_scene_t*);
void rive_scene_draw(rive_scene_t*, rive_renderer_t*);
bool rive_scene_advance(rive_scene_t*, float seconds);

void rive_file_delete(rive_file_t*);
void rive_artboard_delete(rive_artboard_t*);
void rive_scene_delete(rive_scene_t*);

RIVE_C_PLUS_PLUS_END_GUARD

#endif
