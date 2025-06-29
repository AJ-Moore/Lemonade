#include <Platform/Core/Renderer/Materials/Material.h>
#include <Platform/Vulkan/Materials/Texture.h>
#include <Platform/Vulkan/Renderer/LShader.h>
#include <Platform/Core/Resources/GraphicsResources.h>

namespace Lemonade {
    CitrusCore::ResourcePtr<ATexture> LGraphicsResources::GetTextureHandle(std::string path)
    {
        return m_textures.Get<Texture>(path);
    }

    CitrusCore::ResourcePtr<Material> LGraphicsResources::GetMaterialHandle(std::string path)
    {
        return m_materials.Get<Material>(path);
    }

    CitrusCore::ResourcePtr<AShader> LGraphicsResources::GetShaderHandle(std::string path)
    {
        return m_shaders.Get<LShader>(path);
    }
}