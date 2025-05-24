#include <Platform/Core/Services/GraphicsServices.h>
#include <Platform/OGL/Renderer/Core/LGraphicsContext.h>

#ifdef RENDERER_OPENGL

namespace Lemonade
{
    bool LGraphicsContext::Init() 
    {
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

		// Setup Platform/Renderer bindings
#ifdef RENDERER_OPENGL
// TODO IMGUI 
		//ImGui_ImplSDL2_InitForOpenGL(m_sdlWindow, m_windowManager->getContext());
		//ImGui_ImplOpenGL3_Init(glsl_version);
#elif defined(RENDERER_VULKAN)
		ImGui_ImplSDL2_InitForVulkan(m_sdlWindow);
#elif defined(RENDERER_PS5)
		
		ImFont* font = ImGuiLibFont::AddSystemFont(io.Fonts, (float)48);
		ImGui_ImplSampleUtil_Init(nullptr, nullptr, new sce::PS5Utils::System::UserIdManager(), false);


		LogError("Imgui not initialismed for target platform.");
#endif

		auto windowManager = Services::GetWindowManager();
		m_glContext = SDL_GL_CreateContext(windowManager->GetMainWindow()->GetSDLWindow());

		// Set any initial graphics state.
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        return true;
    }

    void LGraphicsContext::Update(){

    } 
    void LGraphicsContext::Unload()
	{
		SDL_GL_DestroyContext(m_glContext);
    }
    void LGraphicsContext::Render(){

    }
}

#endif 
