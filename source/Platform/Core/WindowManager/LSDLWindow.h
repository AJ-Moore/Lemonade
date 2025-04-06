#include "LCommon.h"
#include <SDL3/SDL_video.h>
#include <Platform/Core/WindowManager/AWindow.h>

namespace Lemonade
{
    class LEMONADE_API LSDLWindow : public AWindow
    {
    protected:
        virtual bool Init();
    private:
        SDL_Window* m_window;
    };
}