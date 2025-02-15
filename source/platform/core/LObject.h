#pragma once

#include <LCommon.h>
#include <platform/core/Interface/IManagedObject.h>
#include <Util/Logger.h>
#include <Base/Object.h>

namespace Lemonade
{
	/** Base game object class from which Components & other render engine objects are derived. */
	class LEMONADE_API LObject : public CitrusCore::Object
	{
		// Lemonade specific logic.
	};
}
