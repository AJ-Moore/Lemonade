#include "Animation.h"

Lemonade::LAnimation::LAnimation(std::string name, double duration, double ticksPerSecond)
{
	m_name = name;
	m_duration = duration;
	m_ticksPerSecond = ticksPerSecond;
}
