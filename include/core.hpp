#ifndef _RIVE_CORE_HPP_
#define _RIVE_CORE_HPP_

#include "core/binary_reader.hpp"
#include "status_code.hpp"
#include <cassert>

namespace rive
{
	class CoreContext;
	class ImportStack;
	class Core
	{
	public:
		static const int invalidPropertyKey = 0;
		virtual ~Core() {}
		virtual uint16_t coreType() const = 0;
		virtual bool isTypeOf(uint16_t typeKey) const = 0;
		virtual bool deserialize(uint16_t propertyKey,
		                         BinaryReader& reader) = 0;

		template <typename T> bool is() const { return isTypeOf(T::typeKey); }
		template <typename T> T* as()
		{
			assert(is<T>());
			return reinterpret_cast<T*>(this);
		}

		template <typename T> const T* as() const
		{
			assert(is<T>());
			return reinterpret_cast<const T*>(this);
		}

		/// Called when the object is first added to the context, other objects
		/// may not have resolved their dependencies yet. This is an opportunity
		/// to look up objects referenced by id, but not assume that they in
		/// turn have resolved their references yet. Called during
		/// load/instance.
		virtual StatusCode onAddedDirty(CoreContext* context) = 0;

		/// Called when all the objects in the context have had onAddedDirty
		/// called. This is an opportunity to reference things referenced by
		/// dependencies. (A path should be able to find a Shape somewhere in
		/// its hierarchy, which may be multiple levels up).
		virtual StatusCode onAddedClean(CoreContext* context) = 0;

		virtual StatusCode import(ImportStack& importStack)
		{
			return StatusCode::Ok;
		}
	};
} // namespace rive
#endif