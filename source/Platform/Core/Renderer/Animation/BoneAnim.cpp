#include "Util/Math.h"
#include <Platform/Core/Renderer/Animation/BoneAnim.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>

namespace Lemonade 
{
	LBoneAnim::LBoneAnim(std::string name, int boneId, int parentBoneId)
	{
		m_boneName = name;
		m_boneId = boneId;
		m_parentBoneId = parentBoneId;
	}

	glm::vec3 LBoneAnim::GetBonePositionForAnimTime(float time)
	{
		if (m_positionKeys.empty()) return glm::vec3(0.0f);
		if (m_positionKeys.size() == 1) return glm::vec3(m_positionKeys[0].Value);

		for (int i = 0; i < m_positionKeys.size() -1; ++i)
		{
			if (time > m_positionKeys[i + 1].Time){
				continue;
			}

			float t = CitrusCore::Math::InverseLerp(m_positionKeys[i].Time, m_positionKeys[i + 1].Time, time);
			return glm::mix(m_positionKeys[i].Value, m_positionKeys[i + 1].Value,t);
		}

		return m_positionKeys.back().Value;
	}

	glm::quat LBoneAnim::GetBoneRotationForAnimTime(float time)
	{
		if (m_rotationKeys.empty()) return glm::quat(1, 0, 0, 0);
		if (m_rotationKeys.size() == 1) return glm::quat(m_rotationKeys[0].Value);

		for (int i = 0; i < m_rotationKeys.size() -1; ++i)
		{
			if (time > m_rotationKeys[i + 1].Time){
				continue;
			}

			float t = CitrusCore::Math::InverseLerp(m_rotationKeys[i].Time, m_rotationKeys[i + 1].Time, time);
			return glm::normalize(glm::slerp(m_rotationKeys[i].Value, m_rotationKeys[i + 1].Value, t));
		}

		return glm::quat(m_rotationKeys.back().Value);
	}

	glm::vec3 LBoneAnim::GetBoneScaleForAnimTime(float time)
	{
		if (m_scalingKeys.empty()) return glm::vec3(1.0f);
		if (m_scalingKeys.size() == 1) return glm::vec3(m_scalingKeys[0].Value);

		for (int i = 0; i < m_scalingKeys.size() -1; ++i)
		{
			if (time > m_scalingKeys[i + 1].Time){
				continue;
			}

			float t = CitrusCore::Math::InverseLerp(m_scalingKeys[i].Time, m_scalingKeys[i + 1].Time, time);
			return glm::mix(m_scalingKeys[i].Value, m_scalingKeys[i + 1].Value,t);
		}

		return m_scalingKeys.back().Value;
	}

	glm::mat4 LBoneAnim::GetBoneMatrixForAnimTime(float time)
	{
		glm::vec3 pos = GetBonePositionForAnimTime(time);
		glm::quat rotation = GetBoneRotationForAnimTime(time);
		glm::vec3 scale = GetBoneScaleForAnimTime(time);

		return glm::translate(glm::mat4(1),pos) * 
				glm::mat4_cast(rotation) *
				glm::scale(glm::mat4(1.0f), scale);
	}
}