#include "Material.h"
#include "Platform/Core/Renderer/Materials/AShader.h"
#include "Platform/Core/Renderer/Materials/ATexture.h"
#include "Platform/Core/Services/GraphicsServices.h"
#include "Resources/ResourceHandle.h"
#include <fstream>
#include <filesystem>
#include <Platform/Core/Renderer/Materials/TextureUtils.h>
#include <Platform/Core/Renderer/Materials/TextureType.h>
#include <LCommon.h>

#include <memory>
#include <nlohmann/json.hpp>
#include <utility>

#include <Platform/Vulkan/Renderer/LShaderProgram.h>

namespace Lemonade
{
	using namespace nlohmann;

    void Material::Bind()
    {
    }

	void Material::UnloadResource()
	{ 
		throw "TODO implement unload of resources.";
	}

    std::shared_ptr<AShaderProgram> Material::GetShader() const
    {
        return m_shader;
    }

    ResourcePtr<ATexture> Material::GetTexture() const
    {
        return m_texture;
    }

    bool Material::LoadResource(std::string path)
    {
		std::ifstream ifStream(path, std::ios::binary);

		if (!ifStream.good())
		{
			Logger::Log(Logger::WARN, "Unable to load material [%s].", path.c_str());
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

		// Get shader program.
		m_shader = std::make_shared<LShaderProgram>();

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
					Logger::Log(Logger::VERBOSE, "Loading shader [%s]", shaderPath.c_str());
					ResourcePtr<AShader> shader = GraphicsServices::GetGraphicsResources()->GetShaderHandle(shaderPath);
					m_shader->AddShader(shader);
				}
			}
		}

		// Load Textures
		json::iterator textures = data.find(m_materialTextures);
		int bindLocation = 0;// Implicit bind location fallback.

		if (textures != data.end())
		{
			json texturesJson = textures.value();

			for (json::iterator it = texturesJson.begin(); it != texturesJson.end(); ++it)
			{
				std::string textureTypeStr = it.key().c_str();
				Logger::Log(Logger::VERBOSE,"Loading texture [%s]", textureTypeStr.c_str());

				TextureType textureType = TextureData::GetTextureType(textureTypeStr);

				if (textureType == TextureType::None)
				{
					continue;
				}

				json textureData = it.value();
				json::iterator bindloc = textureData.find("bindlocation");

				int bindIndex = 0;

				if (bindloc != textureData.end())
				{
					bindIndex = bindloc.value().get<int>();
					bindLocation = bindIndex;
				}
				else
				{
					bindIndex = bindLocation++;
				}

				// For ref - we want to load a texture regardless of whether we believe the path to be correct! 
				json::iterator texturepath = textureData.find("path");
				std::string path;

				if (texturepath != textureData.end())
				{
					path = texturepath.value().get<std::string>();
				}

				ResourcePtr<ATexture> texture = GraphicsServices::GetGraphicsResources()->GetTextureHandle(path);
				std::shared_ptr<TextureData> textureDataPtr = std::make_shared<TextureData>(textureType, texture, bindLocation);
				m_textures.insert(std::make_pair(textureType, textureDataPtr));
			}
		}

		delete[] buffer;
		return true;
    }
}
