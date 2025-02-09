#pragma once 

#include <Renderer/ShaderProgram.h>
#include <Renderer/Texture.h>
#include <memory>

namespace CraftyBlocks
{
	class Material : public Resource
	{
	public: 
		void Bind();

		ShaderProgram* GetShader() const;
		Texture* GetTexture() const;
	protected: 
		virtual bool LoadResource(std::string path) override;
	private:
		std::unique_ptr<ShaderProgram> m_shader;
		std::shared_ptr<Texture> m_texture;

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