#include <Platform/Core/Renderer/Animation/Bone.h>

namespace Lemonade 
{
    void LBone::AddChild(std::shared_ptr<LBone> bone)
    {
        m_children[bone->GetBoneId()] = bone;
    }

    void LBone::RemoveChild(int id)
    {
        m_children.erase(id);
    }
}