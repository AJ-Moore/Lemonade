#pragma once
#include <LCommon.h>
#ifdef RENDERER_VULKAN
#include <unordered_map>

namespace Lemonade
{
	class LEMONADE_API LShader : public LGLSLShader
	{
		friend class UMaterial;
		friend class URenderBlock;
	public:
		virtual void load();
		virtual uint32 LoadAndCompileShader(const std::string& shaderFile, ShaderType shaderType);
	};
}
#endif