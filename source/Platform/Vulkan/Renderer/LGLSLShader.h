#include "Platform/Core/Renderer/Materials/AShader.h"
#include <LCommon.h>
#if defined(RENDERER_VULKAN) || defined(RENDERER_OGL)

namespace Lemonade{
    class LEMONADE_API LGLSLShader : public AShader
    {
		virtual uint32 LoadAndCompileShader(const std::string& shaderFile, ShaderType shaderType) = 0;
    };
}

#endif 