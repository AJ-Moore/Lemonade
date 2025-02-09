#include "Shader.h"
#include <filesystem>

bool CraftyBlocks::Shader::LoadResource(std::string path)
{
    // Try get meta 
    // Check if MD5 matches compiled shader (if shader source path exists)? Compile Shaders flag(DebuG?)?
    std::filesystem::path fsPath;

    // Else recompile shader.
    return false;
}

uint32 CraftyBlocks::Shader::LoadAndCompileShader(const std::string& shaderFile, ShaderType shaderType)
{
    return uint32();
}
