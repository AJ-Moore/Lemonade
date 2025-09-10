#include <Platform/Core/Services/GraphicsServices.h>
#include <Platform/Core/WindowManager/AWindow.h>
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
            m_window = SDL_CreateWindow(m_windowCaption.c_str(), m_windowRect.Width, m_windowRect.Height, SDL_WINDOW_VULKAN );
        #endif

        if (m_window == nullptr) {
            // In the case that the window could not be made...
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
            return false;
        }

        SDL_SetWindowBordered(m_window, m_windowBorder);
		SDL_SetWindowResizable(m_window, true);

		if (m_fullscreen)
		{
			SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
		}

        SDL_Event event;
        SDL_PollEvent(&event);

        // Will probably cause issues down line 
        SDL_SetWindowRelativeMouseMode(m_window,true);

        return true;
    }
    
    void LSDLWindow::Render() 
    {
        AWindow::Render();
        SDL_PumpEvents();
    }

    void LSDLWindow::Unload() 
    {
        if (m_window != nullptr)
        {
            SDL_DestroyWindow(m_window);
        }

        m_window = nullptr;
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
            const SDL_DisplayMode* mode = SDL_GetDesktopDisplayMode(0);
            SDL_SetWindowSize(m_window, mode->w, mode->h);
            m_windowRect = Rect<uint32>(0, 0, mode->w, mode->h);   
            SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
        }
        else
        {
            SDL_SetWindowSize(m_window, m_windowRect.Width, m_windowRect.Height);
            SDL_SetWindowFullscreen(m_window, 0);
        }
    }
}
#endif 
