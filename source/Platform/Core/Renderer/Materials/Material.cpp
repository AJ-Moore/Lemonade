#include "Material.h"
#include <fstream>
#include <filesystem>
#include <Renderer/TextureType.h>
#include <Common.h>

namespace CraftyBlocks
{
    void Material::Bind()
    {
    }

    ShaderProgram* Material::GetShader() const
    {
        return m_shader.get();
    }

    Texture* Material::GetTexture() const
    {
        return nullptr;
    }

    bool Material::LoadResource(std::string path)
    {
		std::ifstream ifStream(path, std::ios::binary);

		if (!ifStream.good())
		{
			Log(Logger::WARN, "Unable to load material [%s].", path.c_str());
			return false;
		}

		std::filesystem::path fspath = path;
		int l2 = std::filesystem::file_size(fspath);

		ifStream.ignore((std::numeric_limits<std::streamsize>::max)());
		std::streamsize length = ifStream.gcount();
		ifStream.clear();
		ifStream.seekg(0, ifStream.beg);
		char* buffer = new char[length + 1];
		ifStream.read(buffer, length);
		buffer[length] = '\0';

		json data = json::parse(buffer);

		TextureBindType textureBindType = TextureBindType::Texture2D;

		json::iterator type = data.find(m_materialType);

		if (type != data.end())
		{
			std::string bindType = type.value().get<std::string>();
			textureBindType = TextureData::GetTextureBindType(bindType);
		}

		// Load Shaders 
		json::iterator shaders = data.find(m_materialShaders);
		if (shaders != data.end())
		{
			json shadersJson = shaders.value();

			for (const std::string& shader : m_materialShaderTypes)
			{
				json::iterator shaderIt = shadersJson.find(shader);

				if (shaderIt != shadersJson.end())
				{
					std::string shaderPath = shaderIt.value().get<std::string>();
					Log(Logger::VERBOSE, "Loading shader [%s]", shaderPath);
					ShaderType shaderType = ShaderProgram::GetShaderType(shaderPath);
				}
			}
		}

		// Load Textures
		json::iterator textures = data.find(m_materialTextures);

		if (textures != data.end())
		{
			json texturesJson = textures.value();

			for (json::iterator it = texturesJson.begin(); it != texturesJson.end(); ++it)
			{
				std::string textureTypeStr = it.key().c_str();
				Log(Logger::VERBOSE,"Loading texture [%s]", textureTypeStr.c_str());

			}
		}

		delete[] buffer;
    }
}
