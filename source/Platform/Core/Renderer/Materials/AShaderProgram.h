#pragma once

#include <Platform/Core/LObject.h>
#include <Resources/AResource.h>
#include <Resources/ResourceHandle.h>
#include <Platform/Core/Renderer/Materials/AShader.h>
#include <vector>

namespace Lemonade
{
	class AShaderProgram : public Lemonade::LObject
	{
	public:
		virtual void Bind() = 0;
		const std::vector<CitrusCore::ResourcePtr<AShader>>& GetShaders() const { return m_shaders; }

		virtual void AddShader(CitrusCore::ResourcePtr<AShader> shader);
	protected:
		virtual bool Init() = 0;
		virtual void Unload(){}
		virtual void Update(){}
		virtual void Render(){}
	private: 
		std::vector<CitrusCore::ResourcePtr<AShader>> m_shaders;
	};
}