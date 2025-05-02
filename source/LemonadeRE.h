#pragma once
#include <LCommon.h>

namespace Lemonade
{
	class LEMONADE_API LemonadeRE
	{
	public:
		void Init();
		void Update();
		void Unload(); 

		bool m_bRunning = false;
	};
}