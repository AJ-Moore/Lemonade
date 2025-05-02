#pragma once 
#include <LCommon.h>

#ifdef RENDERER_OPENGL
#include <GLES2/gl2.h>
#include <Platform/Core/Renderer/Materials/AShaderProgram.h>

namespace Lemonade
{
    class LEMONADE_API LShaderProgram : public AShaderProgram
    {
    public:
		GLint GetProgram() const { return m_shaderProgram; }
    private:
        GLint m_shaderProgram = 0;
    };
}

#endif 