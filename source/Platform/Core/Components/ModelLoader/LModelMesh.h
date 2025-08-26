#pragma once

#include <Platform/Core/Components/LMeshRenderer.h>
#include <Platform/Core/Components/RenderComponent.h>
#include <LCommon.h>
#include <Spatial/Transform.h>
#include <Platform/Core/Renderer/Geometry/Mesh.h>
#include <memory>
#include <unordered_set>
#include <vector>

namespace Lemonade
{
    class LEMONADE_API LModelMesh : Lemonade::LMeshRenderer
    {
        friend class LModel;
    public:
        LModelMesh() = delete;
        LModelMesh(std::string meshName, std::shared_ptr<CitrusCore::Transform>);
    public:
        const std::unordered_set<std::shared_ptr<LModelMesh>>& GetChildren() {return m_children;}
    private:

        virtual bool Init() override; 
        virtual void Update() override; 
        virtual void Render() override;

        void AddChild(std::shared_ptr<LModelMesh> child);
        void RemoveChild(std::shared_ptr<LModelMesh> child);

        // We override to try and restrict write access to mesh data on model.
        virtual void SetMesh(std::shared_ptr<Mesh>) override final;

        std::unordered_set<std::shared_ptr<LModelMesh>> m_children;
    };
}