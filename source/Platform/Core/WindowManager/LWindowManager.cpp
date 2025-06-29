#include "SDL3/SDL_init.h"
#include <LCommon.h>
#include <Platform/Core/Renderer/Materials/Material.h>
#include <Platform/OGL/WindowManager/LWindow.h>
#include <Platform/Core/WindowManager/LWindowManager.h>
#include <Geometry/Rect.h>
#include <Util/Logger.h>
#include <memory>

namespace Lemonade {

	LWindowManager::LWindowManager()
	{
		SetName("LWindowManager");
	}

	LWindowManager::~LWindowManager() {

	}

	std::shared_ptr<LWindow> LWindowManager::AddWindow(CitrusCore::Rect<uint32> WindowRect, std::string windowCaption) 
	{
		std::shared_ptr<LWindow> window = std::make_shared<LWindow>();
		window->m_windowRect = WindowRect; 
		window->m_windowCaption = windowCaption;
		AddWindow(window);
		return window;
	}

	void LWindowManager::AddWindow(std::shared_ptr<LWindow> window) 
	{
		m_windows.push_back(window);
		window->SetParent(this);

		if (HasbeenInitialised())
		{
			window->Init();
		}
	}

	/** Initialises all the windows and the GL Context.*/
	bool LWindowManager::Init()
	{
		// Create the GL Context, using the default window.
		//this->glContext = SDL_GL_CreateContext(m_defaultWindow->m_sdlWindow);

		// Initialise glew 
		//glewInit();

		#ifdef USING_SDL
		{
			//if (!SDL_Init(SDL_INIT_VIDEO)) {
			//	Logger::Log(Logger::ERROR, "Failed to initialise SDL.");
			//	return false;
			//}
		}
		#endif

		if (!Load())
		{
			Logger::Log(Logger::ERROR, "Loading default window failed.");
			return false;
		}

		//Initialise these at some other point ???
		Logger::Log(Logger::INFO, "Initialising window manager, windows.");

		for (auto& window : m_windows)
		{
			if (!window->Init())
			{
				//We should do something more useful here probably?!
				Logger::Log(Logger::ERROR, "Initialising window failed.");
			}
		}

		return true; 
	}

	bool LWindowManager::Load()
	{
		bool _return = true;

		m_defaultWindow = std::make_shared<LWindow>();
		
		if (!m_defaultWindow->Load("config.cfg"))
		{
			return false;
		}

		AddWindow(m_defaultWindow);

		for (const auto& window : m_windows)
		{
			if (!window->Load())
			{
				return false;
			}
		}

		return _return;
	}

	void LWindowManager::Update()
	{
		for (const auto& window : m_windows)
		{
			window->Update();
		}
	}

	void LWindowManager::Unload()
	{
		for (const auto& window : m_windows)
		{
			window->Unload();
		}
	}

	void LWindowManager::Render()
	{
		for (auto& window : m_windows)
		{
			m_activeWindow = window.get();
			window->Render();
		}
	}
}
