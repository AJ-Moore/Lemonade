#include <Platform/Core/Components/LMeshRenderer.h>
#include <Spatial/Transform.h>
#include <Platform/Core/Components/ModelLoader/LModelMesh.h>
#include <memory>

namespace Lemonade
{
    LModelMesh::LModelMesh(std::string meshName, std::shared_ptr<CitrusCore::Transform> transform)
    {
        SetName(meshName);
        SetTransform(transform);
    }

    LModelNode::LModelNode(std::string meshName, std::shared_ptr<CitrusCore::Transform> transform)
    {
        SetName(meshName);
        SetTransform(transform);
    }

    void LModelNode::AddMeshRenderer(std::shared_ptr<Lemonade::LMeshRenderer> mesh)
    {
        mesh->SetTransform(m_transform);
        m_meshes.insert(mesh);
    }

    bool LModelNode::Init()
    {
        for (auto& mr : m_meshes)
        {
            mr->Init();
        }

        for (auto& child : m_children)
        {
            child->Init();
        }
        return true;
    }

    void LModelNode::Update()
    {
        for (auto& mr : m_meshes)
        {
            mr->Update();
        }
        for (auto& child : m_children)
        {
            child->Update();
        }
    }

    void LModelNode::Render()
    {
        for (auto& mr : m_meshes)
        {
            mr->Render();
        }
        for (auto& child : m_children)
        {
            child->Render();
        }
    }

    void LModelNode::AddChild(std::shared_ptr<LModelNode> child)
    {
        m_children.insert(child);
        CitrusCore::Transform::MakeParent(child->m_transform,m_transform);
    }

    void LModelNode::RemoveChild(std::shared_ptr<LModelNode> child)
    {
        m_children.erase(child);
        m_transform->RemoveChild(child->m_transform.get());
    }
}