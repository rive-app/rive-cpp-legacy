#include "rive/animation/animation_state.hpp"
#include "rive/animation/linear_animation.hpp"
#include "rive/animation/animation_state_instance.hpp"
#include "rive/animation/system_state_instance.hpp"
#include "rive/core_context.hpp"
#include "rive/artboard.hpp"

using namespace rive;

std::unique_ptr<StateInstance> AnimationState::makeInstance() const {
    if (animation() == nullptr) {
        // Failed to load at runtime/some new type we don't understand.
        return std::make_unique<SystemStateInstance>(this);
    }
    return std::make_unique<AnimationStateInstance>(this);
}
