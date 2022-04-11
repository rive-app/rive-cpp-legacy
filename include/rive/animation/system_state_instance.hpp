#ifndef _RIVE_SYSTEM_STATE_INSTANCE_HPP_
#define _RIVE_SYSTEM_STATE_INSTANCE_HPP_

#include <string>
#include "rive/animation/state_instance.hpp"

namespace rive {
    /// Represents an instance of a system state machine. Basically a
    /// placeholder that may have meaning to the state machine itself, or is
    /// just a no-op state (perhaps an unknown to this runtime state-type).
    class SystemStateInstance : public StateInstance {
    public:
        SystemStateInstance(const LayerState* layerState, ArtboardInstance* instance);

        void advance(float seconds, SMIInput** inputs) override;
        void apply(float mix) override;

        bool keepGoing() const override;
    };
} // namespace rive
#endif