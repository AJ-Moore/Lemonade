#pragma once

#include <Common.h>
#include <vector>

namespace CraftyBlocks
{
	enum class AnimBehaviour
	{
		Default,
		Constant,
		Linear,
		Repeat,
	};

	struct VectorKey
	{
		VectorKey() = default;
		VectorKey(float time, glm::vec3 value) {
			Time = time;
			Value = glm::vec4(value.x, value.y, value.z, 1);
		}

		// WARNING: Order must be maintained/ match GLSL code - best left alone.
		alignas(16) glm::vec4 Value = glm::vec4(0, 0, 0, 1);
		float Time = 0;
	};

	struct QuatKey
	{
		QuatKey() = default;
		QuatKey(float time, glm::vec4 value) { Time = time; Value = value; }

		// WARNING: Order must be maintained/ match GLSL code - best left alone.
		alignas(4) glm::vec4 Value = glm::vec4(0, 0, 0, 0);
		float Time = 0;
	};

	class BoneAnim
	{
	public:
		BoneAnim(std::string name, int boneId, int parentBoneId = -1);

		void AddPositionkey(VectorKey key) { m_positionKeys.push_back(key); }
		void AddScalingkey(VectorKey key) { m_scalingKeys.push_back(key); }
		void AddRotationkey(QuatKey key) { m_rotationKeys.push_back(key); }
		void SetBoneOrigin(glm::vec3 origin) { m_boneOrigin = origin; }

		const std::vector<VectorKey>& GetPositionKeys() { return m_positionKeys; }
		const std::vector<QuatKey>& GetRotationKeys() { return m_rotationKeys; }
		const std::vector<VectorKey>& GetScaleKeys() { return m_scalingKeys; }

		std::string GetName() { return m_boneName; }
		int GetId() { return m_boneId; }
		int GetParentId() { return m_parentBoneId; }
		glm::vec3 GetBoneOrigin() { return m_boneOrigin; }
	private:
		// Name of bone animation applies to?
		std::string m_boneName;
		int m_boneId;
		int m_parentBoneId;
		AnimBehaviour m_postState = AnimBehaviour::Default;
		AnimBehaviour m_preState = AnimBehaviour::Default;

		glm::vec3 m_boneOrigin;
		std::vector<VectorKey> m_positionKeys;
		std::vector<QuatKey> m_rotationKeys;
		std::vector<VectorKey> m_scalingKeys;
	};
}