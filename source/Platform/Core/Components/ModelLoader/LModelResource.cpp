#include <assimp/types.h>
#include <Platform/Core/Components/ModelLoader/LModelResource.h>
#include <Platform/Core/Components/ModelLoader/LModelMesh.h>
#include <Platform/Core/Renderer/Animation/Animation.h>
#include <Platform/Core/Renderer/Materials/Material.h>
#include <Platform/Core/Renderer/Materials/TextureType.h>
#include <Platform/Core/Services/GraphicsServices.h>
#include <Resources/ResourceHandle.h>
#include <Spatial/Transform.h>
#include <Util/Logger.h>
#include <assimp/postprocess.h>
#include <cmath>
#include <glm/fwd.hpp>
#include <memory>
#include <filesystem>
#include <unordered_map>
#include <unordered_set>

namespace Lemonade
{
    using CitrusCore::Logger;

    void LModelResource::Init()
    {
		if (m_bInitialised) return;
        m_root->Init();
		m_bInitialised = true;
    }

	void LModelResource::PlayAnimation(uint32 animationIndex)
	{
		if (animationIndex < m_animationData.size())
		{
			m_bAnimationPlaying = true; 
			m_currentAnimation = m_animationData[0].get();
		}
	}

	LAnimation* LModelResource::GetAnimation(uint32 index)
	{
		if (index < m_animationData.size())
		{
			return m_animationData[index].get();
		}

		return nullptr;
	}

	void LModelResource::PlayAnimation(LAnimation* animation)
	{
		if (animation != nullptr)
		{
			m_bAnimationPlaying = true;
			m_currentAnimation = animation; 
		}
	}

    void LModelResource::Update()
    {
		float time = Lemonade::GraphicsServices::GetTime()->GetTimeSinceApplicationStarted();

		if (m_bAnimationPlaying && m_currentAnimation != nullptr)
		{
			float timeInSeconds = m_currentAnimation->GetDuration()/ m_currentAnimation->GetTicksPerSecond();
			UpdateAnimation(m_currentAnimation, std::fmod(time, timeInSeconds));
		}

        m_root->Update();
    }

    void LModelResource::Render()
    {
        m_root->Render();
    }

	bool LModelResource::LoadResource(std::string path)
	{
        m_filePath = path;
		LoadModel();
		return true;
	}

    std::shared_ptr<LModelData> LModelResource::LoadAssimpModelData(std::string filePath, unsigned int importFlags)
    {
		std::unique_ptr<Assimp::Importer> importer = std::make_unique<Assimp::Importer>();

		const aiScene* sceneValidator = importer->ReadFile(filePath, aiProcess_ValidateDataStructure);

		if (!sceneValidator) {
			Logger::Log(Logger::ERROR, "Unable to load scene.");
			return nullptr;
		}
		
		bool hasSkinnedMesh = false;

		for (unsigned int i = 0; i < sceneValidator->mNumMeshes; ++i)
		{
			if (sceneValidator->mMeshes[i]->HasBones())
			{
				hasSkinnedMesh = true;
				break;
			}
		}

		unsigned int flags = 			
			importFlags |
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			//aiProcess_LimitBoneWeights |
			//aiProcess_GenUVCoords |
			//aiProcessPreset_TargetRealtime_Quality |
			aiProcess_PopulateArmatureData |
			aiProcess_ValidateDataStructure |
			aiProcess_GlobalScale |
			aiProcess_SortByPType;


		if (!hasSkinnedMesh)
		{
			// At present both these flags break skinning.
			flags |= aiProcess_PreTransformVertices;
			flags |= aiProcess_JoinIdenticalVertices;
		}

		const aiScene* scene = importer->ReadFile(filePath, flags);

		if (!scene)
		{
			Logger::Log(CitrusCore::Logger::ERROR, "Error importing Assimp scene: %s", importer->GetErrorString());
			return nullptr;
		}

		std::shared_ptr<LModelData> modelData = std::make_shared<LModelData>();
		modelData->Importer = std::move(importer);

		return modelData;
    }

	void LModelResource::LoadModel()
	{
		m_boneMatrices = std::make_shared<std::vector<glm::mat4>>();
		m_modelData = LoadAssimpModelData(m_filePath, m_customFlags);

		if (m_modelData != nullptr)
		{
			LoadMeta();
			CreateModelFromData(m_modelData.get());
			CreateBoneHierarchy();

			if (!m_animationData.empty())
			{
				UpdateAnimation(m_animationData[0].get(), 0);				
			}
		}
	}

	void LModelResource::UpdateAnimation(LAnimation* animation, const LModelNode& node, glm::mat4 parentTransform, float timeInSeconds)
	{
		glm::mat4 bindPose = node.GetNodeTransform();

		auto it = m_boneIdMap.find(node.GetName());

		// non bone find node in boneanims 
		for (auto& boneAnim : animation->GetBoneAnimations())
		{
			if (boneAnim->GetName() == node.GetName())
			{
				bindPose = boneAnim->GetBoneMatrixForAnimTime(timeInSeconds, bindPose);
			}
		}
		
		// compute global transform for this node
		glm::mat4 globalTransform = parentTransform * bindPose;
		
		// if this node is a bone, write final matrix
		if (it != m_boneIdMap.end())
		{
			LBone* bone = m_skeleton[it->second].get();
			(*m_boneMatrices)[bone->GetBoneId()] = m_globalInverseRoot * globalTransform * bone->GetOffsetMatrix();
		}
		
		for (const std::shared_ptr<LModelNode>& child : node.GetChildren())
		{
			UpdateAnimation(animation, *child.get(), globalTransform, timeInSeconds);
		}
	}

	void LModelResource::UpdateAnimation(LAnimation* animation, float timeInSeconds)
	{
		// +1 last slot reserved for identity.
		(*m_boneMatrices).resize(m_boneIdMap.size() + 1);
		
		for (int i = 0; i < (*m_boneMatrices).size(); ++i)
		{
			(*m_boneMatrices)[i] = glm::mat4(1.0f);
		}

		float ticks = animation->GetAnimationTime(timeInSeconds);

		for (auto& boneAnim : animation->GetBoneAnimations())
		{
			if (boneAnim->GetId() == -1) continue;

			glm::mat4 nodeTransform = m_root->GetNodeTransform();
			m_skeleton[boneAnim->GetId()]->SetBoneMatrix(boneAnim->GetBoneMatrixForAnimTime(timeInSeconds, nodeTransform));
		}

		UpdateAnimation(animation, *m_root.get(), glm::mat4(1), timeInSeconds);
	}

	void LModelResource::CreateBoneHierarchy()
	{
		for (auto& bone : m_skeleton)
		{
			int parentid = bone.second->GetParentBoneId();

			if (parentid == -1)
			{
				m_rootBoneId = bone.second->GetBoneId();
			}

			if (m_skeleton.contains(parentid))
			{
				m_skeleton[parentid]->AddChild(bone.second);
			}
		}
	}

	void LModelResource::CreateMesh(LModelNode* parent, aiNode* node)
	{
		const aiScene* scene = m_modelData->Importer->GetScene();
		float scale = 1.0f;
		float originalScale = 1.0f;
		const char* unitName = "m";
		scene->mMetaData->Get("UnitScaleFactor", scale);
		scene->mMetaData->Get("OriginalUnitScaleFactor", originalScale);
		scene->mMetaData->Get("UnitName", unitName);

		if (parent == nullptr || node == nullptr || scene == nullptr)
		{
			Logger::Log(Logger::ERROR,"Error creating mesh!");
			return;
		}

		glm::mat4 local = ConvertMatrixToGLMFormat(node->mTransformation);

		bool hasBones = false;

		for (uint32 i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];


			if (aimesh->HasBones())
			{
				hasBones = true;
			}
		}

		glm::mat4 localTransform = hasBones ? glm::mat4(1.0f) : local;

		std::shared_ptr<CitrusCore::Transform> transform = std::make_shared<CitrusCore::Transform>(localTransform);
		std::shared_ptr<LModelNode> entity = std::make_shared<LModelNode>(node->mName.C_Str(), transform);
		entity->SetNodeTransform(local);

        CitrusCore::ResourcePtr<Material> mat;
		
		if (hasBones)
		{
			mat = GraphicsServices::GetGraphicsResources()->GetMaterialHandle("Assets/Materials/defaultskinnedpbr.mat.json");
		}
		else {
			mat = GraphicsServices::GetGraphicsResources()->GetMaterialHandle("Assets/Materials/defaultpbr.mat.json");
		}

		parent->AddChild(entity);

		for (uint32 i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
			std::shared_ptr<LModelMesh> lmesh = std::make_shared<LModelMesh>(aimesh->mName.C_Str(), transform);

			if (aimesh == nullptr)
			{
				Logger::Log(Logger::ERROR, "aimesh is null!");
				continue;
			}

			std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
			std::shared_ptr<std::vector<std::shared_ptr<LBone>>> bones = std::make_shared<std::vector<std::shared_ptr<LBone>>>();
			std::shared_ptr<std::vector<glm::vec4>> boneWeights = std::make_shared<std::vector<glm::vec4>>();
			std::shared_ptr<std::vector<glm::vec4>> boneIds = std::make_shared<std::vector<glm::vec4>>();
			std::shared_ptr<std::vector<std::shared_ptr<LAnimation>>> animationData = std::make_shared<std::vector<std::shared_ptr<LAnimation>>>();

			for (std::shared_ptr<LAnimation> anim : m_animationData)
			{
				animationData->push_back(anim);
			}

			mesh->SetBones(bones);
			mesh->SetAnimation(animationData);
			mesh->SetBoneWeights(boneWeights);
			mesh->SetBoneIds(boneIds);
			mesh->SetBoneMatrix(m_boneMatrices);

			if (aimesh->mNumAnimMeshes)
			{
				Logger::Log(Logger::INFO, "Num anime meshes: [%i]", aimesh->mNumAnimMeshes);
			}

			uint32 vbufferSize = aimesh->mNumFaces * 3;
			uint32 cbufferSize = aimesh->mNumFaces * 4;

			std::vector<glm::vec4> weightsToSort;
			std::vector<glm::vec4> boneids;

			if (aimesh->HasBones())
			{
				weightsToSort.resize(aimesh->mNumVertices);
				boneids.resize(aimesh->mNumVertices);

				boneWeights->resize(vbufferSize);
				boneIds->resize(vbufferSize);

				std::fill(boneids.begin(), boneids.end(), glm::vec4(-1,-1,-1,-1));
				glm::vec4 weight;

				for (uint ibone = 0; ibone < aimesh->mNumBones; ++ibone)
				{
					aiBone* bone = aimesh->mBones[ibone];

					// Crawl up hierachy and find first parent that is a bone.
					int parentBoneId = -1;
					aiNode* parent = bone->mNode->mParent;

					while(parent != nullptr && parentBoneId < 0)
					{
						parentBoneId = GetBoneId(parent->mName.C_Str());

						if (parentBoneId < 0)
						{
							parent = parent->mParent;
						}
					}

					auto ubone = std::make_shared<LBone>(bone->mName.C_Str(), 
														GetBoneId(bone->mNode->mName.C_Str()), 
														parentBoneId);

					glm::mat4 offsetMatrix(
						bone->mOffsetMatrix.a1, bone->mOffsetMatrix.b1, bone->mOffsetMatrix.c1, bone->mOffsetMatrix.d1,
						bone->mOffsetMatrix.a2, bone->mOffsetMatrix.b2, bone->mOffsetMatrix.c2, bone->mOffsetMatrix.d2,
						bone->mOffsetMatrix.a3, bone->mOffsetMatrix.b3, bone->mOffsetMatrix.c3, bone->mOffsetMatrix.d3,
						bone->mOffsetMatrix.a4, bone->mOffsetMatrix.b4, bone->mOffsetMatrix.c4, bone->mOffsetMatrix.d4
					);

					ubone->SetOffsetMatrix(ConvertMatrixToGLMFormat(bone->mOffsetMatrix));

					auto weights = std::make_shared<std::vector<LBone::UVertexWeight>>();
					weights->resize(bone->mNumWeights);

					int maxBonesPerVertex = 4;
					int boneId = ubone->GetBoneId();

					for (uint p = 0; p < bone->mNumWeights; ++p)
					{
						int vertexId = bone->mWeights[p].mVertexId;
						float weight =  bone->mWeights[p].mWeight;
						int id = boneId;
						bool inserted = false;
						int lowIndex = 0;
						float lowValue = weightsToSort[vertexId][0];

						for (int index = 0; index < maxBonesPerVertex; ++index)
						{
							if (weightsToSort[vertexId][index] < lowValue)
							{
								lowValue = weightsToSort[vertexId][index];
								lowIndex = index;
							}

							if (boneids[vertexId][index] == -1)
							{
								inserted = true;
								weightsToSort[vertexId][index] = bone->mWeights[p].mWeight;
								boneids[vertexId][index] = boneId;
								break; // move to next bone
							}
						}

						if (!inserted)
						{
							// Replace value with lowest weight
							weightsToSort[vertexId][lowIndex] = bone->mWeights[p].mWeight;
							boneids[vertexId][lowIndex] = boneId;
						}

						(*weights)[p].VertexIndex = bone->mWeights[p].mVertexId;
						(*weights)[p].Weight = bone->mWeights[p].mWeight;
					}

					ubone->SetWeights(weights);
					bones->push_back(ubone);
					m_skeleton[ubone->GetBoneId()] = ubone;		
					lmesh->AddBone(ubone);		
				}
			}

			for (auto& id : *boneIds)
			{
				if (id.x == -1) id.x = boneIds->size() + 1;
				if (id.y == -1) id.y = boneIds->size() + 1;
				if (id.z == -1) id.z = boneIds->size() + 1;
				if (id.w == -1) id.w = boneIds->size() + 1;
			}


			std::shared_ptr<std::vector<glm::vec3>> vertices = std::make_shared<std::vector<glm::vec3>>();
			std::shared_ptr<std::vector<glm::vec3>> normals = std::make_shared<std::vector<glm::vec3>>();
			std::shared_ptr<std::vector<glm::vec4>> colours = std::make_shared<std::vector<glm::vec4>>();
			std::shared_ptr<std::vector<glm::vec2>> uvs = std::make_shared<std::vector<glm::vec2>>();
			std::shared_ptr<std::vector<glm::vec3>> tangents = std::make_shared<std::vector<glm::vec3>>();
			std::shared_ptr<std::vector<glm::vec3>> biTangents = std::make_shared<std::vector<glm::vec3>>();

			vertices->resize(vbufferSize);
			normals->resize(vbufferSize);
			colours->resize(cbufferSize);
			uvs->resize(vbufferSize);
			tangents->resize(vbufferSize);
			biTangents->resize(vbufferSize);

			std::fill(colours->begin(), colours->end(), glm::vec4(1,1,1,1));

			bool bHasNormals = aimesh->HasNormals();
			bool bHasUvs = aimesh->HasTextureCoords(0);
			bool bHasColours = aimesh->HasVertexColors(0);
			bool bHasTangents = aimesh->HasTangentsAndBitangents();

			for (uint32 p = 0; p < aimesh->mNumFaces; p++)
			{
				aiFace* face = &aimesh->mFaces[p];
				for (uint32 q = 0; q < 3; ++q)
				{
					if (face->mNumIndices != 3)
					{
						Logger::Log(Logger::WARN, "Face has unsupported indices!, indices found [%i]", face->mNumIndices);
						continue;
					}

					uint32 index = (p * 3) + q;

					aiVector3D vertex = aimesh->mVertices[face->mIndices[q]];
					(*vertices)[index] = std::move(glm::vec3(vertex.x, vertex.y, vertex.z));

					if (weightsToSort.size())
					{
						float sum = weightsToSort[face->mIndices[q]].x + weightsToSort[face->mIndices[q]].y + weightsToSort[face->mIndices[q]].z + weightsToSort[face->mIndices[q]].w;
						glm::vec4 weight = weightsToSort[face->mIndices[q]]/ sum;

						(*boneWeights)[index] = weight;
					}

					if (boneids.size())
					{
						(*boneIds)[index] = boneids[face->mIndices[q]];
					}

					if (bHasNormals)
					{
						aiVector3D normal = aimesh->mNormals[face->mIndices[q]];
						(*normals)[index] = std::move(glm::vec3(normal.x, normal.y, normal.z));
					}
					
					if (bHasColours)
					{
						aiColor4D* colour = aimesh->mColors[0];

						if (colour != nullptr)
						{
							(*colours)[index] = std::move(glm::vec4(colour->r, colour->g, colour->b, colour->a));
						}
						else
						{
							Logger::Log(Logger::ERROR, "Colour information null, requires investigation.");
						}
					}

					if (bHasUvs)
					{
						aiVector3D uv = aimesh->mTextureCoords[0][face->mIndices[q]];
						(*uvs)[index] = std::move(glm::vec2(uv.x, uv.y));
					}

					if (bHasTangents)
					{
						aiVector3D tangent = aimesh->mTangents[face->mIndices[q]];
						(*tangents)[index] = std::move(glm::vec3(tangent.x, tangent.y, tangent.z));

						tangent = aimesh->mBitangents[face->mIndices[q]];
						(*biTangents)[index] = std::move(glm::vec3(tangent.x, tangent.y, tangent.z));
					}
				}
			}

			bool bMaterialFound = false;
			glm::vec4 baseColour = glm::vec4(1, 1, 1, 1);
			std::string matname;
		
			if ((scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) == 0)
			{
				aiMaterial* material = scene->mMaterials[aimesh->mMaterialIndex];
				aiString name;
				aiString path;
				material->Get(AI_MATKEY_NAME, name);
				std::filesystem::path fspath(m_filePath);
				matname = name.C_Str();

		        std::string modelDirectory = fspath.parent_path().string();
		        std::string materialPath = modelDirectory + "/Materials/" + name.C_Str() + ".mat.json";
				Logger::Log(Logger::VERBOSE, "Attempting to load material [%s]", materialPath.c_str());

				// try and get material from meta, else try and load material from .'/Material/mat_name.material'
				std::unordered_map<std::string, std::string>::iterator iter = m_meta.m_materials.find(name.C_Str());
				if (iter != m_meta.m_materials.end())
				{
					mat = GraphicsServices::GetGraphicsResources()->GetMaterialHandle(iter->second); 
					bMaterialFound = true;
				}

				// Material not found in meta, try loading original material path
				if (!bMaterialFound)
				{
					// Check if mat exists at path
					bool pathValid = std::filesystem::exists(materialPath);

					if (pathValid)
					{
						mat = GraphicsServices::GetGraphicsResources()->GetMaterialHandle(materialPath); 
						baseColour = mat->GetResource()->GetBaseColour();
						bMaterialFound = true;
					}
				}

				// Material file doesn't yet exist? create it from scratch?
				if (!bMaterialFound)
				{
					std::unordered_set<TextureType> pbr ={
						TextureType::Emissive,
						TextureType::Normal,
						TextureType::Diffuse,
						TextureType::Roughness,
						TextureType::AmbientOcclusion,
						TextureType::Emissive, 
						TextureType::Metalness,
					};

					// Manually load textures
					for (int i = 1; i < (int)TextureType::Unknown; ++i)
					{
						// temporarily only allow modern pbr workflow.
						if (!pbr.contains((TextureType)i))
						{
							continue;
						}

						if (material->GetTexture((aiTextureType)i, 0, &path) == AI_SUCCESS)
						{
							int bindLocation = mat->GetResource()->GetBindLocation(static_cast<TextureType>(i));

							if (bindLocation == Material::INVALID_BIND_LOCATION)
							{
								Logger::Log(Logger::ERROR, "Material/ Shader does not support texture type [%i]", i);
							}
							else {
								mat->GetResource()->LoadTexture((TextureType)i, path.C_Str(), bindLocation);
								bMaterialFound = true;
							}
						}
					}
				}

				aiColor4D color(1.0f, 1.0f, 1.0f, 1.0f);
				if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color))
				{
					baseColour = { color.r, color.g, color.b, color.a };
				}

				mat->GetResource()->SetBaseColour(baseColour);

				if (baseColour.a != 1)
				{
					// TODO
					//entity->setRenderPriority((uint)URenderPriority::Transparent);
				}
			}

			mesh->SetNormals(normals);
			mesh->SetUVS(uvs);
			mesh->SetVertices(vertices);
            mesh->SetShouldGenerateTangents(false);
			//mesh->SetColours(colours);
			mesh->SetTangents(tangents);
			mesh->SetBiTangents(biTangents);

			//for (const std::string& component : mat->getUserComponents())
			//{
			//	SharedPtr<UComponent> comp = UpsilonEngine::UServiceLocator::getInstance()->getRTTI()->createFromString<UComponent>(component);
			//	entity->addComponent(comp);
			//}

			lmesh->SetMesh(mesh);
			lmesh->SetMaterial(mat);
            entity->AddMeshRenderer(lmesh);
		}

		//if (node->mMetaData)
		//{
		//	// Unclear if assimp has a way to import custom attributes exported from blender
		//	if (node->mMetaData->HasKey("Component"))
		//	{
		//		const char* componentName = "";
		//		std::string compToCreate;
		//		if (node->mMetaData->Get("Component", componentName))
		//		{
		//			SharedPtr<UComponent> comp = UpsilonEngine::UServiceLocator::getInstance()->getRTTI()->createFromString<UComponent>(componentName);
		//			entity->addComponent(comp);
		//		}
		//	}
		//}

		for (uint32 i = 0; i < node->mNumChildren; ++i)
		{
			CreateMesh(entity.get(), node->mChildren[i]);
		}
	}

	void LModelResource::PopulateBones(aiNode* node, const aiScene* scene)
	{
		for (uint i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			if (mesh->HasBones())
			{
				for (uint ibone = 0; ibone < mesh->mNumBones; ++ibone)
				{
					aiBone* bone = mesh->mBones[ibone];
					GetBoneId(bone->mNode->mName.C_Str(), true);
				}
			}
		}
	
		for (uint i = 0; i < node->mNumChildren; ++i)
		{
			PopulateBones(node->mChildren[i], scene);
		}
	}

	void LModelResource::CreateModelFromData(LModelData* Model)
	{
		// TODO Cameras ?

		// Meshes/ Materials
		const aiScene* scene = m_modelData->Importer->GetScene();

		std::filesystem::path path(m_filePath);
		m_root = std::make_shared<LModelNode>(path.filename().string(), std::make_shared<CitrusCore::Transform>(ConvertMatrixToGLMFormat(scene->mRootNode->mTransformation)));

		// Populate bone ids 
		PopulateBones(scene->mRootNode, scene);

		// Animations - Need to be loaded prior to mesh!
		for (uint i = 0; i < scene->mNumAnimations; ++i)
		{
			aiAnimation* animation = scene->mAnimations[i];
			auto uAnimation = std::make_shared<LAnimation>(animation->mName.C_Str(), animation->mDuration, animation->mTicksPerSecond);
			
			for (uint p = 0; p < animation->mNumChannels; ++p)
			{
				aiNodeAnim* node = animation->mChannels[p];
			
				if (!m_boneIdMap.contains(node->mNodeName.C_Str()))
				{
					printf("invalid bone - non bone skinned based animation not currently supported sorry");
					//continue;
					// Hack we let it add a "bone anim" we can tell theres no bone because the bone id will be -1
				}

				std::shared_ptr<LBoneAnim> boneAnim = std::make_shared<LBoneAnim>(node->mNodeName.C_Str(), GetBoneId(node->mNodeName.C_Str()));

				for (uint j = 0; j < node->mNumPositionKeys; ++j)
				{
					const aiVectorKey key = node->mPositionKeys[j];
					boneAnim->AddPositionkey(VectorKey(key.mTime/ animation->mTicksPerSecond, glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z)));
				}

				for (uint k = 0; k < node->mNumScalingKeys; ++k)
				{
					const aiVectorKey key = node->mScalingKeys[k];
					boneAnim->AddScalingkey(VectorKey(key.mTime/ animation->mTicksPerSecond, glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z)));
				}

				for (uint l = 0; l < node->mNumRotationKeys; ++l)
				{
					const aiQuatKey key = node->mRotationKeys[l];
					boneAnim->AddRotationkey(QuatKey(key.mTime/ animation->mTicksPerSecond, glm::quat(key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z)));
				}

				uAnimation->AddBoneAnimation(boneAnim);
			}

			m_animationData.push_back(uAnimation);
		}

		if (scene->mRootNode != nullptr)
		{
			m_globalInverseRoot = glm::inverse(ConvertMatrixToGLMFormat(scene->mRootNode->mTransformation));
			CreateMesh(m_root.get(), scene->mRootNode);
		}
	}

	int LModelResource::GetBoneId(std::string boneName, bool insert)
	{
		auto entry = m_boneIdMap.find(boneName);

		if (entry != m_boneIdMap.end())
		{
			return entry->second;
		}

		if (!insert)
		{
			return -1;
		}

		m_boneIdMap[boneName] = m_boneCount;
		m_boneCount++;
		return m_boneCount;
	}
}
