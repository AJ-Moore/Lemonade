
#include "Spatial/Transform.h"
#include <Platform/Core/Renderer/Pipeline/AViewport.h>
#include <Platform/Core/Renderer/Pipeline/LCamera.h>
#include <Platform/Core/Services/GraphicsServices.h>
#include <Platform/Core/Renderer/Pipeline/LRenderer.h>
#include <memory>

namespace Lemonade
{
	bool AViewport::Init()
	{
		// TODO remove Test
		m_cameras.push_back(std::make_shared<LCamera>(std::make_shared<CitrusCore::Transform>()));

		return true;
	}

	std::shared_ptr<LCamera> AViewport::GetCameraAtIndex(int index)
	{
		if (m_cameras.size() <= index)
		{
			return nullptr;
		}

		return m_cameras[index];
	}

	void AViewport::Unload()
	{
	}

	void AViewport::Update()
	{
	}

	void AViewport::Render()
	{
		std::shared_ptr<LRenderer> renderer = GraphicsServices::GetRenderer();

		for (auto& camera : m_cameras)
		{
			renderer->SetActiveCamera(camera.get());

			if (renderer->IsShadowPass())
			{
				// Override viewport size
				SetViewport(0, 0, renderer->GetShadowMapSize(), renderer->GetShadowMapSize());
				SetScissor(0, 0, renderer->GetShadowMapSize(), renderer->GetShadowMapSize());
			}
			else
			{
				SetViewport(0, 0, m_viewRect.Width, m_viewRect.Height);
				SetScissor(0, 0, m_viewRect.Width, m_viewRect.Height);
			}

			camera->CalculateProjMatrix((float)m_viewRect.Width, (float)m_viewRect.Height);
			camera->CalculateViewMatrix();
			camera->CalculateViewProjMatrix();

			// Render pass goes here!!! 
			//m_viewCamera->getParent()->m_parentScene->render();
			renderer->RenderPass();
		}
	}
}
