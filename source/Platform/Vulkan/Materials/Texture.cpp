#include <LCommon.h>

#ifdef RENDERER_VULKAN
#include <Platform/Vulkan/Materials/Texture.h>
#include <Platform/Core/Renderer/Materials/TextureType.h>

namespace Lemonade {
	// Native Texture Format Lookup 
	const std::unordered_map<TextureFormat, int> TextureData::m_nativeTextureFormatLookup = {
	};

	int TextureData::GetNativeTextureFormat(const TextureFormat& format)
	{
		std::unordered_map<TextureFormat, int>::const_iterator iter = m_nativeTextureFormatLookup.find(format);
		
		if (iter != m_nativeTextureFormatLookup.end())
		{
			return iter->second;
		}

		return -1;
	}
}

#endif 