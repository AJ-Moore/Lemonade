#include <LCommon.h>

#ifdef RENDERER_OPENGL

#include "Platform/Core/Services/Services.h"
#include "Platform/Core/WindowManager/AWindow.h"
#include <Platform/OGL/WindowManager/LWindow.h>

namespace Lemonade {
    void LWindow::Render() 
    {
        LSDLWindow::Render();
        SDL_GL_MakeCurrent(GetSDLWindow(), Services::GetContext()->GetContext() );
        SDL_GL_SwapWindow(GetSDLWindow());
    }
}

#endif 
