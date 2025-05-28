
#include <Platform/Core/Renderer/Pipeline/LRenderer.h>
#include "Services/Services.h"
#include <Platform/Core/WindowManager/LWindowManager.h>
#include <Platform/Core/Services/GraphicsServices.h>
#include <memory>

namespace Lemonade
{
    GraphicsServices& GraphicsServices::GetInstance()
    {
        static GraphicsServices instance;
        return instance;
    }

    bool GraphicsServices::Init()
    {
        m_time = std::make_shared<Time>();
        m_renderer = std::make_shared<LRenderer>();
        m_context = std::make_shared<LGraphicsContext>();
        m_windowManager = std::make_shared<LWindowManager>();

        // Note I've opted to keep a fairly rigid initilisation order to keep debugging simpler and avoid bugs and complexities that may arise as a result of lazy initisation.
        AddService(m_time);
        AddService(m_renderer);

        #if not defined (RENDERER_OPENGL)
            AddService(m_context);
        #endif

        AddService(m_windowManager);

        #if defined (RENDERER_OPENGL)
            // Context must be initialised after window manager in OpenGL.
            AddService(m_context);     
        #endif

        CitrusCore::Services::Init();
        return true;
    }
}
