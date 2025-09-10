#pragma once
#include <LCommon.h>
#include <Resources/ResourceHandle.h>
#include <Platform/Core/Components/LMeshRenderer.h>
#include <Platform/Core/Renderer/Materials/Material.h>
#include <glm/fwd.hpp>

namespace Lemonade
{
	class LEMONADE_API LCube : public LMeshRenderer
	{
	public:
        explicit LCube(CitrusCore::ResourcePtr<Material> material, glm::vec3 dimensions = glm::vec3(1));
		virtual bool Init() override;
	private:
		glm::vec3 m_dimensions;
	};
}
