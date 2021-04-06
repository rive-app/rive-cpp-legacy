#ifndef _RIVE_CLIPPING_SHAPE_BASE_HPP_
#define _RIVE_CLIPPING_SHAPE_BASE_HPP_
#include "component.hpp"
#include "core/field_types/core_bool_type.hpp"
#include "core/field_types/core_uint_type.hpp"
namespace rive
{
	class ClippingShapeBase : public Component
	{
	protected:
		typedef Component Super;

	public:
		static const uint16_t typeKey = 42;

		/// Helper to quickly determine if a core object extends another without
		/// RTTI at runtime.
		bool isTypeOf(uint16_t typeKey) const override
		{
			switch (typeKey)
			{
				case ClippingShapeBase::typeKey:
				case ComponentBase::typeKey:
					return true;
				default:
					return false;
			}
		}

		uint16_t coreType() const override { return typeKey; }

		static const uint16_t sourceIdPropertyKey = 92;
		static const uint16_t fillRulePropertyKey = 93;
		static const uint16_t isVisiblePropertyKey = 94;

	private:
		int m_SourceId = -1;
		int m_FillRule = 0;
		bool m_IsVisible = true;
	public:
		inline int sourceId() const { return m_SourceId; }
		void sourceId(int value)
		{
			if (m_SourceId == value)
			{
				return;
			}
			m_SourceId = value;
			sourceIdChanged();
		}

		inline int fillRule() const { return m_FillRule; }
		void fillRule(int value)
		{
			if (m_FillRule == value)
			{
				return;
			}
			m_FillRule = value;
			fillRuleChanged();
		}

		inline bool isVisible() const { return m_IsVisible; }
		void isVisible(bool value)
		{
			if (m_IsVisible == value)
			{
				return;
			}
			m_IsVisible = value;
			isVisibleChanged();
		}

		bool deserialize(uint16_t propertyKey, BinaryReader& reader) override
		{
			switch (propertyKey)
			{
				case sourceIdPropertyKey:
					m_SourceId = CoreUintType::deserialize(reader);
					return true;
				case fillRulePropertyKey:
					m_FillRule = CoreUintType::deserialize(reader);
					return true;
				case isVisiblePropertyKey:
					m_IsVisible = CoreBoolType::deserialize(reader);
					return true;
			}
			return Component::deserialize(propertyKey, reader);
		}

	protected:
		virtual void sourceIdChanged() {}
		virtual void fillRuleChanged() {}
		virtual void isVisibleChanged() {}
	};
} // namespace rive

#endif