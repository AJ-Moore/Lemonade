
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

    bool Services::Init()
    {
        m_time = std::make_shared<Time>();
        m_renderer = std::make_shared<LRenderer>();
        m_context = std::make_shared<LGraphicsContext>();
        m_windowManager = std::make_shared<LWindowManager>();

        m_services.emplace(m_time->GetUID(), m_time);
        m_services.emplace(m_renderer->GetUID(), m_renderer);
        m_services.emplace(m_context->GetUID(), m_context);
        m_services.emplace(m_windowManager->GetUID(), m_windowManager);

        for (auto& service : m_services)
        {
            service.second->Init();
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
