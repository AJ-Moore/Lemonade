#pragma once

#include <LCommon.h>
#include <Resources/ResourceGroup.h>
#include <Resources/ResourceHandle.h>
#include <platform/core/Renderer/Materials/ATexture.h>

namespace Lemonade
{
	class LEMONADE_API GraphicsResources
	{
	public:
		std::shared_ptr<Citrus::ResourceHandle<ATexture>> GetTextureHandle(std::string path) const;
	private:
		CitrusCore::ResourceGroup<ATexture> m_textures;
	};
}