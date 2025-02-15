#pragma once

#include <LCommon.h>
#include <Core/LObject.h>
#include <Core/Renderer/Pipeline/LCamera.h>

namespace Lemonade
{
	class Viewport : public LObject
	{
		friend class Window;
	protected:
		virtual bool Init();
		virtual void Unload();
		virtual void Update();
		virtual void Render();
	private:
		Citrus::Rect m_viewRect = Citrus::Rect(0, 0, 1920, 1080);
		std::vector<LCamera> m_cameras;
	};
}