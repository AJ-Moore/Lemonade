#pragma once

#include <LCommon.h>
#include <unordered_map>
#include <vector>
#include <memory>

namespace Lemonade
{
	class LBone {
	public:
		struct UVertexWeight
		{
			uint32 VertexIndex;
			float Weight;
		};

		LBone() = delete;
		LBone(std::string name, int boneId, int parentBoneId = -1) {
			m_name = name;
			m_boneId = boneId;
			m_parentBoneId = parentBoneId;
		}

		void SetOffsetMatrix(const glm::mat4& offsetMatrix) { m_offsetMatrix = offsetMatrix; }
		void SetBoneMatrix(const glm::mat4& boneMat) { m_boneMatrix = boneMat; }
		void SetWeights(std::shared_ptr<std::vector<UVertexWeight>> weights) { m_weights = weights; }
		int GetBoneId() const { return m_boneId; }
		int GetParentBoneId() const { return m_parentBoneId; }
		glm::mat4 GetOffsetMatrix() const { return m_boneMatrix; }
		glm::mat4 GetBoneMatrix() const { return m_offsetMatrix; }
		void AddChild(std::shared_ptr<LBone>);
		void RemoveChild(int id);

		const std::unordered_map<int, std::shared_ptr<LBone>>& GetChildren() const { return m_children; }
	private:
		std::unordered_map<int, std::shared_ptr<LBone>> m_children;
		std::string m_name;
		int m_boneId;
		int m_parentBoneId = -1;
		glm::mat4 m_offsetMatrix;
		glm::mat4 m_boneMatrix;

		/// Storing weights here might be redundant though it includes vertex index (thinky face)
		/// In the future we might want to manipulate vertices affected by bones and weights of said vertices
		std::shared_ptr<std::vector<UVertexWeight>> m_weights;
	};
}