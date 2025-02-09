#pragma once

#include <LCommon.h>
#include <platform/core/Interface/IManagedObject.h>
#include <Util/Logger.h>

namespace Lemonade
{
	/** Base game object class from which Components & other render engine objects are derived. */
	class LEMONADE_API LObject : protected IManagedObject
	{
	public:
		virtual ~LObject() {}

		void SetName(const std::string& name) { m_name = name; }
		const std::string& GetName() const { return m_name; }

		CitrusCore::UID UID;
	private:
		std::string m_name = "LemonObject";
	};
}
