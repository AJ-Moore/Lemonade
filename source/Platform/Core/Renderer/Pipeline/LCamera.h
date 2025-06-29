#pragma once

#include "Spatial/Transform.h"
#include <Platform/Core/LObject.h>
#include <glm/fwd.hpp>

namespace Lemonade
{
	class LCamera : public LObject
	{
		friend class AViewport;
	public:
		LCamera() = delete; 
		LCamera( CitrusCore::Transform* transform) : m_transform(transform){}
		const glm::mat4& GetViewMatrix() const { return m_viewMat; }
		const glm::mat4& GetProjMatrix() const { return m_projMat; }
		const glm::mat4& GetViewProjMatrix() const { return m_viewProjMatrix; }

		CitrusCore::Transform* GetTransfrom() { return m_transform; }
		void SetTransform(CitrusCore::Transform* transform) { m_transform = transform; }
	protected:
		virtual bool Init()override{ return true; }
		virtual void Unload()override{}
		virtual void Update()override{}
		virtual void Render()override{}
	private:
		void SetOrthographicDimensions(glm::vec4 rect);

		// TODO camera transform lifecycle non ownership is a bit hairy
		CitrusCore::Transform* m_transform = nullptr;

		glm::mat4 m_viewMat;
		glm::mat4 m_projMat;
		glm::mat4 m_viewProjMatrix;
		glm::mat4 m_orthographic;
		bool m_orthographicMode = false;

		float m_nearClip = 0.1f;
		float m_farClip = 1000.0f;
		float m_fov = 80.0f;

		void CalculateProjMatrix(float Width, float height);
		void CalculateViewMatrix(); 
		void CalculateViewProjMatrix(); 
	};
}