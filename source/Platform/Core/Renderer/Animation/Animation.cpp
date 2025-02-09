#include "Animation.h"

CraftyBlocks::Animation::Animation(std::string name, double duration, double ticksPerSecond)
{
	m_name = name;
	m_duration = duration;
	m_ticksPerSecond = ticksPerSecond;
}
