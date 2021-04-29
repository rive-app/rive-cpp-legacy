#ifndef _RIVE_STATE_MACHINE_INSTANCE_HPP_
#define _RIVE_STATE_MACHINE_INSTANCE_HPP_

#include <string>
#include <stddef.h>
#include "animation/linear_animation_instance.hpp"

namespace rive
{
	class StateMachine;
	class LayerState;
	class SMIInput;
	class Artboard;
	class SMIBool;
	class SMINumber;
	class SMITrigger;

	class StateMachineLayerInstance;

	class StateMachineInstance
	{
		friend class SMIInput;

	private:
		StateMachine* m_Machine;
		bool m_NeedsAdvance = false;

		size_t m_InputCount;
		SMIInput** m_InputInstances;
		size_t m_LayerCount;
		StateMachineLayerInstance* m_Layers;

		void markNeedsAdvance();

	public:
		StateMachineInstance(StateMachine* machine);
		~StateMachineInstance();

		// Advance the state machine by the specified time. Returns true if the
		// state machine will continue to animate after this advance.
		bool advance(float seconds);

		// Applies the animations of the StateMachine to an artboard.
		void apply(Artboard* artboard) const;

		// Returns true when the StateMachineInstance has more data to process.
		bool needsAdvance() const;

		size_t inputCount() const { return m_InputCount; }
		SMIInput* input(size_t index) const;

		SMIBool* getBool(std::string name) const;
		SMINumber* getNumber(std::string name) const;
		SMITrigger* getTrigger(std::string name) const;

		const size_t currentAnimationCount() const;
		const LinearAnimationInstance* currentAnimationByIndex(size_t index) const;

		// The number of state changes that occurred across all layers on the
		// previous advance.
		size_t stateChangedCount() const;

		// Returns the state name for states that changed in layers on the
		// previously called advance. If the index of out of range, it returns
		// the empty string.
		const LayerState* stateChangedByIndex(size_t index) const;
	};
} // namespace rive
#endif
