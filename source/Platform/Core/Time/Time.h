#pragma once

#include <LCommon.h>
#include <Platform/Core/Services/LService.h>

namespace Lemonade
{
	/** Time class - for time keeping. */
	class LEMONADE_API Time : public LService {
	public:
		Time();

		/// Return the delta time
		double GetDeltaTime() const { return m_deltaTime; }

		/// Get the current time elapsed in milliseconds (Affected by timeScale)
		/// Wraps after 49days * timeScale
		double GetTimeElapsedMS() const;

		/// Get the current time elapsed in seconds (Affected by timeScale)
		double GetTimeElapsed();

		/// Get the time in seconds since the application started
		double GetTimeSinceApplicationStarted();

		/// Returns the time in milliseconds since the program was started
		uint32 GetTicks();

		virtual bool Init() override;

		/// Called by the game once per frame
		virtual void Update() override;

		void SetTimeScale(double scale) { m_timeScale = scale; }

	private:
		/// The time passed (in milliseconds) since the last update. 
		double m_deltaTimeMS = 0;

		/// The time passed (in seconds) since the last update.
		double m_deltaTime = 0;

		/// Multiplier that effects the delta time. 
		double m_timeScale = 1.0l;

		/// Previous number of ticks since program started(private)
		double m_previousTimeElapsed = 0;

		/// The time elapsed - affected by timescale
		double m_timeElapsed = 0;

		/// The time elapsed - affected by timescale in seconds
		double m_time = 0;

		/// Time application started
		double m_timeApplicationStarted = 0;
	};
}
