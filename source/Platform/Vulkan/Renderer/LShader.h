#pragma once
#include <LCommon.h>
#include <Platform/Vulkan/Renderer/LSpirVShader.h>

#ifdef RENDERER_VULKAN

namespace Lemonade
{
	class LEMONADE_API LShader : public LSpirVShader 
	{
		friend class UMaterial;
		friend class URenderBlock;
	public:
		virtual void load();
		virtual uint32 LoadAndCompileShader(const std::string& shaderFile, ShaderType shaderType);

	};
}
#endif