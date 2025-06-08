#pragma once

#include <Platform/Core/Renderer/Materials/AShader.h>
#include <Resources/AResource.h>

namespace Lemonade
{
	class Shader : AShader
	{
	protected:
		virtual bool LoadResource(std::string path) override;
		virtual void UnloadResource() override{}
	private:
		const std::string m_meta = ".meta";
	};
}