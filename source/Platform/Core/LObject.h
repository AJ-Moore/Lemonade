#pragma once

#include "Interface/IInitialisable.h"
#include "Interface/IRenderable.h"
#include "Interface/IUpdateable.h"
#include <LCommon.h>
#include <Util/Logger.h>
#include <Base/Object.h>

namespace Lemonade
{
	/** Base game object class from which Components & other render engine objects are derived. */
	class LEMONADE_API LObject : public CitrusCore::Object, 
								protected CitrusCore::IInitialisable,
								protected CitrusCore::IUpdateable,
								protected CitrusCore::IRenderable
	{
		// Lemonade specific logic.
	};
}
