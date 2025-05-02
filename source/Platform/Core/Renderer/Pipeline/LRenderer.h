#pragma once

#include <Platform/Core/Renderer/Pipeline/AViewport.h>
#include <Platform/Core/Services/LService.h>
#include <Platform/Core/Renderer/Pipeline/LCamera.h>
#include <Platform/Core/Renderer/Lighting/Light.h>
#include <memory>

namespace Lemonade
{
	class LEMONADE_API LRenderer : public LService
	{
		friend class AWindow;
		friend class AViewport;
	public: 
		LCamera* GetActiveCamera() const { return m_activeCamera; }
		bool IsShadowPass() { return m_bIsShadowPass; }
		Light* GetActiveLight() const { return m_activeLight; }
		float GetShadowMapSize() const { return m_shadowMapSize; }
	protected:
		virtual bool Init();
		virtual void Unload();
		virtual void Update();
		virtual void Render();

		void SetActiveCamera(LCamera* camera) {m_activeCamera = camera;}
	private:
		float m_shadowMapSize = 512.0f;
		LCamera* m_activeCamera;
		bool m_bIsShadowPass = false;
		Light* m_activeLight = nullptr;
	};
}