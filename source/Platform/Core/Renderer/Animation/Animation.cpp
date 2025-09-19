#include "Animation.h"

Lemonade::LAnimation::LAnimation(std::string name, double duration, double ticksPerSecond)
{
	m_name = name;
	m_duration = duration;
	m_ticksPerSecond = ticksPerSecond;
}

double Lemonade::LAnimation::GetAnimationTime(float timeElapsedSeconds)
{
	return fmod(timeElapsedSeconds * m_ticksPerSecond, m_duration);
}
