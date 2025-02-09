#pragma once

#include <Renderer/Lighting/Light.h>

namespace CraftyBlocks
{
	class SpotLight : public Light {
	public:
		float GetApature() { return m_apature; }
	private:
		float m_apature = 45.0f; 
	};
}