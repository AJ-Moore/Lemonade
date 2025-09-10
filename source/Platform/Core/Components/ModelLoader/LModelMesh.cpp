#include <Platform/Core/Components/LMeshRenderer.h>
#include <Spatial/Transform.h>
#include <Platform/Core/Components/ModelLoader/LModelMesh.h>

namespace Lemonade
{
    LModelMesh::LModelMesh(std::string meshName, std::shared_ptr<CitrusCore::Transform> transform)
    {
        SetName(meshName);
        SetTransform(transform);
    }


    bool LModelMesh::Init()
    {
        if (m_mesh != nullptr)
            LMeshRenderer::Init();

        for (auto& child : m_children)
        {
            child->Init();
        }
        return true;
    }

    void LModelMesh::Update()
    {
        if (m_mesh != nullptr)
            LMeshRenderer::Update();

        for (auto& child : m_children)
        {
            child->Update();
        }
    }

    void LModelMesh::Render()
    {
        if (m_mesh != nullptr)
            LMeshRenderer::Render();

        for (auto& child : m_children)
        {
            child->Render();
        }
    }

    void LModelMesh::SetMesh(std::shared_ptr<Mesh> mesh)
    {
        LMeshRenderer::SetMesh(mesh);
    }

    void LModelMesh::AddChild(std::shared_ptr<LModelMesh> child)
    {
        m_children.insert(child);
        CitrusCore::Transform::MakeParent(child->m_transform,m_transform);
    }

    void LModelMesh::RemoveChild(std::shared_ptr<LModelMesh> child)
    {
        m_children.erase(child);
        m_transform->RemoveChild(child->m_transform.get());
    }
}