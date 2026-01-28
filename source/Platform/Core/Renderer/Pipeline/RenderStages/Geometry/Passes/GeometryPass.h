#pragma once 

#include <Platform/Core/Renderer/Pipeline/RenderStages/Shadows/Passes/ShadowPass.h>
#include <Platform/Vulkan/Renderer/LUniformBuffer.h>
#include <Platform/Core/Renderer/Pipeline/LRenderLayer.h>
#include <Platform/Vulkan/Renderer/LRenderTarget.h>
#include <Platform/Core/Renderer/Pipeline/LRenderer.h>
#include <LCommon.h>
#include <Platform/Core/Renderer/Pipeline/LRenderStage.h>
#include <Platform/Core/Renderer/Pipeline/RenderStages/Geometry/DeferredData.h>
#include <memory>

namespace Lemonade {
    class LEMONADE_API GeometryPass : public ARenderPass{
    public:
        GeometryPass();
        void SetShadowPass(std::shared_ptr<ShadowPass> pass) { m_shadowPass = pass; }
    protected:
        virtual bool Init() override;
        virtual void Update() override;
        virtual void Render(const LRenderingData& renderingData) override;

        virtual void UpdateDeferredData(const LRenderingData& renderingData);
    private:
        std::shared_ptr<ShadowPass> m_shadowPass;
        DeferredData m_deferredData;
        std::shared_ptr<LUniformBuffer> m_lightignBuffer;
        std::shared_ptr<LUniformBuffer> m_deferredBuffer;
        LRenderTarget m_geometryTarget;
        LRenderTarget m_gBuffer;
        LRenderLayer m_deferredPass;
        uint32 m_shadowsImageSamplerLocation = 8;
    };
}