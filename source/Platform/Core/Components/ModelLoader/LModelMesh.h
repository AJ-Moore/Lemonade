#pragma once

#include "Platform/Core/LObject.h"
#include "Platform/Core/Renderer/Animation/Bone.h"
#include "Platform/Core/Renderer/Animation/BoneAnim.h"
#include <Platform/Core/Components/LMeshRenderer.h>
#include <Platform/Core/Components/RenderComponent.h>
#include <LCommon.h>
#include <Spatial/Transform.h>
#include <Platform/Core/Renderer/Geometry/Mesh.h>
#include <glm/fwd.hpp>
#include <memory>
#include <unordered_set>

namespace Lemonade
{
    class LEMONADE_API LModelMesh : public Lemonade::LMeshRenderer
    {
    public:
        LModelMesh() = delete;
        LModelMesh(std::string meshName, std::shared_ptr<CitrusCore::Transform>);
        void AddBone(std::shared_ptr<LBone> bone){ m_bones.insert(bone);}
        void AddBoneAnim(std::shared_ptr<LBoneAnim> anim) { m_boneAnimations.insert(anim);}
    private:
        std::unordered_set<std::shared_ptr<LBone>> m_bones;
        std::unordered_set<std::shared_ptr<LBoneAnim>> m_boneAnimations;
    };

    class LEMONADE_API LModelNode : LObject
    {
        friend class LModel;
    public:
        LModelNode() = delete;
        LModelNode(std::string meshName, std::shared_ptr<CitrusCore::Transform>);
    public:
        const std::unordered_set<std::shared_ptr<LModelNode>>& GetChildren() const { return m_children; }
        glm::mat4 GetTransform()const{ return m_nodeTransform; }
        void SetNodeTransform(glm::mat4 trans) { m_nodeTransform = trans; }
    private:

        virtual bool Init() override; 
        virtual void Update() override; 
        virtual void Render() override;
        virtual void Unload() override{}

        void AddChild(std::shared_ptr<LModelNode> child);

        void RemoveChild(std::shared_ptr<LModelNode> child);
        void SetTransform(std::shared_ptr<CitrusCore::Transform> transform){m_transform = transform;}

        virtual void AddMeshRenderer(std::shared_ptr<Lemonade::LMeshRenderer> mesh);

        std::unordered_set<std::shared_ptr<LModelNode>> m_children;
        std::unordered_set<std::shared_ptr<Lemonade::LMeshRenderer>> m_meshes;

        std::shared_ptr<CitrusCore::Transform> m_transform;
        glm::mat4 m_nodeTransform = glm::mat4(1.0f);
    };
}