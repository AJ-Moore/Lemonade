#pragma once

#include <LCommon.h>
#include <Util/UID.h>
#include <Geometry/Rect.h>
#include <Platform/Core/LObject.h>
#include <Platform/Core/Renderer/Pipeline/LCamera.h>
#include <Platform/Core/Renderer/Pipeline/AViewport.h>
#include <Platform/Core/WindowManager/AWindowManager.h>
#include <unordered_set>

namespace Lemonade {

	using CitrusCore::Rect;

	class LEMONADE_API AWindow : public LObject {
		friend class LWindowManager; 
		friend class LRenderer;
	public: 
		AWindow();
		virtual ~AWindow(); 
		LCamera* GetActiveCamera() const;
		void SetActiveCamera(LCamera* Camera);
		void AddCamera(std::shared_ptr<LCamera> camera) { m_cameras[camera->GetUID().GetID()] = camera; }
		void SetPostProcessCamera(std::shared_ptr<LCamera> camera) { m_postProcessingCamera = camera; }
		void RemoveCamera(std::shared_ptr<LCamera> camera);

		const Rect<uint32>& GetWindowRect() const { return m_windowRect; }
		int GetWidth() const { return m_windowRect.Width; }
		int GetHeight() const { return m_windowRect.Height; }

#ifdef RENDERER_VULKAN
		const VkInstance& getVkInstance() const { return m_vkInstance; }
		const VkSurfaceKHR& getVkSurface() const { return m_vkSurface; }
#endif

	protected:
		virtual bool Init();
		virtual void Update(){}
		virtual void Unload();
		virtual bool Load(std::string configFile);
		virtual bool Load();
		virtual void Render();
		virtual void RenderImGUI();

	private:
		void InitFramebuffer();
		void ToggleFullscreen(float value);
		void SetParent(LWindowManager* windowManager) { m_windowManager = windowManager; }

		CitrusCore::Rect<uint32> m_windowRect = CitrusCore::Rect<uint32>(50, 50, 800, 600);

#if defined(RENDERER_OPENGL) || defined(RENDERER_VULKAN)
		SDL_Window* m_sdlWindow = nullptr;
#endif

		/// The camera linked with this window, acts as a view into the scene
		LCamera* m_viewCamera = nullptr;
		LRenderer* m_renderer = nullptr;
		LWindowManager* m_windowManager = nullptr; 

		std::string	m_windowCaption = "Game";
		bool	m_fullscreen = false;
		bool	m_windowBorder = false; 
		bool 	m_sizeable = false;

		std::unordered_map<uint64, std::shared_ptr<LCamera>> m_cameras;
		std::unordered_set<std::shared_ptr<AViewport>> m_viewports;

		std::shared_ptr<LCamera> m_postProcessingCamera;

		// Render Target/ framebuffer 
		std::shared_ptr<ARenderTarget>  m_geometryBuffer;
	};
}
