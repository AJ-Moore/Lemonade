#include "AViewport.h"
#include <Platform/Core/Renderer/Pipeline/LCamera.h>
#include <Platform/Core/Services/Services.h>
#include <Platform/Core/Renderer/Pipeline/LRenderer.h>
#include <memory>

namespace Lemonade
{
	bool AViewport::Init()
	{
		return false;
	}

	void AViewport::Unload()
	{
	}

	void AViewport::Update()
	{
	}

	void AViewport::Render()
	{
		std::shared_ptr<Renderer> renderer = Services::GetRenderer();

		for (auto& camera : m_cameras)
		{
			renderer->SetActiveCamera(camera);

			if (renderer->IsShadowPass())
			{
				// Override viewport size
				SetViewport(0, 0, renderer->GetShadowMapSize(), renderer->GetShadowMapSize());
				SetScissor(0, 0, renderer->GetShadowMapSize(), renderer->GetShadowMapSize());
			}
			else
			{
				glViewport(0, 0, m_viewRect.Width, m_viewRect.Height);
				glScissor(0, 0, m_viewRect.Width, m_viewRect.Height);
			}
		}
	}
}
