#ifndef _RIVE_LISTENER_BOOL_CHANGE_HPP_
#define _RIVE_LISTENER_BOOL_CHANGE_HPP_
#include "rive/generated/animation/listener_bool_change_base.hpp"

namespace rive {
    class ListenerBoolChange : public ListenerBoolChangeBase {
    public:
        bool validateInputType(const StateMachineInput* input) const override;
        void perform(StateMachineInstance* stateMachineInstance) const override;
    };
} // namespace rive

#endif