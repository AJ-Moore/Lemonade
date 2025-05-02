#include <LCommon.h>

#ifdef RENDERER_OPENGL

#include <GL/gl.h>
#include <Platform/OGL/Renderer/Pipeline/LViewport.h>

namespace Lemonade 
{
    void LViewport::SetViewport(int x, int y, int width, int height)
    {
        glViewport(x, y, width, height);
    }

    void LViewport::SetScissor(int x, int y, int width, int height)
    {
        glScissor(x, y, width, height);
    }

    void LViewport::SetDepthRange(float near, float far)
    {
        glDepthRange(near, far);
    }
}
#endif 