#include <Platform/Core/Components/ModelLoader/LModelMesh.h>
#include <Platform/Core/Renderer/Materials/Material.h>
#include <Platform/Vulkan/Materials/Texture.h>
#include <Platform/Vulkan/Renderer/LShader.h>
#include <Platform/Core/Resources/GraphicsResources.h>

namespace Lemonade {
    CitrusCore::ResourcePtr<ATexture> LGraphicsResources::GetTextureHandle(const std::string& path, std::function<void(ATexture*)> construct)
    {
        return m_textures.Get<Texture>(path);
    }

    CitrusCore::ResourcePtr<Material> LGraphicsResources::GetMaterialHandle(const std::string& path, std::function<void(Material*)> construct)
    {
        return m_materials.Get<Material>(path);
    }

    CitrusCore::ResourcePtr<Material> LGraphicsResources::GetMaterialMasqueradeFromBase(const std::string& basePath,const std::string& path, std::function<void(Material*)> construct)
    {
        return m_materials.Get<Material>(basePath, path);
    }

    CitrusCore::ResourcePtr<AShader> LGraphicsResources::GetShaderHandle(const std::string& path, std::function<void(AShader*)> construct)
    {
        return m_shaders.Get<LShader>(path);
    }

    CitrusCore::ResourcePtr<LModelResource> LGraphicsResources::GetModelHandle(const std::string& path, std::function<void(LModelResource*)> construct)
    {
        return m_models.Get<LModelResource>(path, construct);
    }
}