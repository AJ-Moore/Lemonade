#include <Util/Logger.h>
#include <Platform/Core/Components/ModelLoader/LModelResource.h>
#include <Platform/Core/Services/GraphicsServices.h>
#include <Platform/Core/Renderer/Geometry/Mesh.h>
#include <Resources/ResourceHandle.h>
#include <Platform/Core/Components/ModelLoader/LModel.h>

namespace Lemonade {

    LModel::LModel(std::string path)
    {
        m_path = path;
    }

    void LModel::PlayAnimation(std::string animationName)
    {
        if (m_model == nullptr)
        {
            return;
        }

        if (!m_animationData.contains(animationName))
        {
            CitrusCore::Logger::Log(CitrusCore::Logger::ERROR, "Unable to play animation [%s]", animationName.c_str());
            return;
        }

        auto anim = m_animations.Get(m_animationData.at(animationName).AnimationId);
        uint32 index = m_animationData.at(animationName).AnimationIndex;
        m_model->GetResource()->PlayAnimation(anim->GetResource()->GetAnimation(index));
    }

    CitrusCore::UID LModel::LoadAnimation(const std::string& modelPath, const std::string& animationName, uint32 animationIndex)
    {
        CitrusCore::ResourcePtr<LModelResource> model = Lemonade::GraphicsServices::GetGraphicsResources()->GetModelHandle(modelPath);
        model->GetResource()->Init();
        CitrusCore::Transform::MakeParent(model->GetResource()->GetRoot()->GetTransform(),m_transform);
        m_animations.Add(model->GetResource()->GetUID(), model);
        m_animationData[animationName] = {
            .AnimationId = model->GetResource()->GetUID(),
            .AnimationIndex = animationIndex
        };
        return model->GetResource()->GetUID();
    }

    bool LModel::Init()
    {
        m_model = Lemonade::GraphicsServices::GetGraphicsResources()->GetModelHandle(m_path);
        m_model->GetResource()->Init();
        CitrusCore::Transform::MakeParent(m_model->GetResource()->GetRoot()->GetTransform(),m_transform);
        return true;
    }

    void LModel::Update()
    {
        m_model->GetResource()->Update();
    }

    void LModel::Render() 
    {
        m_model->GetResource()->Render();
    }
}