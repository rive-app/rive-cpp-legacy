#include "capi/rive_api.h"

#include "rive/file.hpp"
#include "rive/artboard.hpp"
#include "rive/span.hpp"
#include "rive/scene.hpp"
#include "rive/animation/linear_animation_instance.hpp"
#include "rive/animation/state_machine_instance.hpp"
#include "rive/math/aabb.hpp"

static inline rive_file_t* toC(rive::File* file) { return (rive_file_t*)file; }
static inline rive_artboard_t* toC(rive::ArtboardInstance* abi) { return (rive_artboard_t*)abi; }
static inline rive_scene_t* toC(rive::Scene* scene) { return (rive_scene_t*)scene; }
static inline rive_animation_t* toC(rive::LinearAnimationInstance* anim) {
    return (rive_animation_t*)anim;
}
static inline rive_statemachine_t* toC(rive::StateMachineInstance* smi) {
    return (rive_statemachine_t*)smi;
}

static inline rive::Factory* toRive(rive_factory_t* factory) { return (rive::Factory*)factory; }
static inline rive::Renderer* toRive(rive_renderer_t* renderer) { return (rive::Renderer*)renderer; }

static inline rive::File* toRive(rive_file_t* file) { return (rive::File*)file; }
static inline rive::ArtboardInstance* toRive(rive_artboard_t* abi) { return (rive::ArtboardInstance*)abi; }
static inline rive::Scene* toRive(rive_scene_t* scene) { return (rive::Scene*)scene; }

#if 0
static inline rive_factory_t* toC(rive::Factory* factory) { return (rive_factory_t*)factory; }
static inline rive_renderer_t* toC(rive::Renderer* renderer) { return (rive_renderer_t*)renderer; }
static inline rive::LinearAnimationInstance* toRive(rive_animation_t* anim) {
    return (rive::LinearAnimationInstance*)anim;
}
static inline rive::StateMachineInstance* toRive(rive_statemachine_t* smi) {
    return (rive::StateMachineInstance*)smi;
}
#endif

//////////////////////////////////////

void rive_file_delete(rive_file_t* file) { delete toRive(file); }
void rive_artboard_delete(rive_artboard_t* abi) { delete toRive(abi); }
void rive_scene_delete(rive_scene_t* scene) { delete toRive(scene); }

rive_file_t* rive_file_import(rive_span_t cspan, rive_factory_t* factory) {
    rive::Span<const uint8_t> span((const uint8_t*)cspan.buffer, cspan.size);
    return toC(rive::File::import(span, toRive(factory)).release());
}

int32_t rive_file_artboard_count(rive_file_t* file) {
    return toRive(file)->artboardCount();
}

rive_artboard_t* rive_file_artboard_default(rive_file_t* file) {
    return toC(toRive(file)->artboardDefault().release());
}

rive_artboard_t* rive_file_artboard_at(rive_file_t* file, int32_t index) {
    return toC(toRive(file)->artboardAt(index).release());
}

rive_artboard_t* rive_file_artboard_named(rive_file_t* file, const char name[]) {
    return toC(toRive(file)->artboardNamed(name).release());
}

///////////////////////////

int32_t rive_artboard_animation_count(rive_artboard_t* abi) {
    return toRive(abi)->animationCount();
}

rive_animation_t* rive_artboard_animation_at(rive_artboard_t* abi, int32_t index) {
    return toC(toRive(abi)->animationAt(index).release());
}

rive_animation_t* rive_artboard_animation_named(rive_artboard_t* abi, const char name[]) {
    return toC(toRive(abi)->animationNamed(name).release());
}

int32_t rive_artboard_statemachine_count(rive_artboard_t* abi) {
    return toRive(abi)->stateMachineCount();
}

rive_statemachine_t* rive_artboard_statemachine_at(rive_artboard_t* abi, int32_t index) {
    return toC(toRive(abi)->stateMachineAt(index).release());
}

rive_statemachine_t* rive_artboard_statemachine_named(rive_artboard_t* abi, const char name[]) {
    return toC(toRive(abi)->stateMachineNamed(name).release());
}

rive_statemachine_t* rive_artboard_statemachine_default(rive_artboard_t* abi) {
    return toC(toRive(abi)->defaultStateMachine().release());
}

rive_scene_t* rive_artboard_scene_default(rive_artboard_t* abi) {
    return toC(toRive(abi)->defaultScene().release());
}

////////////////////////////////

rive_aabb_t rive_scene_bounds(rive_scene_t* scene) {
    auto aabb = toRive(scene)->bounds();
    return {aabb.left(), aabb.top(), aabb.right(), aabb.bottom()};
}

void rive_scene_draw(rive_scene_t* scene, rive_renderer_t* renderer) {
    toRive(scene)->draw(toRive(renderer));
}

bool rive_scene_advance(rive_scene_t* scene, float seconds) {
    return toRive(scene)->advanceAndApply(seconds);
}
