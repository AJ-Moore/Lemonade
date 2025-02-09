#pragma once

#include <Common.h>
#include <vector>
#include <memory>

namespace CraftyBlocks
{
	class Bone {
	public:
		struct UVertexWeight
		{
			uint32 VertexIndex;
			float Weight;
		};

		Bone() = delete;
		Bone(std::string name, int boneId, int parentBoneId = -1) {
			m_name = name;
			m_boneId = boneId;
			m_parentBoneId = parentBoneId;
		}

		void SetOffsetMatrix(const glm::mat4& offsetMatrix) { m_offsetMatrix = offsetMatrix; }
		void SetWeights(std::shared_ptr<std::vector<UVertexWeight>> weights) { m_weights = weights; }
		int GetBoneId() const { return m_boneId; }
		int GetParentBoneId() const { return m_parentBoneId; }
		glm::mat4 GetOffsetMatrix() const { return m_offsetMatrix; }
	private:
		std::string m_name;
		int m_boneId;
		int m_parentBoneId = -1;
		glm::mat4 m_offsetMatrix;
		std::shared_ptr<std::vector<UVertexWeight>> m_weights;
	};
}