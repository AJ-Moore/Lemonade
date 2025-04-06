#pragma once 

#include "Resources/AResource.h"
#include "Resources/ResourceHandle.h"
#include "Util/Logger.h"
#include <Platform/Core/Renderer/Materials/ShaderProgram.h>
#include <Platform/Core/Renderer/Materials/ATexture.h>
#include <memory>

namespace Lemonade
{
	using CitrusCore::ResourcePtr;
	using CitrusCore::Logger;

	class Material : public CitrusCore::AResource<Material>
	{
	public: 
		void Bind();

		ResourcePtr<ShaderProgram> GetShader() const;
		ResourcePtr<ATexture> GetTexture() const;
	protected: 
		virtual bool LoadResource(std::string path) override;
	private:
		ResourcePtr<ShaderProgram> m_shader;
		ResourcePtr<ATexture> m_texture;
		//std::unique_ptr<ShaderProgram> m_shader;
		//std::shared_ptr<Texture> m_texture;

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