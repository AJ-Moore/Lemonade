#include <Platform/Core/Resources/GraphicsResources.h>

namespace Lemonade {
    CitrusCore::ResourcePtr<ATexture> LGraphicsResources::GetTextureHandle(std::string path)
    {
        return m_textures.Get(path);
    }

    CitrusCore::ResourcePtr<Material> LGraphicsResources::GetMaterialHandle(std::string path)
    {
        return m_materials.Get(path);
    }
}