#include <LCommon.h>

#if defined(USING_SDL)
#include <Platform/Core/WindowManager/LSDLWindow.h>

namespace Lemonade
{
    bool LSDLWindow::Init()
    {
#if defined (RENDERER_OPENGL)
        m_window = SDL_CreateWindow(m_windowCaption.c_str(), m_windowRect.Width, m_windowRect.Height, SDL_WINDOW_OPENGL);
#elif defined(RENDERER_VULKAN)
        m_window = SDL_CreateWindow(m_windowCaption.c_str(), m_windowRect.Width, m_windowRect.Height, SDL_WINDOW_VULKAN);
#endif

        if (m_window == nullptr) {
            // In the case that the window could not be made...
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
            return 1;
        }
    }

    void LSDLWindow::ToggleFullscreen(bool value)
    {
        if (value == m_fullscreen)
        {
            return;
        }
    
        m_fullscreen = !m_fullscreen;
        
        if (m_fullscreen)
        {
            SDL_DisplayMode mode;
            SDL_GetDesktopDisplayMode(0, &mode);
            SDL_SetWindowSize(m_window, mode.w, mode.h);
            m_windowRect = Rect<uint32>(0, 0, mode.w, mode.h);
            m_geometryBuffer->setDimensions(glm::ivec2(mode.w, mode.h));
        
            SDL_SetWindowFullscreen(m_sdlWindow, SDL_WINDOW_FULLSCREEN);
        }
        else
        {
            SDL_SetWindowSize(m_sdlWindow, m_windowRect.Width, m_windowRect.Height);
            SDL_SetWindowFullscreen(m_sdlWindow, 0);
        }
    }
}
#endif 
