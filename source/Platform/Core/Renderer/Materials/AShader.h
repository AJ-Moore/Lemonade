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

	class AShader : public CitrusCore::AResource<AShader>
	{
	public:
		virtual void Bind() = 0;
		virtual uint32 LoadAndCompileShader(const std::string& shaderFile, ShaderType shaderType) = 0;
		static const std::unordered_map<std::string, ShaderType> m_shaderTypeLookup;
		static ShaderType GetShaderType(std::string type);
	protected:
		virtual bool LoadResource(std::string path) = 0;
	};
}