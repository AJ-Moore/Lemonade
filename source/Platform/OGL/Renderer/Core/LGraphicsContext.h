#pragma once

#include <Core/Renderer/Core/AGraphicsContext.h>#
#include <Vulkan/Renderer/LVulkanDevice.h>

namespace Lemonade
{
	class LEMONADE_API LGraphicsContext : public AGraphicsContext
	{
	public:
		const SDL_GLContext& getContext() const { return m_glContext; }
	private:
		SDL_GLContext m_glContext;
	};
}