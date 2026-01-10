#pragma once

#include <Platform/Core/LObject.h>
#include <Platform/Core/Renderer/Pipeline/ARenderTarget.h>
#include <Platform/Core/Renderer/RenderBlock/ARenderBlock.h>
#include <Types.h>
#include <LCommon.h>
#include <Platform/Core/Renderer/Materials/Material.h>
#include <glm/fwd.hpp>
#include <memory>
#include <vulkan/vulkan_core.h>

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
		virtual bool Init() override;
        virtual void Unload() override{}
		virtual void Render() override;
		virtual void Update() override;

		void SetMaterial(ResourcePtr<Material> material) { m_material = material; }
        void SetDimensions(glm::uvec2 dimensions) { m_ssboData.Dimensions = dimensions; }
		void SetRenderTarget(ARenderTarget* renderTarget) { m_sourceTarget = renderTarget; }
		void SetBlendEnabled(bool value){m_renderBlock->SetBlendEnabled(value);}

		std::shared_ptr<ARenderBlock> GetRenderBlock() const { return m_renderBlock; }
	private:
        SSBOData m_ssboData;
		std::shared_ptr<ARenderBlock> m_renderBlock = nullptr;
		ResourcePtr<Material> m_material = nullptr;
		ARenderTarget* m_sourceTarget = nullptr;
	};
}

