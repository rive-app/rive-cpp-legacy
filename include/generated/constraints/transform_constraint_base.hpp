#ifndef _RIVE_TRANSFORM_CONSTRAINT_BASE_HPP_
#define _RIVE_TRANSFORM_CONSTRAINT_BASE_HPP_
#include "constraints/targeted_constraint.hpp"
#include "core/field_types/core_uint_type.hpp"
namespace rive
{
	class TransformConstraintBase : public TargetedConstraint
	{
	protected:
		typedef TargetedConstraint Super;

	public:
		static const uint16_t typeKey = 83;

		/// Helper to quickly determine if a core object extends another without
		/// RTTI at runtime.
		bool isTypeOf(uint16_t typeKey) const override
		{
			switch (typeKey)
			{
				case TransformConstraintBase::typeKey:
				case TargetedConstraintBase::typeKey:
				case ConstraintBase::typeKey:
				case ComponentBase::typeKey:
					return true;
				default:
					return false;
			}
		}

		uint16_t coreType() const override { return typeKey; }

		static const uint16_t sourceSpaceValuePropertyKey = 179;
		static const uint16_t destSpaceValuePropertyKey = 180;

	private:
		int m_SourceSpaceValue = 0;
		int m_DestSpaceValue = 0;
	public:
		inline int sourceSpaceValue() const { return m_SourceSpaceValue; }
		void sourceSpaceValue(int value)
		{
			if (m_SourceSpaceValue == value)
			{
				return;
			}
			m_SourceSpaceValue = value;
			sourceSpaceValueChanged();
		}

		inline int destSpaceValue() const { return m_DestSpaceValue; }
		void destSpaceValue(int value)
		{
			if (m_DestSpaceValue == value)
			{
				return;
			}
			m_DestSpaceValue = value;
			destSpaceValueChanged();
		}

		Core* clone() const override;
		void copy(const TransformConstraintBase& object)
		{
			m_SourceSpaceValue = object.m_SourceSpaceValue;
			m_DestSpaceValue = object.m_DestSpaceValue;
			TargetedConstraint::copy(object);
		}

		bool deserialize(uint16_t propertyKey, BinaryReader& reader) override
		{
			switch (propertyKey)
			{
				case sourceSpaceValuePropertyKey:
					m_SourceSpaceValue = CoreUintType::deserialize(reader);
					return true;
				case destSpaceValuePropertyKey:
					m_DestSpaceValue = CoreUintType::deserialize(reader);
					return true;
			}
			return TargetedConstraint::deserialize(propertyKey, reader);
		}

	protected:
		virtual void sourceSpaceValueChanged() {}
		virtual void destSpaceValueChanged() {}
	};
} // namespace rive

#endif