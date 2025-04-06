#pragma once

#include <LCommon.h>
#include <Platform/Core/LObject.h>
#include <Platform/Core/Renderer/Pipeline/LCamera.h>
#include <Geometry/Rect.h>

namespace Lemonade
{
	using CitrusCore::Recti;

	class AViewport : public LObject
	{
		friend class Window;
	protected:
		virtual bool Init();
		virtual void Unload();
		virtual void Update();
		virtual void Render();

		virtual void SetViewport(int x, int y, int width, int height) = 0; 
		virtual void SetScissor(int x, int y, int width, int height) = 0; 
		virtual void SetDepthRange(float near, float far) = 0;
	private:
		Recti m_viewRect = Recti(0, 0, 1920, 1080);
		std::vector<std::shared_ptr<LCamera>> m_cameras;
	};
}