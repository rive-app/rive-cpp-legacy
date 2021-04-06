#ifndef _RIVE_POINTS_PATH_BASE_HPP_
#define _RIVE_POINTS_PATH_BASE_HPP_
#include "core/field_types/core_bool_type.hpp"
#include "shapes/path.hpp"
namespace rive
{
	class PointsPathBase : public Path
	{
	protected:
		typedef Path Super;

	public:
		static const uint16_t typeKey = 16;

		/// Helper to quickly determine if a core object extends another without
		/// RTTI at runtime.
		bool isTypeOf(uint16_t typeKey) const override
		{
			switch (typeKey)
			{
				case PointsPathBase::typeKey:
				case PathBase::typeKey:
				case NodeBase::typeKey:
				case TransformComponentBase::typeKey:
				case ContainerComponentBase::typeKey:
				case ComponentBase::typeKey:
					return true;
				default:
					return false;
			}
		}

		uint16_t coreType() const override { return typeKey; }

		static const uint16_t isClosedPropertyKey = 32;

	private:
		bool m_IsClosed = false;
	public:
		inline bool isClosed() const { return m_IsClosed; }
		void isClosed(bool value)
		{
			if (m_IsClosed == value)
			{
				return;
			}
			m_IsClosed = value;
			isClosedChanged();
		}

		bool deserialize(uint16_t propertyKey, BinaryReader& reader) override
		{
			switch (propertyKey)
			{
				case isClosedPropertyKey:
					m_IsClosed = CoreBoolType::deserialize(reader);
					return true;
			}
			return Path::deserialize(propertyKey, reader);
		}

	protected:
		virtual void isClosedChanged() {}
	};
} // namespace rive

#endif