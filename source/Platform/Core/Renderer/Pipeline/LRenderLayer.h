#pragma once

#include <Platform/Core/LObject.h>
#include "Platform/Core/Renderer/RenderBlock/ARenderBlock.h"
#include <Types.h>
#include <LCommon.h>
#include <Platform/Core/Renderer/Materials/Material.h>
#include <glm/fwd.hpp>
#include <memory>

namespace Lemonade 
{
	class LRenderLayer : public LObject
	{
        struct SSBOData
        {
            glm::uvec2 Dimensions;
        };
	public:
    LRenderLayer(ResourcePtr<Material> material) : m_material(material) {}
		virtual bool Init();
        virtual void Unload(){}
		virtual void Render();
		virtual void Update();

		void SetMaterial(ResourcePtr<Material> material) { m_material = material; }
        void SetDimensions(glm::uvec2 dimensions) { m_ssboData.Dimensions = dimensions; }
	private:
        SSBOData m_ssboData;
		std::shared_ptr<ARenderBlock> m_renderBlock = nullptr;
		ResourcePtr<Material> m_material = nullptr;
	};
}

