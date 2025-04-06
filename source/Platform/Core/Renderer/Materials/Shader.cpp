#include "Shader.h"
#include <filesystem>

namespace Lemonade {
    bool Shader::LoadResource(std::string path)
    {
        // Try get meta 
        // Check if MD5 matches compiled shader (if shader source path exists)? Compile Shaders flag(DebuG?)?
        std::filesystem::path fsPath;
    
        // Else recompile shader.
        return false;
    }
    
    uint32 Shader::LoadAndCompileShader(const std::string& shaderFile, ShaderType shaderType)
    {
        return uint32();
    }
    
}
