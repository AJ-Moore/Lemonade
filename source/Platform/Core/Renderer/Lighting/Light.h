#pragma once

#include <LCommon.h>
#include <Types.h>
#include <Platform/Core/LObject.h>
#include <Platform/Core/Renderer/Lighting/LightingData.h>

namespace Lemonade
{
	enum class LEMONADE_API LightType : uint32
	{
		None = 0,
		Point = 1,
		Spot = 2,
		Direction = 3,
	};

	class LEMONADE_API Light : public LObject
	{
	public:
		Light(LightType type){ m_lightType = type; }	

		LightType GetLightType() const { return m_lightType; }

		/// Get the light intensity in lumens
		float GetLightIntensity() const { return m_lightIntensityLumens; }

		/// Get angle of cone (spot light)
		float GetConeAngle() { return m_coneAngle; }

		/// Get Max distance
		float GetMaxDistance() const { return m_lightingData.MaxDistance; }
	protected: 
		virtual bool Init() override { return true; }
		virtual void Unload() override {}
		virtual void Update() override {}
		virtual void Render() override {}
	private:
		LightingData m_lightingData;

		/// Spot / Direction / Point etc...
		LightType m_lightType = LightType::None;

		/// 10w LED/ 60W incandescent bulb = 800 lumens
		float m_lightIntensityLumens = 800;

		/// Cone angle 
		float m_coneAngle = 45.0f; 
	};

	class LEMONADE_API PointLight : public Light
	{
	public:
		PointLight() : Light(LightType::Point){}
	};

	class LEMONADE_API DirectionalLight : public Light
	{
	public:
		DirectionalLight() : Light(LightType::Direction){}
	};

	class LEMONADE_API SpotLight : public Light
	{
	public:
		SpotLight() : Light(LightType::Spot){}
	};
}