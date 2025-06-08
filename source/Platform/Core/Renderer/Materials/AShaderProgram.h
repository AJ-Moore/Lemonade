#pragma once

#include "Resources/AResource.h"
#include <Resources/ResourceHandle.h>
#include <Platform/Core/Renderer/Materials/AShader.h>
#include <memory>
#include <vector>

namespace Lemonade
{
	class AShaderProgram : public CitrusCore::AResource<AShaderProgram>
	{
	public:
		virtual void Bind();
		const std::vector<CitrusCore::ResourcePtr<AShader>>& GetShaders() const { return m_shaders; }

		virtual void AddShader(CitrusCore::ResourcePtr<AShader> shader);
	protected:
		virtual bool Init() = 0;
		virtual void Unload(){}
		virtual void Update(){}
		virtual void Render(){}

		virtual bool LoadResource(std::string path) = 0;
	private: 
		std::vector<CitrusCore::ResourcePtr<AShader>> m_shaders;
	};
}