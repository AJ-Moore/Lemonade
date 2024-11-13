#pragma once

#include "../../UCommon.h"
#include "../../Core/UService.h"
#include "../../Util/Rect.h"

#include "UWindow.h"

namespace UpsilonEngine
{
	class DLLExport UWindowManager : public UService {
		friend class UServiceLocator;
		friend class URenderer;
		friend class UWindow;
	public:
		UWindowManager(); 
		virtual ~UWindowManager(); 

		/// Add a window to the window manager using the location of a window config file
		UWindow* addWindow(String ConfigFile);

		/// Add a window to window manager at root of hierarchy 
		UWindow* addWindow(Rect<uint32> WindowRect, String WindowName);

		/// Adds the window specified to the root of the hierarchy. 
		void addWindow(UWindow* Window);

		/// Returns a reference to the default persistant window.
		UWindow* getMainWindow();

#if defined(RENDERER_VULKAN)
		UVulkanDevice* getVulkanDevice() { return &m_vulkanDevice; }
#endif

	protected:
		virtual bool init();
		virtual bool load();

		virtual void update();
		virtual void unload();
		virtual void render();

	private:

#ifdef RENDERER_OPENGL
		const SDL_GLContext& getContext() const { return glContext; }

		/// The GLContext used by all windows handeled by this window manager.
		SDL_GLContext glContext;
#endif

#if defined(RENDERER_VULKAN)
		UVulkanDevice m_vulkanDevice;
#endif

		/// Maintains a list of the root windows.
		std::map<uint32, UWindow*> m_windows;

		/// Reference to the default window used by the engine, persistant
		UWindow* defaultWindow; 
	};
}
