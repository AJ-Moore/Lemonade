#pragma once 

#include "Platform/Core/Renderer/Materials/TextureType.h"
#include "Resources/AResource.h"
#include "Resources/ResourceHandle.h"
#include "Util/Logger.h"
#include <Platform/Core/Renderer/Materials/AShaderProgram.h>
#include <Platform/Core/Renderer/Materials/ATexture.h>
#include <memory>
#include <unordered_map>

namespace Lemonade
{
	using CitrusCore::ResourcePtr;
	using CitrusCore::Logger;

	class Material : public CitrusCore::AResource<Material>
	{
	public: 
		void Bind();

		ResourcePtr<AShaderProgram> GetShader() const;
		ResourcePtr<ATexture> GetTexture() const;
	protected: 
		virtual bool LoadResource(std::string path) override;
		virtual void UnloadResource() override{}
	private:
		ResourcePtr<AShaderProgram> m_shader;
		ResourcePtr<ATexture> m_texture;

		std::unordered_map<TextureType, std::shared_ptr<TextureData>> m_textures;

		TextureFilter m_textureFilter = TextureFilter::Linear;
		TextureClamp m_textureClamp = TextureClamp::Clamp;

		const std::string m_materialType = "type";
		const std::string m_materialShaders = "shaders";
		const std::string m_materialTextures = "textures";

		const std::vector<std::string> m_materialShaderTypes = {
			"fragment", 
			"vertex", 
			"geometry"
		};
	};
}