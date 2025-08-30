#pragma once 

#include <LCommon.h>
#include <Platform/Core/Renderer/Materials/TextureType.h>
#include <Resources/AResource.h>
#include <Resources/ResourceHandle.h>
#include <Util/Logger.h>
#include <Platform/Core/Renderer/Materials/AShaderProgram.h>
#include <Platform/Core/Renderer/Materials/ATexture.h>
#include <glm/fwd.hpp>
#include <memory>
#include <unordered_map>

namespace Lemonade
{
	using CitrusCore::ResourcePtr;
	using CitrusCore::Logger;

	class LEMONADE_API Material : public CitrusCore::AResource<Material>
	{
		struct TextureMap : public std::unordered_map<TextureType, std::shared_ptr<TextureData>> {
			TextureMap() = default;
			TextureMap(const TextureMap&) = delete;
			TextureMap& operator=(const TextureMap&) = delete;
		};

	public: 
		Material() = default;
		void Bind();

		std::shared_ptr<AShaderProgram> GetShader() const;
		ResourcePtr<ATexture> GetTexture() const;

		const TextureMap& GetTextures() const noexcept { return m_textures; }
		glm::vec4 GetBaseColour() const { return m_baseColour; }

		/// Manually load a texture into this material. 
		void LoadTexture(TextureType textureType, std::string path, int bindIndex);
		void Save();
	protected: 
		virtual bool LoadResource(std::string path) override;
		virtual void UnloadResource() override;
	private:
		std::shared_ptr<AShaderProgram> m_shader;
		ResourcePtr<ATexture> m_texture;
		glm::vec4 m_baseColour = glm::vec4(1);

		TextureMap m_textures;

		TextureFilter m_textureFilter = TextureFilter::Linear;
		TextureClamp m_textureClamp = TextureClamp::Clamp;

		const std::string m_materialType = "type";
		const std::string m_materialShaders = "shaders";
		const std::string m_materialTextures = "textures";
		const std::string m_baseColourString = "basecolour";

		const std::vector<std::string> m_materialShaderTypes = {
			"fragment", 
			"vertex", 
			"geometry"
		};
	};
}