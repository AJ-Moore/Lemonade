#pragma once

#include "Base/Object.h"
#include <LCommon.h>
#include <Resources/ResourceGroup.h>
#include <Resources/ResourceHandle.h>
#include <Platform/Core/Renderer/Materials/ATexture.h>

namespace Lemonade
{
	class LEMONADE_API GraphicsResources : CitrusCore::Object
	{
	public:
		std::shared_ptr<CitrusCore::ResourceHandle<ATexture>> GetTextureHandle(std::string path) const;
	private:
		CitrusCore::ResourceGroup<ATexture> m_textures;
	};
}