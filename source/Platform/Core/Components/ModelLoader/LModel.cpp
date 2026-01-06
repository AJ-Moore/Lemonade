
#include "Platform/Core/Services/GraphicsServices.h"
#include <Platform/Core/Renderer/Geometry/Mesh.h>
#include <Resources/ResourceHandle.h>
#include <Platform/Core/Components/ModelLoader/LModel.h>

namespace Lemonade {

    LModel::LModel(std::string path)
    {
        m_path = path;
    }

    bool LModel::Init()
    {
        m_model = Lemonade::GraphicsServices::GetGraphicsResources()->GetModelHandle(m_path);
        m_model->GetResource()->Init();
        CitrusCore::Transform::MakeParent(m_model->GetResource()->GetRoot()->GetTransform(),m_transform);
        return true;
    }
}