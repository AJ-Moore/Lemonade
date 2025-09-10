
#include <Platform/Core/Renderer/Materials/AShader.h>
#include <Platform/Vulkan/Renderer/LGLSLShader.h>
#include <fstream>
#include <shaderc/shaderc.h>
#include <stdexcept>
#include <vector>

namespace Lemonade
{
    bool LGLSLShader::LoadResource(std::string path)
    {
        // TODO check whether -> should compile shaders, check MD5, Recompile shader flags etc...

        size_t underscorePos = path.rfind('_');
        size_t dotPos = path.find('.', underscorePos);
        std::string shaderTypeStr = path.substr(underscorePos + 1, dotPos - underscorePos - 1);

        SetShaderType(GetShaderTypeForString(shaderTypeStr));

        std::ifstream file(path);

        if (!file.is_open()) 
        {
            // Todo handle this better!? on Failure we want to use a default material.
            throw std::runtime_error("Failed to open shader file");
        }

        std::string shader =  std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        shaderc_shader_kind kind = GetShaderKind();

        bool isHlslShader = true;

        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetOptimizationLevel(shaderc_optimization_level_performance);

        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
        shaderc::SpvCompilationResult result;

        if (isHlslShader)
        {
            options.SetSourceLanguage(shaderc_source_language_hlsl);
            result = compiler.CompileGlslToSpv(shader, kind, path.c_str(), "main", options);
        }
        else
        {
            result = compiler.CompileGlslToSpv(shader, kind, path.c_str(), options);
        }

        if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
            // TODO handle more grace like
            throw std::runtime_error(result.GetErrorMessage());
        }
        std::vector<uint32_t> shaderSource = {result.cbegin(), result.cend()};
        WriteSpirvToFile(shaderSource, path);
        OnShaderCompiled(shaderSource);
        return true;
    }

    shaderc_shader_kind LGLSLShader::GetShaderKind() 
    {
        // Change this for a lookup, thanks.
        switch (GetShaderType()) {
            case Lemonade::ShaderType::Fragment:
                return shaderc_fragment_shader;
            case Lemonade::ShaderType::Vertex:
                return shaderc_vertex_shader;
            default:
                throw std::runtime_error("Unsupported shader type");
        }
    }

    void LGLSLShader::UnloadResource()
    {

    }

    void LGLSLShader::WriteSpirvToFile(const std::vector<uint32_t>& spirv, const std::string& path)
    {
        std::string spirExt = ".spirv";
        std::ofstream file(path+spirExt, std::ios::binary);
        file.write(reinterpret_cast<const char*>(spirv.data()), spirv.size() * sizeof(uint32_t));
    }
}