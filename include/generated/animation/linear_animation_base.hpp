#ifndef _RIVE_LINEAR_ANIMATION_BASE_HPP_
#define _RIVE_LINEAR_ANIMATION_BASE_HPP_
#include "animation/animation.hpp"
#include "core/field_types/core_bool_type.hpp"
#include "core/field_types/core_double_type.hpp"
#include "core/field_types/core_uint_type.hpp"
namespace rive
{
	class LinearAnimationBase : public Animation
	{
	protected:
		typedef Animation Super;

	public:
		static const uint16_t typeKey = 31;

		/// Helper to quickly determine if a core object extends another without
		/// RTTI at runtime.
		bool isTypeOf(uint16_t typeKey) const override
		{
			switch (typeKey)
			{
				case LinearAnimationBase::typeKey:
				case AnimationBase::typeKey:
					return true;
				default:
					return false;
			}
		}

		uint16_t coreType() const override { return typeKey; }

		static const uint16_t fpsPropertyKey = 56;
		static const uint16_t durationPropertyKey = 57;
		static const uint16_t speedPropertyKey = 58;
		static const uint16_t loopValuePropertyKey = 59;
		static const uint16_t workStartPropertyKey = 60;
		static const uint16_t workEndPropertyKey = 61;
		static const uint16_t enableWorkAreaPropertyKey = 62;

	private:
		int m_Fps = 60;
		int m_Duration = 60;
		float m_Speed = 1.0f;
		int m_LoopValue = 0;
		int m_WorkStart = -1;
		int m_WorkEnd = -1;
		bool m_EnableWorkArea = false;
	public:
		inline int fps() const { return m_Fps; }
		void fps(int value)
		{
			if (m_Fps == value)
			{
				return;
			}
			m_Fps = value;
			fpsChanged();
		}

		inline int duration() const { return m_Duration; }
		void duration(int value)
		{
			if (m_Duration == value)
			{
				return;
			}
			m_Duration = value;
			durationChanged();
		}

		inline float speed() const { return m_Speed; }
		void speed(float value)
		{
			if (m_Speed == value)
			{
				return;
			}
			m_Speed = value;
			speedChanged();
		}

		inline int loopValue() const { return m_LoopValue; }
		void loopValue(int value)
		{
			if (m_LoopValue == value)
			{
				return;
			}
			m_LoopValue = value;
			loopValueChanged();
		}

		inline int workStart() const { return m_WorkStart; }
		void workStart(int value)
		{
			if (m_WorkStart == value)
			{
				return;
			}
			m_WorkStart = value;
			workStartChanged();
		}

		inline int workEnd() const { return m_WorkEnd; }
		void workEnd(int value)
		{
			if (m_WorkEnd == value)
			{
				return;
			}
			m_WorkEnd = value;
			workEndChanged();
		}

		inline bool enableWorkArea() const { return m_EnableWorkArea; }
		void enableWorkArea(bool value)
		{
			if (m_EnableWorkArea == value)
			{
				return;
			}
			m_EnableWorkArea = value;
			enableWorkAreaChanged();
		}

		bool deserialize(uint16_t propertyKey, BinaryReader& reader) override
		{
			switch (propertyKey)
			{
				case fpsPropertyKey:
					m_Fps = CoreUintType::deserialize(reader);
					return true;
				case durationPropertyKey:
					m_Duration = CoreUintType::deserialize(reader);
					return true;
				case speedPropertyKey:
					m_Speed = CoreDoubleType::deserialize(reader);
					return true;
				case loopValuePropertyKey:
					m_LoopValue = CoreUintType::deserialize(reader);
					return true;
				case workStartPropertyKey:
					m_WorkStart = CoreUintType::deserialize(reader);
					return true;
				case workEndPropertyKey:
					m_WorkEnd = CoreUintType::deserialize(reader);
					return true;
				case enableWorkAreaPropertyKey:
					m_EnableWorkArea = CoreBoolType::deserialize(reader);
					return true;
			}
			return Animation::deserialize(propertyKey, reader);
		}

	protected:
		virtual void fpsChanged() {}
		virtual void durationChanged() {}
		virtual void speedChanged() {}
		virtual void loopValueChanged() {}
		virtual void workStartChanged() {}
		virtual void workEndChanged() {}
		virtual void enableWorkAreaChanged() {}
	};
} // namespace rive

#endif