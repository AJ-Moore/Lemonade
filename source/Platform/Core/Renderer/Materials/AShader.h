#pragma once

#include <Base/Object.h>
#include <Resources/AResource.h>
#include <LCommon.h>
#include <Platform/Core/Renderer/Materials/TextureUtils.h>

namespace Lemonade
{
	// Move somewhere else.
	enum class ShaderType
	{
		None,
		Fragment,
		Vertex,
		Geometry,
		TessellationControl,
		TessellationEvaluation,
		Compute,
		RayGeneration,
		AnyHit,
		ClosestHit,
		Miss,
		Intersection,
		Task,
		Mesh,
		Amplification,
	};

	class AShaderProgram;

	class AShader : public CitrusCore::AResource<AShader>
	{
		friend class AShaderProgram;
	public:
		virtual void Bind() = 0;
		static const std::unordered_map<std::string, ShaderType> m_shaderTypeLookup;
		static ShaderType GetShaderTypeForString(std::string type);
		ShaderType GetShaderType() const { return m_shaderType; }

	protected:
		virtual bool LoadResource(std::string path) override = 0;
		virtual void UnloadResource() override{};
		void SetParent(AShaderProgram* shaderProgram) { m_parent = shaderProgram; }

		AShaderProgram* GetParent() const { return m_parent; }
		void SetShaderType(ShaderType type);
	private:
		AShaderProgram* m_parent = nullptr;
		ShaderType m_shaderType;
	};
}