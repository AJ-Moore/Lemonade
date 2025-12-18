#pragma once 

#include <Platform/Core/Renderer/Materials/ASampler.h>
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
#include <unordered_set>
#include <vector>

namespace Lemonade
{
	using CitrusCore::ResourcePtr;
	using CitrusCore::Logger;

	enum class LEMONADE_API TextureStatus : int {
		NotProvided = 0,
		Loaded      = 1,
		Missing     = -1
	};	

	class LEMONADE_API Material : public CitrusCore::AResource<Material>
	{
		struct TextureMap : public std::unordered_map<TextureType, std::shared_ptr<TextureData>> {
			TextureMap() = default;
			TextureMap(const TextureMap&) = delete;
			TextureMap& operator=(const TextureMap&) = delete;
		};

		struct SamplerMap : public std::unordered_set<std::shared_ptr<ASampler>> {
			SamplerMap() = default;
			SamplerMap(const SamplerMap&) = delete;
			SamplerMap& operator=(const SamplerMap&) = delete;
		};	

	public: 
		Material() = default;
		void Bind();

		std::shared_ptr<AShaderProgram> GetShader() const;
		ResourcePtr<ATexture> GetTexture() const;

		const TextureMap& GetTextures() const noexcept { return m_textures; }
		const SamplerMap& GetSamplers() const noexcept { return m_samplers; }
		glm::vec4 GetBaseColour() const;
		void SetBaseColour(glm::vec4 colour) { m_baseColour = colour;}
		TextureStatus GetTextureStatus(TextureType textureType) const;
		int GetBindLocation(TextureType);

		/// Manually load a texture into this material. 
		void LoadTexture(TextureType textureType, std::string path, int bindIndex);
		void Save();

		static const int INVALID_BIND_LOCATION = -1;
	protected: 
		virtual bool LoadResource(std::string path) override;
		virtual void UnloadResource() override;
	private:
		std::unordered_map<TextureType, TextureStatus> m_textureStatus;

		std::shared_ptr<AShaderProgram> m_shader;
		ResourcePtr<ATexture> m_texture;
		glm::vec4 m_baseColour = glm::vec4(1,1,1,1);
		bool m_hasBaseColour = false; 

		TextureMap m_textures;
		SamplerMap m_samplers;

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