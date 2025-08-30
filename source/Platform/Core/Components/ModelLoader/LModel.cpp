#include "Platform/Core/Components/ModelLoader/LModel.h"
#include "Platform/Core/Components/ModelLoader/LModelMesh.h"
#include "Platform/Core/Renderer/Animation/Animation.h"
#include "Platform/Core/Renderer/Materials/Material.h"
#include "Platform/Core/Renderer/Materials/TextureType.h"
#include "Platform/Core/Services/GraphicsServices.h"
#include "Resources/ResourceHandle.h"
#include "Spatial/Transform.h"
#include "Util/Logger.h"
#include "assimp/postprocess.h"
#include <memory>
#include <filesystem>
#include <fstream>

namespace Lemonade
{
    using CitrusCore::Logger;


    void LModel::Init()
    {
        m_root->Init();
    }

    void LModel::Update()
    {
        m_root->Update();
    }

    void LModel::Render()
    {
        m_root->Render();
    }

	bool LModel::LoadResource(std::string path)
	{
        m_filePath = path;
        m_metaPath = m_filePath + ".upmeta";
		LoadModel();
		return true;
	}

	void LModel::LoadMeta()
	{
		std::ifstream ifStream(m_metaPath);

		if (!ifStream.good())
		{
			CitrusCore::Logger::Log(CitrusCore::Logger::LogLevel::WARN, "No meta data was found for model [%s] at [%s]", m_filePath.c_str(), m_metaPath.c_str());
			return;
		}

		ifStream.seekg(0, ifStream.end);
		std::streampos length = ifStream.tellg();
		char* buffer = new char[length];
		ifStream.read(buffer, length);

		//json meta = json::parse(buffer);
		//
		//if (meta.contains("Meshes"))
		//{
		//
		//}
	}

	void LModel::SaveMeta()
	{
        printf("Do something here allan for the love of  god.");
	}

    std::shared_ptr<LModelData> LModel::LoadAssimpModelData(std::string filePath, unsigned int importFlags)
    {
		std::unique_ptr<Assimp::Importer> importer = std::make_unique<Assimp::Importer>();

		const aiScene* scene = importer->ReadFile(filePath,
			importFlags |
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |

			// Should this be disabled for animations?
			aiProcess_PreTransformVertices |
			 
			//aiProcess_GenUVCoords |
			//aiProcessPreset_TargetRealtime_Quality |
			aiProcess_PopulateArmatureData |
			aiProcess_MakeLeftHanded |
			aiProcess_ValidateDataStructure |
			aiProcess_GlobalScale |
			aiProcess_SortByPType);

		if (!scene)
		{
			Logger::Log(CitrusCore::Logger::ERROR, "Error importing Assimp scene: %s", importer->GetErrorString());
			return nullptr;
		}

		std::shared_ptr<LModelData> modelData = std::make_shared<LModelData>();
		modelData->Importer = std::move(importer);

		return modelData;
    }

	void LModel::LoadModel()
	{
		m_modelData =LoadAssimpModelData(m_filePath, m_customFlags);

		if (m_modelData != nullptr)
		{
			LoadMeta();
			CreateModelFromData(m_modelData.get());
			SaveMeta();
		}
	}

	void LModel::CreateMesh(LModelMesh* parent, aiNode* node)
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

		glm::mat4 local(*node->mTransformation[0]);

		glm::mat4 zUpToYUp(
			1,  0,  0, 0,
			0,  0,  1, 0,
			0, -1,  0, 0,
			0,  0,  0, 1
		);
		//local = zUpToYUp * local;
		//glm::mat4 local(1);

		std::shared_ptr<CitrusCore::Transform> transform = std::make_shared<CitrusCore::Transform>(local);
		std::shared_ptr<LModelMesh> entity = std::make_shared<LModelMesh>(node->mName.C_Str(), transform);
        CitrusCore::ResourcePtr<Material> mat = GraphicsServices::GetGraphicsResources()->GetMaterialHandle("Assets/Materials/default.mat.json");
        entity->SetMaterial(mat);
		parent->AddChild(entity);

		//SharedPtr<UMaterial> mat = std::make_shared<UMaterial>();

		for (uint32 i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];

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

			// Check for animations that reference this mesh...
			std::string name = aimesh->mName.C_Str();

			for (std::shared_ptr<LAnimation> anim : m_animationData)
			{
				//for (SharedPtr<UBoneAnim> boneanim : anim->getBoneAnimations())
				{
					//if (boneanim->getName() == name)
					{
						animationData->push_back(anim);
					}
				}
			}

			//mesh->setBones(bones);
			//mesh->setAnimation(animationData);
			//mesh->setBoneWeights(boneWeights);
			//mesh->setBoneIds(boneIds);

			if (aimesh->mNumAnimMeshes)
			{
				Logger::Log(Logger::INFO, "Num anime meshes: [%i]", aimesh->mNumAnimMeshes);
			}

			uint32 vbufferSize = aimesh->mNumFaces * 3;
			uint32 cbufferSize = aimesh->mNumFaces * 4;

			std::vector<glm::vec4> boneweights;
			std::vector<glm::vec4> boneids;

			if (aimesh->HasBones())
			{
				boneweights.resize(aimesh->mNumVertices);
				boneids.resize(aimesh->mNumVertices);

				boneWeights->resize(vbufferSize);
				boneIds->resize(vbufferSize);

				std::fill(boneIds->begin(), boneIds->end(), glm::vec4(-1, -1, -1, -1));

				glm::vec4 weight;

				for (uint ibone = 0; ibone < aimesh->mNumBones; ++ibone)
				{
					aiBone* bone = aimesh->mBones[ibone];
					auto ubone = std::make_shared<LBone>(bone->mName.C_Str(), 
														GetBoneId(bone->mNode->mName.C_Str()), 
														GetBoneId(bone->mNode->mParent->mName.C_Str()));

					glm::mat4 offsetMatrix(
						bone->mOffsetMatrix.a1, bone->mOffsetMatrix.b1, bone->mOffsetMatrix.c1, bone->mOffsetMatrix.d1,
						bone->mOffsetMatrix.a2, bone->mOffsetMatrix.b2, bone->mOffsetMatrix.c2, bone->mOffsetMatrix.d2,
						bone->mOffsetMatrix.a3, bone->mOffsetMatrix.b3, bone->mOffsetMatrix.c3, bone->mOffsetMatrix.d3,
						bone->mOffsetMatrix.a4, bone->mOffsetMatrix.b4, bone->mOffsetMatrix.c4, bone->mOffsetMatrix.d4
					);


					ubone->SetOffsetMatrix(ConvertMatrixToGLMFormat(bone->mOffsetMatrix));
					//ubone->setOffsetMatrix(offsetMatrix);

					auto weights = std::make_shared<std::vector<LBone::UVertexWeight>>();
					weights->resize(bone->mNumWeights);

					for (uint p = 0; p < bone->mNumWeights; ++p)
					{
						//if (ibone < 4)
						{
							for (int pbonindex = 0; pbonindex < 4; ++pbonindex)
							{
								// needs some work to check if they are already set
								if ((*boneIds)[bone->mWeights[p].mVertexId][pbonindex] == -1)
								{
									(*boneWeights)[bone->mWeights[p].mVertexId][pbonindex] = bone->mWeights[p].mWeight;
									std::string nameyname = bone->mNode->mName.C_Str();
									(*boneIds)[bone->mWeights[p].mVertexId][pbonindex] = GetBoneId(bone->mName.C_Str());
									boneweights[bone->mWeights[p].mVertexId][pbonindex] = bone->mWeights[p].mWeight;
									boneids[bone->mWeights[p].mVertexId][pbonindex] = GetBoneId(bone->mName.C_Str());
									break;
								}
							}


						}

						(*weights)[p].VertexIndex = bone->mWeights[p].mVertexId;
						(*weights)[p].Weight = bone->mWeights[p].mWeight;
					}

					ubone->SetWeights(weights);
					bones->push_back(ubone);
				}
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
					(*vertices)[index] = std::move(glm::vec3(vertex.x, vertex.y, vertex.z) * (float)scale);

					if (boneweights.size())
					{
						(*boneWeights)[index] = boneweights[face->mIndices[q]];
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
				// AI_MATKEY_NAME
				// Causes build error on linux undefined
				//aiString name = material->GetName();
				aiString name;
				aiString path;
				material->Get(AI_MATKEY_NAME, name);
				std::filesystem::path fspath(m_filePath);
				matname = name.C_Str();

		        std::string modelDirectory = fspath.parent_path().string();
		        std::string materialPath = modelDirectory + "/materials/" + name.C_Str() + ".mat.json";
                mat = GraphicsServices::GetGraphicsResources()->GetMaterialHandle(materialPath);

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
					// Manually load textures
					for (int i = 1; i < (int)TextureType::Unknown; ++i)
					{
						if (material->GetTexture((aiTextureType)i, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
						{
							mat = GraphicsServices::GetGraphicsResources()->GetMaterialHandle("Assets/Materials/default.mat.json"); 
							mat->GetResource()->LoadTexture((TextureType)i, path.C_Str(), i);
							bMaterialFound = true;
						}
					}
				}
				//else if (mat->init(materialPath))
				//{
				//	baseColour = mat->getBaseColour();
				//	bMaterialFound = true;
				//}
				//else
				//{
				//	for (int i = 1; i < (int)UTextureType::Unknown; ++i)
				//	{
				//		if (material->GetTexture((aiTextureType)i, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
				//		{
				//			mat->init("assets/materials/default.material");
				//			mat->loadTexture(std::make_shared<UTextureData>((UTextureType)i, path.C_Str()));
				//			LogGLErrors();
				//			bMaterialFound = true;
				//		}
				//	}
//
					aiColor4D color;
					material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
					baseColour = { color.r, color.g, color.b, color.a };
//
				//	if (bMaterialFound)
				//	{
				//		mat->init("assets/materials/default.material");
				//		mat->setBaseColour(baseColour);
				//		mat->getShader().setUniformVec4("baseColour", baseColour);
				//		mat->save(materialPath);
				//	}
				//}
//
				//// Use colour material if no materials present on mesh (this is correct functionality)
				//if (!bMaterialFound)
				//{
				//	mat->init("assets/materials/default.material");
				//	mat->setBaseColour(baseColour);
				//	mat->getShader().setUniformVec4("baseColour", baseColour);
				//	mat->save(materialPath);
				//}
//
				if (baseColour.a != 1)
				{
					//entity->setRenderPriority((uint)URenderPriority::Transparent);
				}
			}
			else
			{
				//mat->init("assets/materials/default.material");
				//mat->getShader().setUniformVec4("baseColour", baseColour);
			}

			mesh->SetNormals(normals);
			mesh->SetUVS(uvs);
			mesh->SetVertices(vertices);
            mesh->SetShouldGenerateTangents(false);
			//mesh->SetColours(colours);
			//mesh->SetTangents(tangents);
			//mesh->SetBiTangents(biTangents);

			// Add collider experiment 
			//if (m_filePath.find("mike.fbx") == std::string::npos)
			//if (m_filePath.find("FpsProto.fbx") != std::string::npos)
			//{		
			//	if (mat->hasProperty("prop_physics"))
			//	{
			//		auto box = entity->addComponent<UNewtonBox>(glm::vec3(1,1,1));
			//		SharedPtr<UNewtonBody> body = entity->addComponent<UNewtonBody>(box.get(), false, false);
			//	}
			//	else
			//	{
			//		SharedPtr<UNewtonTreeCollision> hull = entity->addComponent<UNewtonTreeCollision>(mesh);
			//		SharedPtr<UNewtonBody> body = entity->addComponent<UNewtonBody>(hull.get(), true, false);
			//	}
			//}

			// Uncomment for Everything has collisions... for now.
			//if (mat->hasProperty("prop_physics_newton_tree"))
			//{
			//	SharedPtr<UNewtonTreeCollision> hull = entity->addComponent<UNewtonTreeCollision>(mesh);
			//	SharedPtr<UNewtonBody> body = entity->addComponent<UNewtonBody>(hull.get(), true, false);
			//}

			//for (const std::string& component : mat->getUserComponents())
			//{
			//	SharedPtr<UComponent> comp = UpsilonEngine::UServiceLocator::getInstance()->getRTTI()->createFromString<UComponent>(component);
			//	entity->addComponent(comp);
			//}
            entity->SetMesh(mesh);
            //entity->SetMaterial(mat);
		}

		//if (node->mMetaData)
		//{
		//	// Unclear is assimp has a way to import custom attributes exported from blender
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

	void LModel::CreateModelFromData(LModelData* Model)
	{
		// Cameras ?

		// Meshes/ Materials
		const aiScene* scene = m_modelData->Importer->GetScene();

		std::filesystem::path path(m_filePath);
		m_root = std::make_shared<LModelMesh>(path.filename().string(), std::make_shared<CitrusCore::Transform>());

		//getParent()->addChild(root);

		// Animations - Need to be loaded prior to mesh!
		for (uint i = 0; i < scene->mNumAnimations; ++i)
		{
			aiAnimation* animation = scene->mAnimations[i];
			auto uAnimation = std::make_shared<LAnimation>(animation->mName.C_Str(), animation->mDuration, animation->mTicksPerSecond);
			
			for (uint p = 0; p < animation->mNumChannels; ++p)
			{
				aiNodeAnim* node = animation->mChannels[p];
				std::shared_ptr<LBoneAnim> boneAnim = std::make_shared<LBoneAnim>(node->mNodeName.C_Str(), GetBoneId(node->mNodeName.C_Str()));

				for (uint j = 0; j < node->mNumPositionKeys; ++j)
				{
					const aiVectorKey key = node->mPositionKeys[j];
					boneAnim->AddPositionkey(VectorKey(key.mTime, glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z)));
				}

				for (uint k = 0; k < node->mNumScalingKeys; ++k)
				{
					const aiVectorKey key = node->mScalingKeys[k];
					boneAnim->AddScalingkey(VectorKey(key.mTime, glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z)));
				}

				for (uint l = 0; l < node->mNumRotationKeys; ++l)
				{
					const aiQuatKey key = node->mRotationKeys[l];
					boneAnim->AddRotationkey(QuatKey(key.mTime, glm::vec4(key.mValue.x, key.mValue.y, key.mValue.z, key.mValue.w)));
				}

				uAnimation->AddBoneAnimation(boneAnim);
			}

			m_animationData.push_back(uAnimation);
		}

		if (scene->mRootNode != nullptr)
		{
			CreateMesh(m_root.get(), scene->mRootNode);
		}
	}

	int LModel::GetBoneId(std::string boneName)
	{
		auto entry = m_boneIdMap.find(boneName);

		if (entry != m_boneIdMap.end())
		{
			return entry->second;
		}

		m_boneCount++;
		int boneId = m_boneCount;
		m_boneIdMap.insert(std::make_pair(boneName, boneId));
		return boneId;
	}
}
