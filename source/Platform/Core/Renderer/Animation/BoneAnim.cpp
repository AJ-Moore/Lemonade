#include <Renderer/BoneAnim.h>

CraftyBlocks::BoneAnim::BoneAnim(std::string name, int boneId, int parentBoneId)
{
	m_boneName = name;
	m_boneId = boneId;
	m_parentBoneId = parentBoneId;
}
