#pragma once

#include <LCommon.h>
#include <Platform/Core/LObject.h>

namespace Lemonade
{
	class LEMONADE_API LService : public LObject
	{
		friend class Services;
	public:
		virtual ~LService() {}

		bool HasbeenInitialised() const { return m_bDoneInit; }
	protected:
		virtual bool Init() = 0;
		virtual void Unload() = 0;
		virtual void Update() = 0;
		virtual void Render() = 0;
	private: 
		bool m_bDoneInit = false;
	};
}