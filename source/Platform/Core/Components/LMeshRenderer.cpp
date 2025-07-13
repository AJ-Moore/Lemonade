
#include "Platform/Core/Components/RenderComponent.h"
#include "Platform/Core/Renderer/Geometry/Mesh.h"
#include <Platform/Core/Renderer/RenderBlock/ARenderBlock.h>
#include <Platform/Core/Components/LMeshRenderer.h>
#include <memory>

namespace Lemonade {

    bool LMeshRenderer::Init()
    {
        return RenderComponent::Init();
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

    void LMeshRenderer::SetMesh(std::shared_ptr<Mesh> mesh)
    {
        m_mesh = mesh;
        m_renderBlock->SetMesh(mesh);
        m_renderBlock->SetDirty();
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

    void LMeshRenderer::setDirty()
	{
		m_renderBlock->SetDirty();
	}
}