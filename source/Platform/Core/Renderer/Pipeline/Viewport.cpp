#include "Viewport.h"
#include <Core/Services/Services.h>

bool CraftyBlocks::Viewport::Init()
{
	return false;
}

void CraftyBlocks::Viewport::Unload()
{
}

void CraftyBlocks::Viewport::Update()
{
}

void CraftyBlocks::Viewport::Render()
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
