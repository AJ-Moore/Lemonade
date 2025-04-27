#pragma once

#include <Platform/Core/Services/LService.h>
#include <LCommon.h>
#include <Platform/Core/LObject.h>

namespace Lemonade
{
	class LEMONADE_API AGraphicsContext : public LService
	{
	public:
		virtual ~AGraphicsContext() {}
	protected:
		virtual bool Init() = 0;
		virtual void Unload() = 0;
		virtual void Update() = 0;
		virtual void Render() = 0;
	};
}