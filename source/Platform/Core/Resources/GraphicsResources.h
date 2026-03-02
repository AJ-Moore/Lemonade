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
		CitrusCore::ResourcePtr<T> GetResourceHandle(std::string path)
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

		CitrusCore::ResourcePtr<ATexture> GetTextureHandle(std::string path);
		CitrusCore::ResourcePtr<Material> GetMaterialHandle(std::string path);
		CitrusCore::ResourcePtr<AShader> GetShaderHandle(std::string path);
		CitrusCore::ResourcePtr<LModelResource> GetModelHandle(std::string path);
	private:
		CitrusCore::ResourceGroup<ATexture> m_textures;
		CitrusCore::ResourceGroup<AShader> m_shaders;
		CitrusCore::ResourceGroup<Material> m_materials;
		CitrusCore::ResourceGroup<LModelResource> m_models;
	};
}