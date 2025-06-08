#include "Platform/Core/Renderer/Materials/AShader.h"
#include "Resources/ResourceHandle.h"
#include <Platform/Core/Renderer/Materials/AShaderProgram.h>
namespace Lemonade 
{
    void AShaderProgram::AddShader(CitrusCore::ResourcePtr<AShader> shader)
    {
        m_shaders.push_back(shader);
    }
}