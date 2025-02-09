#pragma once

#include <Common.h>
#include <Core/GameObject.h>
#include <Util/Rect.h>
#include <Renderer/Camera.h>

namespace CraftyBlocks
{
	class Viewport : public GameObject
	{
		friend class Window;
	protected:
		virtual bool Init();
		virtual void Unload();
		virtual void Update();
		virtual void Render();
	private:
		uRect m_viewRect = uRect(0, 0, 1920, 1080);
		std::vector<Camera> m_cameras;
	};
}