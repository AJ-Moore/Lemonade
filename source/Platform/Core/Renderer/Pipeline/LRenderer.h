#pragma once

#include "Events/Event.h"
#include "LCommon.h"
#include "Platform/Core/Renderer/Pipeline/ARenderTarget.h"
#include "Platform/Core/Renderer/Pipeline/LRenderStage.h"
#include <Platform/Core/Renderer/Pipeline/AViewport.h>
#include <Platform/Core/Services/LService.h>
#include <Platform/Core/Renderer/Pipeline/LCamera.h>
#include <Platform/Core/Renderer/Lighting/Light.h>
#include <memory>
#include <vector>

namespace Lemonade
{
	struct LEMONADE_API LRenderingData
	{

	};

	class LEMONADE_API LRenderer : public LService
	{
		friend class AWindow;
		friend class AViewport;
	public: 
		LCamera* GetActiveCamera() const { return m_activeCamera; }
		bool IsShadowPass() { return m_bIsShadowPass; }
		Light* GetActiveLight() const { return m_activeLight; }
		ARenderTarget* GetActiveRenderTarget() { return m_activeRenderTarget; }
		float GetShadowMapSize() const { return m_shadowMapSize; }

		virtual void RenderScene(); 
		CitrusCore::Event<const LRenderingData&> OnRenderScene;
	protected:
		virtual bool Init();
		virtual void Unload();
		virtual void Update();
		virtual void Render();


		void SetActiveCamera(LCamera* camera) {m_activeCamera = camera;}
	private:
		LRenderingData m_renderingData;
		std::vector<LRenderStage> m_renderStages;

		float m_shadowMapSize = 512.0f;
		LCamera* m_activeCamera;
		bool m_bIsShadowPass = false;
		Light* m_activeLight = nullptr;
		ARenderTarget* m_activeRenderTarget = nullptr;
	};
}