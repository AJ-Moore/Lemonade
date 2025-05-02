
#include <Platform/Core/WindowManager/LWindowManager.h>
#include <Platform/Core/Services/Services.h>
#include <memory>

namespace Lemonade
{
    Services& Services::GetInstance()
    {
        static Services instance;
        return instance;
    }

    void Services::AddService(std::shared_ptr<LService> service)
    {
        m_services.emplace(service->GetUID(), service);
        m_awaitingInitialisation.push(service);
    }

    bool Services::Init()
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


        while (!m_awaitingInitialisation.empty())
        {
            std::shared_ptr<LService> service = m_awaitingInitialisation.front();
            m_awaitingInitialisation.pop();
            if (service->Init())
            {
                service->m_bDoneInit = true;
            }
        }

        return true;
    }

    void Services::Unload()
    {
        for (auto& service : m_services)
        {
            service.second->Unload();
        }
    }

    void Services::Update()
    {
        for (auto& service : m_services)
        {
            service.second->Update();
        }
    }

    void Services::Render()
    {
        for (auto& service : m_services)
        {
            service.second->Render();
        }
    }
}
