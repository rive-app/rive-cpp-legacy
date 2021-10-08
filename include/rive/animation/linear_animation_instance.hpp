#ifndef _RIVE_LINEAR_ANIMATION_INSTANCE_HPP_
#define _RIVE_LINEAR_ANIMATION_INSTANCE_HPP_
#include "rive/artboard.hpp"

namespace rive
{
	class LinearAnimation;

	class LinearAnimationInstance
	{
	private:
		const LinearAnimation* m_Animation = nullptr;
		float m_Time;
		float m_TotalTime;
		float m_LastTotalTime;
		float m_SpilledTime;
		int m_Direction;
		bool m_DidLoop;
		int m_LoopValue = -1;

	public:
		LinearAnimationInstance(const LinearAnimation* animation);

		// Advance the animation by the specified time. Returns true if the
		// animation will continue to animate after this advance.
		bool advance(float seconds);

		// Returns a pointer to the instance's animation
		const LinearAnimation* animation() const { return m_Animation; }

		// Returns the current point in time at which this instance has advance
		// to
		float time() const { return m_Time; }

		// Returns the direction that we are currently playing in
		float direction() const { return m_Direction; }

		// Update the direction of the animation instance, positive value for
		// forwards Negative for backwards
		void direction(int direction)
		{
			if (direction > 0)
			{
				m_Direction = 1;
			}
			else
			{
				m_Direction = -1;
			}
		}

		// Sets the animation's point in time.
		void time(float value);

		// Applies the animation instance to an artboard. The mix (a value
		// between 0 and 1) is the strength at which the animation is mixed with
		// other animations applied to the artboard.
		void apply(Artboard* artboard, float mix = 1.0f) const
		{
			m_Animation->apply(artboard, m_Time, mix);
		}

		// Set when the animation is advanced, true if the animation has stopped
		// (oneShot), reached the end (loop), or changed direction (pingPong)
		bool didLoop() const { return m_DidLoop; }

		float totalTime() const { return m_TotalTime; }
		float lastTotalTime() const { return m_LastTotalTime; }
		float spilledTime() const { return m_SpilledTime; }
		float durationSeconds() const;

		// Returns either the animation's default or overridden loop values
		Loop loop() { return (Loop)loopValue(); }
		int loopValue();
		// Override the animation's default loop
		void loopValue(int value);
	};
} // namespace rive
#endif