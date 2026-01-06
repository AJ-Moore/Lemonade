#pragma once
#include <Platform/Core/Components/ModelLoader/LModelResource.h>
#include <LCommon.h>
#include <Platform/Core/Components/RenderComponent.h>
#include <Resources/ResourceHandle.h>
#include <Platform/Core/Components/LMeshRenderer.h>
#include <Platform/Core/Renderer/Materials/Material.h>
#include <glm/fwd.hpp>

namespace Lemonade
{
	class LEMONADE_API LModel : public RenderComponent
	{
	public:
        LModel(std::string modelPath);
		virtual bool Init() override;
	private:
		CitrusCore::ResourcePtr<LModelResource> m_model;
        std::string m_path;
	};
}
