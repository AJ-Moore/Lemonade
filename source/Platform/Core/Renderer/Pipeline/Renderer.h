#pragma once

#include <Core/Services/LService.h>
#include <Core/Renderer/Camera.h>
#include <Core/Renderer/Lighting/Light.h>

namespace Lemonade
{
	class Renderer : public LService
	{
	public: 
		std::shared_ptr<Camera> GetActiveCamera() const { return m_camera; }
		bool IsShadowPass() { return m_bIsShadowPass; }
		Light* GetActiveLight() const { return m_activeLight; }
		float GetShadowMapSize() const { return m_shadowMapSize; }
	protected:
		virtual bool Init();
		virtual void Unload();
		virtual void Update();
		virtual void Render();
	private:
		float m_shadowMapSize = 512.0f;
		std::shared_ptr<Camera> m_camera;
		bool m_bIsShadowPass = false;
		Light* m_activeLight = nullptr;
	};
}