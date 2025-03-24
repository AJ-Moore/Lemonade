#pragma once
#include <LCommon.h>

namespace Lemonade
{
	class LEMONADE_API Lemonade
	{
	public:
		void Init();

	private:

		void Update();
		void Render();

		bool m_bRunning = false;
	};
}