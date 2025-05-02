#pragma once

#include <LCommon.h>
#ifdef RENDERER_OPENGL

#include <Platform/Core/WindowManager/LSDLWindow.h>

namespace Lemonade
{
	class LEMONADE_API LWindow : public LSDLWindow {
		friend class LWindowManager; 
	public:
		virtual ~LWindow(){}
	protected:
		virtual void Unload() override;
		virtual bool Init() override;
		virtual void Render() override;
	private:
	};
}

#endif