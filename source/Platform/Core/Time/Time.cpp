
#include <LCommon.h>

#if defined(RENDERER_OPENGL) || defined(RENDERER_VULKAN)

#include <SDL2/SDL_timer.h>
#include <Platform/Core/Time/Time.h>

namespace Lemonade
{
	Time::Time()
	{
		SetName("Time");
	}

	// Time at beginning of frame, Wraps after 49days * timeScale
	double Time::GetTimeElapsedMS() const {
		return Time::m_timeElapsed;
	}

	double Time::GetTimeElapsed()
	{
		return m_time;
	}

	uint32 Time::GetTicks()
	{
		return SDL_GetTicks(); //!< Value return wraps after 49 days 
	}

	// Add the delta time to the time elapsed taking into account timescale
	void Time::Update()
	{
		double now = SDL_GetPerformanceCounter() * 1000.0l / SDL_GetPerformanceFrequency();
		Time::m_deltaTimeMS = ((now - Time::m_previousTimeElapsed) * Time::m_timeScale);
		Time::m_deltaTime = m_deltaTimeMS * 0.001l;
		Time::m_timeElapsed += Time::m_deltaTimeMS;
		Time::m_time = Time::m_timeElapsed * 0.001l;
		Time::m_previousTimeElapsed = now;
	}

	double Time::GetTimeSinceApplicationStarted()
	{
		return m_time - m_timeApplicationStarted;
	}

	bool Time::Init()
	{
		Update();
		m_timeApplicationStarted = m_time;
		return true;
	}
}
#endif
