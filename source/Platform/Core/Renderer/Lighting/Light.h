#pragma once

#include <Types.h>
#include <Platform/Core/LObject.h>

namespace Lemonade
{
	enum class LightType : uint32
	{
		None = 0,
		Point = 1,
		Spot = 2,
		Direction = 3,
	};

	class Light : public LObject
	{
	public:
		LightType GetLightType() const { return m_lightType; }
	private:
		LightType m_lightType = LightType::None;
	};
}