#ifndef _RIVE_FILL_BASE_HPP_
#define _RIVE_FILL_BASE_HPP_
#include "core/field_types/core_uint_type.hpp"
#include "shapes/paint/shape_paint.hpp"
namespace rive
{
	class FillBase : public ShapePaint
	{
	protected:
		typedef ShapePaint Super;

	public:
		static const uint16_t typeKey = 20;

		/// Helper to quickly determine if a core object extends another without
		/// RTTI at runtime.
		bool isTypeOf(uint16_t typeKey) const override
		{
			switch (typeKey)
			{
				case FillBase::typeKey:
				case ShapePaintBase::typeKey:
				case ContainerComponentBase::typeKey:
				case ComponentBase::typeKey:
					return true;
				default:
					return false;
			}
		}

		uint16_t coreType() const override { return typeKey; }

		static const uint16_t fillRulePropertyKey = 40;

	private:
		int m_FillRule = 0;
	public:
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

		bool deserialize(uint16_t propertyKey, BinaryReader& reader) override
		{
			switch (propertyKey)
			{
				case fillRulePropertyKey:
					m_FillRule = CoreUintType::deserialize(reader);
					return true;
			}
			return ShapePaint::deserialize(propertyKey, reader);
		}

	protected:
		virtual void fillRuleChanged() {}
	};
} // namespace rive

#endif