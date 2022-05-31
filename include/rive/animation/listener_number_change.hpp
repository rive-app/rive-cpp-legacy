#ifndef _RIVE_LISTENER_NUMBER_CHANGE_HPP_
#define _RIVE_LISTENER_NUMBER_CHANGE_HPP_
#include "rive/generated/animation/listener_number_change_base.hpp"

namespace rive {
    class ListenerNumberChange : public ListenerNumberChangeBase {
    public:
        bool validateInputType(const StateMachineInput* input) const override;
        void perform(StateMachineInstance* stateMachineInstance) const override;
    };
} // namespace rive

#endif