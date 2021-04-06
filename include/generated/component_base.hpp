#ifndef _RIVE_COMPONENT_BASE_HPP_
#define _RIVE_COMPONENT_BASE_HPP_
#include "core.hpp"
#include "core/field_types/core_string_type.hpp"
#include "core/field_types/core_uint_type.hpp"
#include <string>
namespace rive
{
	class ComponentBase : public Core
	{
	protected:
		typedef Core Super;

	public:
		static const uint16_t typeKey = 10;

		/// Helper to quickly determine if a core object extends another without
		/// RTTI at runtime.
		bool isTypeOf(uint16_t typeKey) const override
		{
			switch (typeKey)
			{
				case ComponentBase::typeKey:
					return true;
				default:
					return false;
			}
		}

		uint16_t coreType() const override { return typeKey; }

		static const uint16_t namePropertyKey = 4;
		static const uint16_t parentIdPropertyKey = 5;

	private:
		std::string m_Name = "";
		int m_ParentId = 0;
	public:
		inline std::string name() const { return m_Name; }
		void name(std::string value)
		{
			if (m_Name == value)
			{
				return;
			}
			m_Name = value;
			nameChanged();
		}

		inline int parentId() const { return m_ParentId; }
		void parentId(int value)
		{
			if (m_ParentId == value)
			{
				return;
			}
			m_ParentId = value;
			parentIdChanged();
		}

		bool deserialize(uint16_t propertyKey, BinaryReader& reader) override
		{
			switch (propertyKey)
			{
				case namePropertyKey:
					m_Name = CoreStringType::deserialize(reader);
					return true;
				case parentIdPropertyKey:
					m_ParentId = CoreUintType::deserialize(reader);
					return true;
			}
			return false;
		}

	protected:
		virtual void nameChanged() {}
		virtual void parentIdChanged() {}
	};
} // namespace rive

#endif