#pragma once

#include "Platform/Core/Services/LService.h"
#include "Platform/Core/WindowManager/LWindowManager.h"
#include "Services/Services.h"
#include <Platform/Core/LObject.h>
#include <LCommon.h>
#include <Platform/Core/Time/Time.h>
#include <Platform/Core/Renderer/Pipeline/LRenderer.h>
#include <memory>

#if defined(RENDERER_OGL)
#include <Platform/OGL/Renderer/Core/LGraphicsContext.h>
#elif defined(RENDERER_VULKAN)
#include <Platform/Vulkan/Renderer/Core/LGraphicsContext.h>
#endif

namespace Lemonade
{
	class LEMONADE_API GraphicsServices : public CitrusCore::Services
	{
		friend class LemonadeRE;
	public:
		GraphicsServices() = default;
		static GraphicsServices& GetInstance();
		static std::shared_ptr<Time> GetTime() { return GetInstance().m_time; }
		static std::shared_ptr<LRenderer> GetRenderer() { return GetInstance().m_renderer; }
		static std::shared_ptr<LGraphicsContext> GetContext() { return GetInstance().m_context; }
		static std::shared_ptr<LWindowManager> GetWindowManager() { return GetInstance().m_windowManager; }
	
		// Delete the methods we don't want to allow
		GraphicsServices(GraphicsServices const&) = delete; // Copy constructor
		void operator=(GraphicsServices const&) = delete; // Copy assignment operator

	protected:
		virtual bool Init() override;

	private:
		std::shared_ptr<Time> m_time;
		std::shared_ptr<LRenderer> m_renderer;
		std::shared_ptr<LGraphicsContext> m_context;
		std::shared_ptr<LWindowManager> m_windowManager;
	};
}