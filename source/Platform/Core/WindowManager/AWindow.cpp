

#include <Core/WindowManager/AWindow.h>
#include <LCommon.h>
#include "../../Util/Config.h"
#include "../../Core/SceneManager/UScene.h"
#include "../../Core/UServiceLocator.h"

#ifdef RENDERER_PS5 
#include <Renderer/RenderTargets/Platform/PS5/URenderTarget_ps5.h>
#include <_ThirdParty/imgui/imgui_libfont.h>
#include <Util/Platform/PS5/system.h>
#include <mat.h>
#include <agc/drawcommandbuffer.h>
#include <video_out.h>
#include <_ThirdParty/imgui/prospero/imgui_impl_agc.h>
#elif defined(RENDERER_VULKAN)
#include <Renderer/Platform/Vulkan/URenderTarget_vulkan.h>
#include <SDL2/SDL_vulkan.h>
#else 
#include <Renderer/Platform/OGL/URenderTarget_ogl.h>
#endif


namespace Lemonade {
	LWindow::LWindow() 
	{

	}

	LWindow::~LWindow() {
	}

	UCamera* LWindow::GetActiveCamera() const 
	{
		return m_viewCamera;
	}

	void LWindow::setActiveCamera(UCamera* Camera){
		m_viewCamera = Camera;
	}

	void LWindow::removeCamera(SharedPtr<UCamera> camera)
	{
		if (m_cameras.find(camera->UUID.getID()) != m_cameras.end())
		{
			m_cameras.erase(camera->UUID.getID());
		}
	}

#ifdef RENDERER_PS5
	void* allocMem(size_t	size, void* userData)
	{
		return sceLibcMspaceMalloc((SceLibcMspace)userData, size);
	}

	void freeMem(void* ptr, void* userData)
	{
		sceLibcMspaceFree((SceLibcMspace)userData, ptr);
	}
#endif

	bool LWindow::Init() 
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
#endif 

		// Setup Platform/Renderer bindings
#ifdef RENDERER_OPENGL
		ImGui_ImplSDL2_InitForOpenGL(m_sdlWindow, m_windowManager->getContext());
		ImGui_ImplOpenGL3_Init(glsl_version);
#elif defined(RENDERER_VULKAN)
		ImGui_ImplSDL2_InitForVulkan(m_sdlWindow);
#elif defined(RENDERER_PS5)
		
		ImFont* font = ImGuiLibFont::AddSystemFont(io.Fonts, (float)48);
		ImGui_ImplSampleUtil_Init(nullptr, nullptr, new sce::PS5Utils::System::UserIdManager(), false);


		LogError("Imgui not initialismed for target platform.");
#endif

		initFramebuffer();

		auto input = UServiceLocator::getInstance()->getInputManager();
#ifdef RENDERER_OPENGL
		input->addBind(SDL_SCANCODE_F, &LWindow::ToggleFullscreen, this);
#endif

		return true;
	}

	void LWindow::InitFramebuffer()
	{
		LogInfo("Initialising framebuffer.");
		Rect<uint32> windowRect = m_windowRect;

		LogGLErrors();
		m_geometryBuffer = std::make_shared<URenderTarget>(glm::vec2(windowRect.Width, windowRect.Height));
		LogGLErrors();

		m_geometryBuffer->setColourAttachments({ 
			UColourAttachment::UP_COLOR_ATTACHMENT0, 
			UColourAttachment::UP_COLOR_ATTACHMENT1, 
			UColourAttachment::UP_COLOR_ATTACHMENT2, 
			UColourAttachment::UP_COLOR_ATTACHMENT3, 
			UColourAttachment::UP_COLOR_ATTACHMENT4,
			UColourAttachment::UP_COLOR_ATTACHMENT5,
			UColourAttachment::UP_COLOR_ATTACHMENT6 }, true);

		m_geometryBuffer->addDepthAttachment();
	}

	void AWindow::ToggleFullscreen(float value)
	{
		if (!value)
		{
			return;
		}

#if defined(USING_SDL)
		m_fullscreen = !m_fullscreen;

		if (m_fullscreen)
		{
			SDL_DisplayMode mode;
			SDL_GetDesktopDisplayMode(0, &mode);
			SDL_SetWindowSize(m_sdlWindow, mode.w, mode.h);
			m_windowRect = Rect<uint32>(0, 0, mode.w, mode.h);
			m_geometryBuffer->setDimensions(glm::ivec2(mode.w, mode.h));

			SDL_SetWindowFullscreen(m_sdlWindow, SDL_WINDOW_FULLSCREEN);
		}
		else
		{
			SDL_SetWindowSize(m_sdlWindow, m_windowRect.Width, m_windowRect.Height);
			SDL_SetWindowFullscreen(m_sdlWindow, 0);
		}
#endif
	}

	bool LWindow::load() 
	{
		m_renderer = UServiceLocator::getInstance()->getRenderer();

#ifdef RENDERER_OPENGL
		if (m_sdlWindow != nullptr)
		{
			return true;
		}

		m_sdlWindow = SDL_CreateWindow(m_windowCaption.c_str(), m_windowRect.X, m_windowRect.Y, m_windowRect.Width, m_windowRect.Height, SDL_WINDOW_OPENGL);
#elif defined(RENDERER_VULKAN)
		m_sdlWindow = SDL_CreateWindow(m_windowCaption.c_str(), m_windowRect.X, m_windowRect.Y, m_windowRect.Width, m_windowRect.Height, SDL_WINDOW_VULKAN);

		VkApplicationInfo app = {
			VK_STRUCTURE_TYPE_APPLICATION_INFO, // sType
			nullptr,                            // pNext
			m_windowCaption.c_str(),            // pApplicationName
			0,                                  // applicationVersion
			"Upsilon",				            // pEngineName
			0,                                  // engineVersion
			VK_API_VERSION_1_3                  // apiVersion
		};

		//SDL_Vulkan_GetInstanceExtensions(m_sdlWindow, &(demo->enabled_extension_count), demo->extension_names);
		VkInstanceCreateInfo inst_info = {};
		inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		inst_info.pNext = nullptr;
		inst_info.flags = 0;
		inst_info.pApplicationInfo = &app;
		inst_info.enabledLayerCount = 0;
		inst_info.ppEnabledLayerNames = reinterpret_cast<const char* const*>(0);
		inst_info.enabledExtensionCount = 0;
		inst_info.ppEnabledExtensionNames = reinterpret_cast<const char* const*>("");

		vkCreateInstance(&inst_info, nullptr, &m_vkInstance);
		SDL_Vulkan_CreateSurface(m_sdlWindow, m_vkInstance, &m_vkSurface);

#else 
		LogError("window not initialised for platform!");
#endif

#if defined(RENDERER_OPENGL) || defined(RENDERER_VULKAN)
		SDL_SetWindowBordered(m_sdlWindow, (m_windowBorder) ? SDL_TRUE : SDL_FALSE);
		SDL_SetWindowBordered(m_sdlWindow, SDL_TRUE);
		SDL_SetWindowResizable(m_sdlWindow, SDL_TRUE);

		if (m_fullscreen)
		{
			SDL_SetWindowFullscreen(m_sdlWindow, SDL_WINDOW_FULLSCREEN);
		}
#endif

		return true;
	}

	void LWindow::unload()
	{
#ifdef RENDERER_VULKAN
		vkDestroySurfaceKHR(m_vkInstance, m_vkSurface, nullptr);
		vkDestroyInstance(m_vkInstance, nullptr);
#endif
	}

	bool LWindow::load(String ConfigFile)
	{
		Config _config;

		_config.loadConfig(ConfigFile);
		m_windowCaption = _config.getString("window_caption");
		m_windowRect.X = _config.getInt("window_x");
		m_windowRect.Y = _config.getInt("window_y");
		m_windowRect.Width = _config.getInt("window_width", 1920);
		m_windowRect.Height = _config.getInt("window_height", 1080);
		m_windowBorder = _config.getInt("window_border");
		m_fullscreen = _config.getInt("fullscreen");

		return load();
	}

	void LWindow::render() 
	{
		float depthRange = 1.0f / (float)m_cameras.size();
		float index = 0; 

		for (auto camera : m_cameras)
		{
#ifdef RENDERER_OPENGL
			glDepthRange(0, 1);

			// Will break shadows.
			//glDepthRange(index * depthRange, (index + 1) * depthRange);
#else 
			LogWarning("Depth range per camera not supported by renderer.");
#endif
			index++;

			m_viewCamera = camera.second.get();
			m_renderer->m_activeCamera = m_viewCamera;

#ifdef RENDERER_OPENGL

			if (m_renderer->isShadowPass())
			{
				// Override viewport size
				glViewport(0, 0, m_renderer->getShadowMapSize(), m_renderer->getShadowMapSize());
				glScissor(0, 0, m_renderer->getShadowMapSize(), m_renderer->getShadowMapSize());
			}
			else
			{
				glViewport(0, 0, m_windowRect.Width, m_windowRect.Height);
				glScissor(0, 0, m_windowRect.Width, m_windowRect.Height);
			}

			// probably needed on ps5 aswell - I'm not sure whether we can clear the depth here as it changes the bound framebuffer, why is it here?
			//URenderTarget().GetScreenTarget()->clear((uint)UBufferBit::DEPTH);
#else 
			LogError("Viewport/ scissor not set, not supported by renderer.");
#endif

			m_viewCamera->calculateProjMatrix((float)m_windowRect.Width, (float)m_windowRect.Height);
			m_viewCamera->calculateViewMatrix();
			m_viewCamera->calculateViewProjMatrix();
			m_viewCamera->getParent()->m_parentScene->render();

			// Default back to screen size.
			glViewport(0, 0, m_windowRect.Width, m_windowRect.Height);
			glScissor(0, 0, m_windowRect.Width, m_windowRect.Height);
		}

#ifdef RENDERER_OPENGL
		glDepthRange(0, 1);
#else 
		LogWarning("Explicit Depth range not supported by renderer.");
#endif

	}

	void LWindow::DrawImGUIFullscreenWindow()
	{
		// Creates a full screen dock space 
		// if (IsEditorMode)
		ImGui::DockSpaceOverViewport();

	}

	void LWindow::RenderImGUI()
	{
	}
}
