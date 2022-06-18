#include "rive/animation/listener_number_change.hpp"
#include "rive/animation/state_machine_instance.hpp"
#include "rive/animation/state_machine_number.hpp"
#include "rive/animation/state_machine_input_instance.hpp"

using namespace rive;

bool ListenerNumberChange::validateInputType(const StateMachineInput* input) const {
    // A null input is valid as the StateMachine can attempt to limp along if we
    // introduce new input types that old conditions are expected to handle in
    // newer runtimes. The older runtimes will just evaluate them to true.
    return input == nullptr || input->is<StateMachineNumber>();
}

void ListenerNumberChange::perform(StateMachineInstance* stateMachineInstance,
                                   Vec2D position) const {
    auto inputInstance = stateMachineInstance->input(inputId());
    if (inputInstance == nullptr) {
        return;
    }
    // If it's not null, it must be our correct type (why we validate at load time).
    auto numberInput = reinterpret_cast<SMINumber*>(inputInstance);
    numberInput->value(value());
}
