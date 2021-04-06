#ifndef _RIVE_DRAW_TARGET_BASE_HPP_
#define _RIVE_DRAW_TARGET_BASE_HPP_
#include "component.hpp"
#include "core/field_types/core_uint_type.hpp"
namespace rive
{
	class DrawTargetBase : public Component
	{
	protected:
		typedef Component Super;

	public:
		static const uint16_t typeKey = 48;

		/// Helper to quickly determine if a core object extends another without
		/// RTTI at runtime.
		bool isTypeOf(uint16_t typeKey) const override
		{
			switch (typeKey)
			{
				case DrawTargetBase::typeKey:
				case ComponentBase::typeKey:
					return true;
				default:
					return false;
			}
		}

		uint16_t coreType() const override { return typeKey; }

		static const uint16_t drawableIdPropertyKey = 119;
		static const uint16_t placementValuePropertyKey = 120;

	private:
		int m_DrawableId = -1;
		int m_PlacementValue = 0;
	public:
		inline int drawableId() const { return m_DrawableId; }
		void drawableId(int value)
		{
			if (m_DrawableId == value)
			{
				return;
			}
			m_DrawableId = value;
			drawableIdChanged();
		}

		inline int placementValue() const { return m_PlacementValue; }
		void placementValue(int value)
		{
			if (m_PlacementValue == value)
			{
				return;
			}
			m_PlacementValue = value;
			placementValueChanged();
		}

		bool deserialize(uint16_t propertyKey, BinaryReader& reader) override
		{
			switch (propertyKey)
			{
				case drawableIdPropertyKey:
					m_DrawableId = CoreUintType::deserialize(reader);
					return true;
				case placementValuePropertyKey:
					m_PlacementValue = CoreUintType::deserialize(reader);
					return true;
			}
			return Component::deserialize(propertyKey, reader);
		}

	protected:
		virtual void drawableIdChanged() {}
		virtual void placementValueChanged() {}
	};
} // namespace rive

#endif