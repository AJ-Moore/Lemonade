#pragma once

#include "Platform/Core/WindowManager/LWindowManager.h"
#include "Platform/OGL/Renderer/Core/LGraphicsContext.h"
#include <Platform/Core/LObject.h>
#include <LCommon.h>
#include <Platform/Core/Time/Time.h>
#include <Platform/Core/Renderer/Pipeline/LRenderer.h>
#include <memory>
#include <unordered_map>

#if defined(RENDERER_OGL)

#elif defined(RENDERER_VULKAN)
#endif

namespace Lemonade
{
	using CitrusCore::UID;

	class LEMONADE_API Services : public LObject
	{
	public:
		Services() = default;
		static Services& GetInstance();
		static std::shared_ptr<Time> GetTime() { return GetInstance().m_time; }
		static std::shared_ptr<LRenderer> GetRenderer() { return GetInstance().m_renderer; }
		static std::shared_ptr<LGraphicsContext> GetContext() { return GetInstance().m_context; }
		static std::shared_ptr<LWindowManager> GetWindowManager() { return GetInstance().m_windowManager; }
	
		// Delete the methods we don't want to allow
		Services(Services const&) = delete; // Copy constructor
		void operator=(Services const&) = delete; // Copy assignment operator
	protected:
		virtual bool Init();
		virtual void Unload();
		virtual void Update();
		virtual void Render();

	private:
		std::unordered_map<UID, std::shared_ptr<LService>> m_services;

		bool m_bRunning = false;
		std::shared_ptr<Time> m_time;
		std::shared_ptr<LRenderer> m_renderer;
		std::shared_ptr<LGraphicsContext> m_context;
		std::shared_ptr<LWindowManager> m_windowManager;
	};
}