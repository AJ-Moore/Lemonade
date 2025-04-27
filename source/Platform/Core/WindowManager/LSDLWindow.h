#pragma once 

#include "LCommon.h"
#include <SDL3/SDL_video.h>
#include <Platform/Core/WindowManager/AWindow.h>

namespace Lemonade
{
    class LEMONADE_API LSDLWindow : public AWindow
    {
    protected:
        virtual bool Init();

        virtual void ToggleFullscreen(bool value) override;
        SDL_Window* GetSDLWindow() { return m_window; }
    private:
        SDL_Window* m_window;
    };
}