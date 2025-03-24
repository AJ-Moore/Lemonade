#pragma once

#include <LCommon.h>
#include <platform/core/Time/Time.h>

namespace Lemonade
{
	class LEMONADE_API Services
	{
	public:
		Time* GetTimeService() { return m_timeService; }

	protected:
		virtual void Init();
		virtual void Update();
		virtual void Render();

	private:
		bool m_bRunning = false;
		Time* m_timeService;
	};
}