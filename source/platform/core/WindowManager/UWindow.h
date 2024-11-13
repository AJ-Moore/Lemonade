#pragma once

#include <UCommon.h>
#include <Renderer/UCamera.h>
#include <Renderer/URenderer.h>
#include <Util/UID.h>
#include <Util/Rect.h>
#include <Renderer/ARenderTarget.h>

namespace UpsilonEngine {
	class DLLExport UWindow {
		friend class UWindowManager; 
		friend class URenderer;
	public: 
		UWindow();
		virtual ~UWindow(); 
		UCamera* getActiveCamera() const;
		void setActiveCamera(UCamera* Camera);
		void addCamera(SharedPtr<UCamera> camera) { m_cameras[camera->UUID.getID()] = camera; }
		void setPostProcessCamera(SharedPtr<UCamera> camera) { m_postProcessingCamera = camera; }
		void removeCamera(SharedPtr<UCamera> camera);

		const Rect<uint32>& GetWindowRect() const { return m_windowRect; }
		int GetWidth() const { return m_windowRect.Width; }
		int GetHeight() const { return m_windowRect.Height; }

#ifdef RENDERER_VULKAN
		const VkInstance& getVkInstance() const { return m_vkInstance; }
		const VkSurfaceKHR& getVkSurface() const { return m_vkSurface; }
#endif

	protected:
		virtual bool init();
		virtual void update(){}
		virtual void unload();
		virtual bool load(String ConfigFile);
		virtual bool load();
		virtual void render();
		virtual void renderImGUI();

	private:
		void initFramebuffer();
		void ToggleFullscreen(float value);
		void setParent(UWindowManager* windowManager);

		void drawImGUIFullscreenWindow();

		UID uuid;
		Rect<uint32> m_windowRect;

#if defined(RENDERER_OPENGL) || defined(RENDERER_VULKAN)
		SDL_Window* m_sdlWindow = nullptr;
#endif

#ifdef RENDERER_VULKAN
		VkInstance m_vkInstance = nullptr;
		VkSurfaceKHR m_vkSurface = VK_NULL_HANDLE;
#endif

		/// The camera linked with this window, acts as a view into the scene
		UCamera* m_viewCamera = nullptr;
		URenderer* m_renderer = nullptr;
		UWindowManager* m_windowManager = nullptr; 

		String	m_windowCaption;
		bool	m_fullscreen = false;
		bool	m_windowBorder = false; 
		bool 	m_sizeable = false;

		std::map<uint64, SharedPtr<UCamera>> m_cameras;

		SharedPtr<UCamera> m_postProcessingCamera;

		// Render Target/ framebuffer 
		SharedPtr<ARenderTarget>  m_geometryBuffer;
	};
}
