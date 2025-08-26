#include <Platform/Core/Renderer/Animation/BoneAnim.h>

Lemonade::LBoneAnim::LBoneAnim(std::string name, int boneId, int parentBoneId)
{
	m_boneName = name;
	m_boneId = boneId;
	m_parentBoneId = parentBoneId;
}
