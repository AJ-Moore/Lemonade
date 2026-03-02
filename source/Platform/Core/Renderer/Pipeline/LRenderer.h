#pragma once

#include <Platform/Vulkan/WindowManager/LWindow.h>
#include <Events/Event.h>
#include <LCommon.h>
#include <Platform/Core/Renderer/Pipeline/ARenderTarget.h>
#include <Platform/Core/Renderer/Pipeline/LRenderInput.h>
#include <Platform/Core/Renderer/Pipeline/AViewport.h>
#include <Platform/Core/Services/LService.h>
#include <Platform/Core/Renderer/Pipeline/LCamera.h>
#include <Platform/Core/Renderer/Lighting/Light.h>
#include <glm/fwd.hpp>
#include <memory>
#include <vector>

namespace Lemonade
{
	struct LEMONADE_API LRenderingData
	{
		LCamera* ActiveCamera;
		LWindow* ActiveWindow;
		LRenderInput* RenderInput;
	};

	struct LEMONADE_API LShadowData
	{
		glm::mat4 View; 
		glm::mat4 Projection;
	};

	class LRenderStage;

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
		const LRenderInput& GetRenderInput () const { return *m_renderInput;}

		virtual void RenderScene(); 
		virtual void PrepareScene(); 
		virtual void RenderSwapChain();

		CitrusCore::Event<const LRenderingData&> OnRenderScene;

		/// Set render input during this callback.
		CitrusCore::Event<const LRenderingData&> OnPrepareScene;

		/// Best place to hook in things like ImGUI
		CitrusCore::Event<const LRenderingData&> OnRenderSwapChain;

		void SetActiveCamera(LCamera* camera) {m_activeCamera = camera;}
		void SetActiveRenderTarget(ARenderTarget* target) { m_activeRenderTarget = target; }
		void SetRenderInput(LRenderInput* input) { m_renderInput = input; }
		void SetShadowPass(bool value) { m_bIsShadowPass = value; }
		void SetShadowData(const LShadowData& data) { m_shadowPassData = data; }
		const LShadowData& GetShadowdata() { return m_shadowPassData; }
		void SetViewport(AViewport* viewport) { m_viewport = viewport; }

		AViewport* GetViewport(){ return m_viewport; }
	protected:
		virtual bool Init();
		virtual void Unload();
		virtual void Update();
		virtual void Render();
		void RenderPass();

	private:
		LShadowData m_shadowPassData;
		LRenderInput* m_renderInput;
		LRenderingData m_renderingData;
		std::vector<std::shared_ptr<LRenderStage>> m_renderStages;

		float m_shadowMapSize = 128.0f;
		LCamera* m_activeCamera;
		bool m_bIsShadowPass = false;
		Light* m_activeLight = nullptr;
		ARenderTarget* m_activeRenderTarget = nullptr;
		AViewport* m_viewport;
	};
}