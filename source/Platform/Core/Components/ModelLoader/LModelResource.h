#pragma once

#include <Platform/Core/Renderer/Animation/Animation.h>
#include <Platform/Core/Renderer/Animation/Bone.h>
#include <assimp/Importer.hpp>
#include <LCommon.h>
#include <Platform/Core/Components/ModelLoader/LModelMesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/fwd.hpp>
#include <memory>
#include <unordered_map>
#include <string>

namespace Lemonade
{
    struct LEMONADE_API LModelData
    {
        std::unique_ptr<Assimp::Importer> Importer;
        bool Persist = false;
    };

    class LEMONADE_API LModelMeta
    {
        friend class LModelResource;
    private:
        // Key:mesh, value:material 
        std::unordered_map<std::string, std::string> m_materials;
    };

	// Represents a model in the game - wraps Assimp aiScene 
	class LEMONADE_API LModelResource : public CitrusCore::AResource<LModelResource>
	{
		friend class LModel;
	public:
        std::shared_ptr<LModelNode> GetRoot() { return m_root; }

        virtual void Init(); 
        virtual void Update(); 
        virtual void Render();
    protected:
		virtual bool LoadResource(std::string path) override;
		virtual void UnloadResource() override{};

		void PlayAnimation(uint32 animationIndex);
		void PlayAnimation(LAnimation* animation);

		LAnimation* GetAnimation(uint32 index);

	private:
	    std::unordered_map<int, std::shared_ptr<LBone>> m_skeleton;
        std::shared_ptr<LModelNode> m_root;
		std::shared_ptr<LModelData> m_modelData;
		uint m_customFlags = 0;
		std::string m_filePath;
		bool m_bGenerateCollider = false;
		bool m_bAnimationPlaying = false;

		// Ignore root motion will not apply the root bone transform to the model transform.
		// this is useful as it allows us to apply the root motion ourselves via the root motion delta.
		bool m_bIgnoreRootMotion = true;
		float m_previousRootMotionTime = 0.0f;
		glm::vec3 m_previousRootBonePosition = glm::vec3(0,0,0);
		glm::vec3 m_rootMotionDelta = glm::vec3(0,0,0);
		glm::quat m_rootMotionRotation = glm::quat(1,0,0,0);

		LModelMeta m_meta;

		void LoadMeta(){}
		void SaveMeta(){}
        std::shared_ptr<LModelData> LoadAssimpModelData(std::string filePath, unsigned int importFlags);
		void LoadModel();
		void CreateMesh(LModelNode* parent, aiNode*);
		void CreateModelFromData(LModelData* Model);
		void PopulateBones(aiNode* node, const aiScene* scene);
		void CreateBoneHierarchy();
		int GetBoneId(std::string boneName, bool insert = false);
		void Retarget(LModelResource* model);
		void SetIgnoreRootMotion(bool ignore) { m_bIgnoreRootMotion = ignore; }

		glm::vec3 GetRootMotionDelta() const { return m_rootMotionDelta; }
		glm::quat GetRootMotionRotation() const { return m_rootMotionRotation; }

		void UpdateAnimation(LAnimation* animation, float timeInSeconds);
		void UpdateAnimation(LAnimation* animation, const LModelNode& mesh, glm::mat4 parentTransform, float time);
		std::shared_ptr<std::vector<glm::mat4>> m_boneMatrices;
		int m_rootBoneId;

		std::vector<std::shared_ptr<LAnimation>> m_animationData;
		std::unordered_map<std::string, int> m_boneIdMap;
		int m_boneCount = 0;
		glm::mat4 m_globalInverseRoot;
		LAnimation* m_currentAnimation = nullptr;
		float m_animationTimeElapsed = 0;
		bool m_bInitialised = false;

		static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
		{
			glm::mat4 to;
			//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
			to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
			to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
			to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
			to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
			return to;
		}
	};
}