#pragma once
#include <LCommon.h>
#include <Resources/ResourceHandle.h>
#include <Platform/Core/Components/LMeshRenderer.h>
#include <Platform/Core/Renderer/Materials/Material.h>
#include <glm/fwd.hpp>

namespace Lemonade
{
	class LEMONADE_API LLines : public LMeshRenderer
	{
	public:
        explicit LLines(CitrusCore::ResourcePtr<Material> material, std::shared_ptr<std::vector<glm::vec3>> vertcies);
		virtual bool Init() override;
	private:
		std::shared_ptr<std::vector<glm::vec3>> m_vertices;
	};
}
