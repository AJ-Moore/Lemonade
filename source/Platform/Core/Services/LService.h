#pragma once

#include <Services/AService.h>
#include <LCommon.h>
#include <Platform/Core/LObject.h>

namespace Lemonade
{
	class LEMONADE_API LService : public CitrusCore::AService
	{
		friend class Services;
	public:
		virtual ~LService() {}
	protected:
		virtual bool Init() = 0;
		virtual void Unload() = 0;
		virtual void Update() = 0;
		virtual void Render() = 0;
	};
}