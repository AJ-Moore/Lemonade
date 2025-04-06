#pragma once

#include <Platform/Core/LObject.h>
#include <LCommon.h>
#include <Platform/Core/Time/Time.h>
#include <Platform/Core/Renderer/Pipeline/Renderer.h>
#include <memory>
#include <unordered_map>

namespace Lemonade
{
	using CitrusCore::UID;

	class LEMONADE_API Services : public LObject
	{
	public:
		Services() = default;
		static Services& GetInstance();
		static std::shared_ptr<Time> GetTime() { return GetInstance().m_time; }
		static std::shared_ptr<Renderer> GetRenderer() { return GetInstance().m_renderer; }
	
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
		std::shared_ptr<Renderer> m_renderer;
	};
}