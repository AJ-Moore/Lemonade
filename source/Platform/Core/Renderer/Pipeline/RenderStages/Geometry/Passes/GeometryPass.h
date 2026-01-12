#pragma once 

#include <Platform/Vulkan/Renderer/LUniformBuffer.h>
#include <Platform/Core/Renderer/Pipeline/LRenderLayer.h>
#include <Platform/Vulkan/Renderer/LRenderTarget.h>
#include <Platform/Core/Renderer/Pipeline/LRenderer.h>
#include <LCommon.h>
#include <Platform/Core/Renderer/Pipeline/LRenderStage.h>
#include <Platform/Core/Renderer/Pipeline/RenderStages/Geometry/DeferredData.h>

namespace Lemonade {
    class LEMONADE_API GeometryPass : public ARenderPass{
    public:
        GeometryPass();
    protected:
        virtual bool Init() override;
        virtual void Update() override;
        virtual void Render(const LRenderingData& renderingData) override;

        virtual void UpdateDeferredData(const LRenderingData& renderingData);
    private:
        DeferredData m_deferredData;
        std::shared_ptr<LUniformBuffer> m_lightignBuffer;
        std::shared_ptr<LUniformBuffer> m_deferredBuffer;
        LRenderTarget m_geometryTarget;
        LRenderTarget m_gBuffer;
        LRenderLayer m_deferredPass;
    };
}