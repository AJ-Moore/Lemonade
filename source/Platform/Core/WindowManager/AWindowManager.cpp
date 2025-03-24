
#include <LCommon.h>
#include <Core/WindowManager/AWindowManager.h>
#include <Geometry/Rect.h>

namespace Lemonade {

	LWindowManager::LWindowManager()
	{
		SetName("AWindowManager");
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
		m_windows.emplace(window->GetUID().GetID(), window);
		window->SetParent(this);

		if (HasbeenInitialised())
		{
			window->Init();
		}
	}

	/** Initialises all the windows and the GL Context.*/
	bool LWindowManager::Init()
	{
#ifdef RENDERER_OPENGL
#if __APPLE__
		const char* glsl_version = "#version 150";
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
		const char* glsl_version = "#version 460";
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
#endif
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		// Create the GL Context, using the default window.
		this->glContext = SDL_GL_CreateContext(m_defaultWindow->m_sdlWindow);

		// Initialise glew 
		glewInit();

		// Set any initial graphics state.
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#elif defined(RENDERER_PS5)
		LogError("TODO - ps5 graphics initialisation");
#elif defined(RENDERER_VULKAN)		
		m_vulkanDevice.init();
		LogError("TODO - vulkan graphics initialisation");
#else 
		LogError("Graphics platform not supported?");
#endif

		//Initialise these at some other point ???
		LogInfo("Initialising window manager, windows.");
		std::map<uint32, LWindow*>::iterator iter; 
		for (auto& window : m_windows) {
			if (!window.second->init())
			{
				//We should do something more useful here probably?!
				LogError("Initialising window failed.");
			}
		}

		m_bDoneInit = true;
		return true; 
	}

	bool LWindowManager::load() {
		bool _return = true;

		this->defaultWindow = new LWindow();
		
		if (!this->defaultWindow->load("config.cfg"))
		{
			return false;
		}

		this->addWindow(this->defaultWindow);

		std::map<uint32, LWindow*>::iterator iter;
		for (iter = this->m_windows.begin(); iter != this->m_windows.end(); iter++) {
			if (!iter->second->load()) {
				return false;
			}
		}
		return _return;
	}

	void LWindowManager::update() {

		std::map<uint32, LWindow*>::iterator iter;
		for (iter = this->m_windows.begin(); iter != this->m_windows.end(); iter++) {
			iter->second->update();
		}
	}

	void LWindowManager::unload() {
#ifdef RENDERER_OPENGL
		SDL_GL_DeleteContext(this->glContext);
		
		// Destroy windows
		for (auto& window : m_windows)
		{
			SDL_DestroyWindow(window.second->m_sdlWindow);
		}
#endif
	}

	void LWindowManager::render() {

#ifdef RENDERER_OPENGL
		SDL_GL_MakeCurrent(m_defaultWindow->m_sdlWindow, this->glContext);
		SDL_GL_SwapWindow(m_defaultWindow->m_sdlWindow);
#endif

		for (auto& window : m_windows)
		{
#ifdef RENDERER_OPENGL
			SDL_GL_MakeCurrent(window.second->m_sdlWindow, this->glContext);
			SDL_GL_SwapWindow(window.second->m_sdlWindow);
#endif
			window.second->render();
		}
	}


}