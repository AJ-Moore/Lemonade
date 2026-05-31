#pragma once

#include <Resources/AResource.h>
#include <Base/Object.h>
#include <Platform/Core/Components/ModelLoader/LModelResource.h>
#include <Platform/Core/Renderer/Materials/Material.h>
#include <LCommon.h>
#include <Resources/ResourceGroup.h>
#include <Resources/ResourceHandle.h>
#include <Platform/Core/Renderer/Materials/ATexture.h>

namespace Lemonade
{
	class LEMONADE_API LGraphicsResources : CitrusCore::Object
	{
	public:
		template<class T>
		CitrusCore::ResourcePtr<T> GetResourceHandle(const std::string& path)
		{
			if constexpr (std::is_same_v<T, ATexture>)
			{
				return GetTextureHandle(path);
			}
			else if constexpr (std::is_same_v<T, Material>)
			{
				return GetMaterialHandle(path);
			}
			else if constexpr (std::is_same_v<T, AShader>)
			{
				return GetShaderHandle(path);
			}
			else if constexpr (std::is_same_v<T, LModelResource>)
			{
				return GetModelHandle(path);
			}

			return nullptr;
		}

		CitrusCore::ResourcePtr<ATexture> GetTextureHandle(const std::string& path, std::function<void(ATexture*)> construct = nullptr);
		CitrusCore::ResourcePtr<Material> GetMaterialHandle(const std::string& path, std::function<void(Material*)> construct = nullptr);
		CitrusCore::ResourcePtr<Material> GetMaterialMasqueradeFromBase(const std::string& basePath,const std::string& path, std::function<void(Material*)> construct = nullptr);
		CitrusCore::ResourcePtr<AShader> GetShaderHandle(const std::string& path, std::function<void(AShader*)> construct = nullptr);
		CitrusCore::ResourcePtr<LModelResource> GetModelHandle(const std::string& path, std::function<void(LModelResource*)> construct = nullptr);
	private:
		CitrusCore::ResourceGroup<ATexture> m_textures;
		CitrusCore::ResourceGroup<AShader> m_shaders;
		CitrusCore::ResourceGroup<Material> m_materials;
		CitrusCore::ResourceGroup<LModelResource> m_models;
	};
}