#pragma once

#include <Platform/Core/Renderer/Materials/AShader.h>
#include <LCommon.h>
#include <shaderc/shaderc.hpp>

#if defined(RENDERER_VULKAN) || defined(RENDERER_OGL)

namespace Lemonade{
    class LEMONADE_API LGLSLShader : public AShader
    {
	public:
		virtual ~LGLSLShader(){}
		virtual void Bind()override{}
    protected:
		virtual bool LoadResource(std::string path) override;
		virtual void UnloadResource() override;
		virtual void OnShaderCompiled(std::vector<uint32_t>& code){};
	private:
		void WriteSpirvToFile(const std::vector<uint32_t>& spirv, const std::string& path);
		shaderc_shader_kind GetShaderKind();
    };
}

#endif 