#include "Viewport.h"
#include <Core/Services/Services.h>
#include <Core/Renderer/Pipeline/Renderer.h>

namespace Lemonade
{
	bool Viewport::Init()
	{
		return false;
	}

	void Viewport::Unload()
	{
	}

	void Viewport::Update()
	{
	}

	void Viewport::Render()
	{
		Renderer* renderer = Services::GetRenderer();

		for (auto& camera : m_cameras)
		{
			if (renderer->IsShadowPass())
			{
				// Override viewport size
				glViewport(0, 0, renderer->GetShadowMapSize(), renderer->GetShadowMapSize());
				glScissor(0, 0, renderer->GetShadowMapSize(), renderer->GetShadowMapSize());
			}
			else
			{
				glViewport(0, 0, m_viewRect.Width, m_viewRect.Height);
				glScissor(0, 0, m_viewRect.Width, m_viewRect.Height);
			}
		}
	}
}
