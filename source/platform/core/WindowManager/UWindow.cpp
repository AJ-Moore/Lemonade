

#include "UWindow.h"
#include <UCommon.h>
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


namespace UpsilonEngine {
	UWindow::UWindow() 
	{
		m_windowRect = Rect<uint32>(50, 50, 800, 600);
		m_fullscreen = false; 
		m_sizeable = false; 
		m_windowBorder = true; 
		m_windowCaption = "Upsilon Game";

		m_viewCamera = nullptr;
	}

	UWindow::~UWindow() {
	}

	UCamera* UWindow::getActiveCamera() const 
	{
		return m_viewCamera;
	}

	void UWindow::setActiveCamera(UCamera* Camera){
		m_viewCamera = Camera;
	}

	void UWindow::removeCamera(SharedPtr<UCamera> camera)
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

	bool UWindow::init() 
	{

#ifdef RENDERER_PS5
		auto heapAllocator = UServiceLocator::getInstance()->getPS5VideoOut();

		void* s_defaultHeapStart = nullptr;
		off_t offsetOut;
		size_t defaultHeapSize = 30 * 1024 * 1024;
		int32_t ret = sceKernelAllocateMainDirectMemory(defaultHeapSize, 0x10000, SCE_KERNEL_MTYPE_C, &offsetOut);

		if (ret != SCE_OK)
		{
			LogError("Out of memory.");
		}

		ret = sceKernelMapNamedDirectMemory(&s_defaultHeapStart, defaultHeapSize, SCE_KERNEL_PROT_GPU_RW | SCE_KERNEL_PROT_CPU_RW, 0, offsetOut, 0x10000, "imgui heap");

		if (ret != SCE_OK)
		{
			LogError("Out of memory.");
		}

		SceLibcMspace imguiHeap = sceLibcMspaceCreate("imgui heap", s_defaultHeapStart, defaultHeapSize, 0);

		ImGui::SetAllocatorFunctions(allocMem, freeMem, imguiHeap);
#endif

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigWindowsResizeFromEdges = true;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();
		// 
		// Unreal style by dev0-1 from ImThemes
		ImGuiStyle& style = ImGui::GetStyle();

		style.Alpha = 1.0f;
		style.DisabledAlpha = 0.6000000238418579f;
		style.WindowPadding = ImVec2(8.0f, 8.0f);
		style.WindowRounding = 0.0f;
		style.WindowBorderSize = 1.0f;
		style.WindowMinSize = ImVec2(32.0f, 32.0f);
		style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
		style.WindowMenuButtonPosition = ImGuiDir_Left;
		style.ChildRounding = 0.0f;
		style.ChildBorderSize = 1.0f;
		style.PopupRounding = 0.0f;
		style.PopupBorderSize = 1.0f;
		style.FramePadding = ImVec2(4.0f, 3.0f);
		style.FrameRounding = 0.0f;
		style.FrameBorderSize = 0.0f;
		style.ItemSpacing = ImVec2(8.0f, 4.0f);
		style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
		style.CellPadding = ImVec2(4.0f, 2.0f);
		style.IndentSpacing = 21.0f;
		style.ColumnsMinSpacing = 6.0f;
		style.ScrollbarSize = 14.0f;
		style.ScrollbarRounding = 9.0f;
		style.GrabMinSize = 10.0f;
		style.GrabRounding = 0.0f;
		style.TabRounding = 4.0f;
		style.TabBorderSize = 0.0f;
		style.TabMinWidthForCloseButton = 0.0f;
		style.ColorButtonPosition = ImGuiDir_Right;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

		style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.05882352963089943f, 0.05882352963089943f, 0.05882352963089943f, 0.9399999976158142f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9399999976158142f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2000000029802322f, 0.2078431397676468f, 0.2196078449487686f, 0.5400000214576721f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4000000059604645f, 0.4000000059604645f, 0.4000000059604645f, 0.4000000059604645f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.1764705926179886f, 0.1764705926179886f, 0.1764705926179886f, 0.6700000166893005f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.03921568766236305f, 0.03921568766236305f, 0.03921568766236305f, 1.0f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.2862745225429535f, 0.2862745225429535f, 0.2862745225429535f, 1.0f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.5099999904632568f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.01960784383118153f, 0.01960784383118153f, 0.01960784383118153f, 0.5299999713897705f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3098039329051971f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.407843142747879f, 0.407843142747879f, 0.407843142747879f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5098039507865906f, 0.5098039507865906f, 0.5098039507865906f, 1.0f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.9372549057006836f, 0.9372549057006836f, 0.9372549057006836f, 1.0f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.5098039507865906f, 0.5098039507865906f, 0.5098039507865906f, 1.0f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.8588235378265381f, 0.8588235378265381f, 0.8588235378265381f, 1.0f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.4392156898975372f, 0.4392156898975372f, 0.4392156898975372f, 0.4000000059604645f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.4588235318660736f, 0.4666666686534882f, 0.47843137383461f, 1.0f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.4196078479290009f, 0.4196078479290009f, 0.4196078479290009f, 1.0f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.6980392336845398f, 0.6980392336845398f, 0.6980392336845398f, 0.3100000023841858f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.6980392336845398f, 0.6980392336845398f, 0.6980392336845398f, 0.800000011920929f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.47843137383461f, 0.4980392158031464f, 0.5176470875740051f, 1.0f);
		style.Colors[ImGuiCol_Separator] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.7176470756530762f, 0.7176470756530762f, 0.7176470756530762f, 0.7799999713897705f);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.5098039507865906f, 0.5098039507865906f, 0.5098039507865906f, 1.0f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.9098039269447327f, 0.9098039269447327f, 0.9098039269447327f, 0.25f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.8078431487083435f, 0.8078431487083435f, 0.8078431487083435f, 0.6700000166893005f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.4588235318660736f, 0.4588235318660736f, 0.4588235318660736f, 0.949999988079071f);
		style.Colors[ImGuiCol_Tab] = ImVec4(0.1764705926179886f, 0.3490196168422699f, 0.5764706134796143f, 0.8619999885559082f);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.800000011920929f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(0.196078434586525f, 0.407843142747879f, 0.6784313917160034f, 1.0f);
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.06666667014360428f, 0.1019607856869698f, 0.1450980454683304f, 0.9724000096321106f);
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1333333402872086f, 0.2588235437870026f, 0.4235294163227081f, 1.0f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.4274509847164154f, 0.3490196168422699f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.729411780834198f, 0.6000000238418579f, 0.1490196138620377f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
		style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
		style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
		style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.8666666746139526f, 0.8666666746139526f, 0.8666666746139526f, 0.3499999940395355f);
		style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
		style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.6000000238418579f, 0.6000000238418579f, 0.6000000238418579f, 1.0f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
		style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);

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
		input->addBind(SDL_SCANCODE_F, &UWindow::ToggleFullscreen, this);
#endif

		return true;
	}

	void UWindow::initFramebuffer()
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

	void UWindow::ToggleFullscreen(float value)
	{
		if (!value)
		{
			return;
		}

#if defined(RENDERER_OPENGL) || defined(RENDERER_VULKAN)
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

	bool UWindow::load() 
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

	void UWindow::unload()
	{
#ifdef RENDERER_VULKAN
		vkDestroySurfaceKHR(m_vkInstance, m_vkSurface, nullptr);
		vkDestroyInstance(m_vkInstance, nullptr);
#endif
	}

	bool UWindow::load(String ConfigFile)
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

	void UWindow::render() 
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

	void UWindow::drawImGUIFullscreenWindow()
	{
		// Creates a full screen dock space 
		// if (IsEditorMode)
		ImGui::DockSpaceOverViewport();

	}

	void UWindow::renderImGUI()
	{
		// Start the Dear ImGui frame
#ifdef RENDERER_OPENGL
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(m_sdlWindow);
		ImGui::NewFrame();

		//drawImGUIFullscreenWindow();

		m_viewCamera->getParent()->m_parentScene->drawGUI();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#elif defined(RENDERER_VULKAN)
		//ImGui_ImplVulkan_NewFrame();
		//ImGui_ImplSDL2_NewFrame(m_sdlWindow);
		//ImGui::NewFrame();
		//
		////drawImGUIFullscreenWindow();
		//
		//m_viewCamera->getParent()->m_parentScene->drawGUI();
		//
		//ImGui::Render();
		//ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), UServiceLocator::getInstance()->getRenderer()->getCommandBuffer());
		LogError("Imgui not setup for rendering on platform.");
#elif defined(RENDERER_PS5)
		ImGuiIO& io = ImGui::GetIO();
		if (ImGuiLibFont::BuildFontAtlas(io.Fonts, UServiceLocator::getInstance()->getPS5VideoOut()->getCPUMemory()))
		{
			ImGui_ImplSampleUtil_InvalidateDeviceObjects();
			ImGui_ImplSampleUtil_CreateDeviceObjects();
		}
		ImGui_ImplSampleUtil_NewFrame(m_windowRect.Width, m_windowRect.Height, nullptr, nullptr, nullptr);
		ImGui::NewFrame();
		ImGui::PushFont(ImGui_ImplSampleUtil_defaultFont);

		m_viewCamera->getParent()->m_parentScene->drawGUI();

		ImGui::PopFont();
		ImGui::Render();

		const sce::Agc::DrawCommandBuffer& db = UServiceLocator::getInstance()->getPS5VideoOut()->getContext().m_dcb;
		ImGui_ImplSampleUtil_RenderDrawData(const_cast<sce::Agc::DrawCommandBuffer*>(&db), ImGui::GetDrawData());
#else 
		LogError("Imgui not setup for rendering on platform.");
#endif
	}

	void UWindow::setParent(UWindowManager* windowManager)
	{
		m_windowManager = windowManager;
	}
}
