#include <Platform/Core/WindowManager/AWindow.h>
#include <LCommon.h>
#include <Util/Config.h>
#include <memory>
#include "Platform/Core/Renderer/Pipeline/LCamera.h"
#include "Util/Logger.h"


namespace Lemonade {

	using CitrusCore::Logger;

	LCamera* AWindow::GetActiveCamera() const 
	{
		return m_viewCamera;
	}

	void AWindow::SetActiveCamera(LCamera* Camera)
	{
		m_viewCamera = Camera;
	}

	void AWindow::RemoveCamera(std::shared_ptr<LCamera> camera)
	{
		if (m_cameras.find(camera->GetUID().GetID()) != m_cameras.end())
		{
			m_cameras.erase(camera->GetUID().GetID());
		}
	}

	bool AWindow::Init() 
	{
		InitFramebuffer();

//		auto input = UServiceLocator::getInstance()->getInputManager();
//#ifdef RENDERER_OPENGL
//		input->addBind(SDL_SCANCODE_F, &AWindow::ToggleFullscreen, this);
//#endif

		return true;
	}

	void AWindow::InitFramebuffer()
	{
		Logger::Log(Logger::INFO, "Initialising framebuffer.");
		Rect<uint32> windowRect = m_windowRect;

		//LogGLErrors();
		//m_geometryBuffer = std::make_shared<URenderTarget>(glm::vec2(windowRect.Width, windowRect.Height));
		//LogGLErrors();
//
		//m_geometryBuffer->setColourAttachments({ 
		//	UColourAttachment::UP_COLOR_ATTACHMENT0, 
		//	UColourAttachment::UP_COLOR_ATTACHMENT1, 
		//	UColourAttachment::UP_COLOR_ATTACHMENT2, 
		//	UColourAttachment::UP_COLOR_ATTACHMENT3, 
		//	UColourAttachment::UP_COLOR_ATTACHMENT4,
		//	UColourAttachment::UP_COLOR_ATTACHMENT5,
		//	UColourAttachment::UP_COLOR_ATTACHMENT6 }, true);
//
		//m_geometryBuffer->addDepthAttachment();
	}

	void AWindow::ToggleFullscreen(bool value)
	{
		if (!value)
		{
			return;
		}

		Logger::Log(Logger::ERROR, "Fullscreen not implemented on platform");
	}

	bool AWindow::Load() 
	{
		return true;
	}

	void AWindow::Unload()
	{
	}

	bool AWindow::Load(std::string configFile)
	{
		CitrusCore::Config config;

		config.LoadConfig(configFile);
		m_windowCaption = config.GetString("window_caption");
		m_windowRect.X = config.GetInt("window_x");
		m_windowRect.Y = config.GetInt("window_y");
		m_windowRect.Width = config.GetInt("window_width", 1920);
		m_windowRect.Height = config.GetInt("window_height", 1080);
		m_windowBorder = config.GetInt("window_border");
		m_fullscreen = config.GetInt("fullscreen");

		return Load();
	}

	void AWindow::Render() 
	{
		float depthRange = 1.0f / (float)m_cameras.size();
		float index = 0; 

		for (auto& viewport : m_viewports)
		{
			viewport->Render();
		}

		for (auto camera : m_cameras)
		{
#ifdef RENDERER_OPENGL
			glDepthRange(0, 1);

			// Will break shadows.
			//glDepthRange(index * depthRange, (index + 1) * depthRange);
#else 
			Logger::Log(Logger::WARN, "Depth range per camera not supported by renderer.");
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
			Logger::Log(Logger::ERROR, "Viewport/ scissor not set, not supported by renderer.");
#endif

			m_viewCamera->calculateProjMatrix((float)m_windowRect.Width, (float)m_windowRect.Height);
			m_viewCamera->calculateViewMatrix();
			m_viewCamera->calculateViewProjMatrix();
			//m_viewCamera->getParent()->m_parentScene->render();

			// Default back to screen size.
			//glViewport(0, 0, m_windowRect.Width, m_windowRect.Height);
			//glScissor(0, 0, m_windowRect.Width, m_windowRect.Height);
		}

#ifdef RENDERER_OPENGL
		glDepthRange(0, 1);
#else 
		Logger::Log(Logger::WARN,"Explicit Depth range not supported by renderer.");
#endif

	}

	//void AWindow::DrawImGUIFullscreenWindow()
	//{
	//	// Creates a full screen dock space 
	//	// if (IsEditorMode)
	//	ImGui::DockSpaceOverViewport();
//
	//}

	void AWindow::RenderImGUI()
	{
	}
}
