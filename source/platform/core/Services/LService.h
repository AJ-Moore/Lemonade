#pragma once

#include <LCommon.h>
#include <Base/Object.h>

namespace Lemonade
{
	class LEMONADE_API LService : public CitrusCore::Object
	{
	public:
		virtual ~LService() {}
	protected:
		virtual bool Init() = 0;
		virtual void Unload() = 0;
		virtual void Update() = 0;
		virtual void Render() = 0;
	};
}