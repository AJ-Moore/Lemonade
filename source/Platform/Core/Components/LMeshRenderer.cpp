
#include "Platform/Core/Components/RenderComponent.h"
#include <Platform/Core/Renderer/RenderBlock/ARenderBlock.h>
#include <Platform/Core/Components/LMeshRenderer.h>

namespace Lemonade {

    bool LMeshRenderer::Init()
    {
        RenderComponent::Init();
    }

    void LMeshRenderer::Update()
    {
        RenderComponent::Update();
    }

    void LMeshRenderer::Render()
    {
        RenderComponent::Render();
    }

    void LMeshRenderer::Unload()
    {
        RenderComponent::Unload();
    }

    void LMeshRenderer::SetMaterial(ResourcePtr<Material> material)
	{		
		m_material = material;
		m_renderBlock->SetMaterial(material);
		m_renderBlock->SetDirty();
	}

    void LMeshRenderer::SetDrawMode(PrimitiveMode mode)
	{
		m_renderBlock->SetDrawMode(mode); 

		if (m_mesh != nullptr)
		{
			m_mesh->SetDrawMode(mode);
		}
	}
}