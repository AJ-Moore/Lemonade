
#include <Platform/Core/Services/Services.h>

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
        m_renderer = std::make_shared<Renderer>();
        //m_window = std::make_shared<Window>();

        m_services.emplace(m_time->GetUID(), m_time);
        m_services.emplace(m_renderer->GetUID(), m_renderer);
        //m_services.emplace(m_window->GetUID(), m_window);

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
