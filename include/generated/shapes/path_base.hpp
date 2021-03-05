#ifndef _RIVE_PATH_BASE_HPP_
#define _RIVE_PATH_BASE_HPP_
#include "core/field_types/core_uint_type.hpp"
#include "node.hpp"
namespace rive
{
	class PathBase : public Node
	{
	protected:
		typedef Node Super;

	public:
		static const int typeKey = 12;

		/// Helper to quickly determine if a core object extends another without
		/// RTTI at runtime.
		bool isTypeOf(int typeKey) const override
		{
			switch (typeKey)
			{
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

		int coreType() const override { return typeKey; }

		static const int pathFlagsPropertyKey = 128;

	private:
		int m_PathFlags = 0;
	public:
		inline int pathFlags() const { return m_PathFlags; }
		void pathFlags(int value)
		{
			if (m_PathFlags == value)
			{
				return;
			}
			m_PathFlags = value;
			pathFlagsChanged();
		}

		bool deserialize(int propertyKey, BinaryReader& reader) override
		{
			switch (propertyKey)
			{
				case pathFlagsPropertyKey:
					m_PathFlags = CoreUintType::deserialize(reader);
					return true;
			}
			return Node::deserialize(propertyKey, reader);
		}

	protected:
		virtual void pathFlagsChanged() {}
	};
} // namespace rive

#endif