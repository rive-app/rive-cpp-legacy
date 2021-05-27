#ifndef _RIVE_LINEAR_ANIMATION_HPP_
#define _RIVE_LINEAR_ANIMATION_HPP_
#include "animation/loop.hpp"
#include "generated/animation/linear_animation_base.hpp"
#include <vector>
namespace rive
{
	class Artboard;
	class KeyedObject;

	class LinearAnimation : public LinearAnimationBase
	{
	private:
		std::vector<KeyedObject*> m_KeyedObjects;

	public:
		~LinearAnimation();
		StatusCode onAddedDirty(CoreContext* context) override;
		StatusCode onAddedClean(CoreContext* context) override;
		void addKeyedObject(KeyedObject* object);
		void apply(Artboard* artboard, float time, float mix = 1.0f) const;

		Loop loop() const { return (Loop)loopValue(); }

		StatusCode import(ImportStack& importStack) override;

		float startSeconds() const;
		float endSeconds() const;
		float durationSeconds() const;

#ifdef TESTING
		size_t numKeyedObjects() { return m_KeyedObjects.size(); }
		// Used in testing to check how many animations gets deleted.
		static int deleteCount;
#endif
	};
} // namespace rive

#endif