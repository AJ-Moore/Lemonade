#pragma once 

#include <LCommon.h>
#include <SDL3/SDL_video.h>
#include <Platform/Core/WindowManager/AWindow.h>

namespace Lemonade
{
    class LEMONADE_API LSDLWindow : public AWindow
    {
    public:
        SDL_Window* GetSDLWindow() { return m_window; }

        /// Sets mouse as hidden and constrained to the window.
        virtual void SetRelativeMouseMode(bool value) override;
    protected:
        virtual bool Init() override;
        virtual void Unload() override;
        virtual void Render() override;

        virtual void ToggleFullscreen(bool value) override;
    private:
        SDL_Window* m_window;
    };
}