#pragma once

#include <Core/Component.h>

namespace CraftyBlocks
{
	enum class LightType : uint
	{
		None = 0,
		Point = 1,
		Spot = 2,
		Direction = 3,
	};

	class Light : public Component
	{
	public:
		LightType GetLightType() const { return m_lightType; }
	private:
		LightType m_lightType = LightType::None;
	};
}