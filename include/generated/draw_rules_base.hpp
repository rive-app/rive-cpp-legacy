#ifndef _RIVE_DRAW_RULES_BASE_HPP_
#define _RIVE_DRAW_RULES_BASE_HPP_
#include "container_component.hpp"
#include "core/field_types/core_uint_type.hpp"
namespace rive
{
	class DrawRulesBase : public ContainerComponent
	{
	protected:
		typedef ContainerComponent Super;

	public:
		static const uint16_t typeKey = 49;

		/// Helper to quickly determine if a core object extends another without
		/// RTTI at runtime.
		bool isTypeOf(uint16_t typeKey) const override
		{
			switch (typeKey)
			{
				case DrawRulesBase::typeKey:
				case ContainerComponentBase::typeKey:
				case ComponentBase::typeKey:
					return true;
				default:
					return false;
			}
		}

		uint16_t coreType() const override { return typeKey; }

		static const uint16_t drawTargetIdPropertyKey = 121;

	private:
		int m_DrawTargetId = -1;
	public:
		inline int drawTargetId() const { return m_DrawTargetId; }
		void drawTargetId(int value)
		{
			if (m_DrawTargetId == value)
			{
				return;
			}
			m_DrawTargetId = value;
			drawTargetIdChanged();
		}

		bool deserialize(uint16_t propertyKey, BinaryReader& reader) override
		{
			switch (propertyKey)
			{
				case drawTargetIdPropertyKey:
					m_DrawTargetId = CoreUintType::deserialize(reader);
					return true;
			}
			return ContainerComponent::deserialize(propertyKey, reader);
		}

	protected:
		virtual void drawTargetIdChanged() {}
	};
} // namespace rive

#endif