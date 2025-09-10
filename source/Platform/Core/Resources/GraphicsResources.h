#pragma once

#include <Base/Object.h>
#include <Platform/Core/Components/ModelLoader/LModel.h>
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
		CitrusCore::ResourcePtr<ATexture> GetTextureHandle(std::string path);
		CitrusCore::ResourcePtr<Material> GetMaterialHandle(std::string path);
		CitrusCore::ResourcePtr<AShader> GetShaderHandle(std::string path);
		CitrusCore::ResourcePtr<LModel> GetModelHandle(std::string path);
	private:
		CitrusCore::ResourceGroup<ATexture> m_textures;
		CitrusCore::ResourceGroup<AShader> m_shaders;
		CitrusCore::ResourceGroup<Material> m_materials;
		CitrusCore::ResourceGroup<LModel> m_models;
	};
}