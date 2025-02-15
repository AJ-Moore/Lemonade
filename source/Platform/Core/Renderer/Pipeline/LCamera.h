#pragma once

#include <Core/LObject.h>

namespace Lemonade
{
	class LCamera : public LObject
	{
	public:
		const glm::mat4& GetViewMatrix() const { return m_viewMat; }
		const glm::mat4& GetProjMatrix() const { return m_projMat; }
		const glm::mat4& GetViewProjMatrix() const { return m_viewProjMatrix; }
	private:
		glm::mat4 m_viewMat;
		glm::mat4 m_projMat;
		glm::mat4 m_viewProjMatrix;
		glm::mat4 m_orthographic;
	};
}