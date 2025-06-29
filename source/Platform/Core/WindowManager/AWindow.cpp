#include <Platform/Core/WindowManager/AWindow.h>
#include <LCommon.h>
#include <Util/Config.h>
#include <memory>
#include "Platform/Core/Renderer/Pipeline/LCamera.h"
#include "Platform/Vulkan/Renderer/Pipeline/LViewport.h"
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
		m_fullscreen = config.GetInt("fullscreen", 0);

		std::shared_ptr<LViewport> viewport = std::make_shared<LViewport>();
		viewport->Init();
		viewport->m_viewRect = m_windowRect;
		m_viewports.push_back(viewport);

		return Load();
	}

	void AWindow::Render() 
	{
		float depthRange = 1.0f / (float)m_cameras.size();
		float index = 0; 

		// Will break shadows.
		//glDepthRange(index * depthRange, (index + 1) * depthRange);

		for (auto& viewport : m_viewports)
		{
			viewport->Render();
		}
	}

	void AWindow::RenderImGUI()
	{
	}
}
