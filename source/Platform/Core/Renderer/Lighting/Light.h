#pragma once

#include <LCommon.h>
#include <Types.h>
#include <Platform/Core/LObject.h>
#include <Platform/Core/Renderer/Lighting/LightingData.h>
#include <glm/fwd.hpp>

namespace Lemonade
{
	enum class LEMONADE_API LightType : uint32
	{
		None = 0,
		Point = 1,
		Spot = 2,
		Direction = 3,
	};

	enum class LEMONADE_API LightDirection : uint32
	{
		Forward = 0, 
		Back = 1,
		Left = 2,
		Right = 3,
		Up = 4,
		Down = 5,
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

		void SetPosition(glm::vec3 position) { m_lightingData.LightPosition = position; }
		void SetUp(glm::vec3 up) { m_lightingData.LightUp = up; }
		void SetDirection(glm::vec3 direction) { m_lightingData.LightDirection = direction; }
		void SetMaxDistance(float distance) {m_lightingData.MaxDistance = distance; }
		void SetLightIntensity(float lumens) {m_lightIntensityLumens = lumens;} 
		void SetColour(glm::vec3 colour) {m_lightingData.Colour = colour;} 
		void SetView(glm::mat4 view, LightDirection direction = LightDirection::Forward) { m_view[(uint32)direction] = view; }
		const glm::mat4& GetView(LightDirection direction = LightDirection::Forward) const { return m_view[(uint32)direction]; }
		void SetWorld(glm::mat4 world, LightDirection direction) { m_lightingData.WorldMatrix[(uint32)direction] = world; }
		void SetProjection(glm::mat4 projection) { m_projection = projection; }
		const glm::mat4& GetProjection() const { return m_projection; }

		void Dump();
		const LightingData& GetLightData() const { return m_lightingData; }
	protected: 
		virtual bool Init() override { return true; }
		virtual void Unload() override {}
		virtual void Update() override {}
		virtual void Render() override {}
	private:

		// View Matrix 
		glm::mat4 m_view[6]; 

		// Projection Matrix
		glm::mat4 m_projection;

		// Raw lighting data to be copied to GPU
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