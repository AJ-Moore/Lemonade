#pragma once 

#include <Platform/Vulkan/Renderer/LUniformBuffer.h>
#include <Platform/Core/Renderer/Pipeline/LRenderLayer.h>
#include <Platform/Vulkan/Renderer/LRenderTarget.h>
#include <Platform/Core/Renderer/Pipeline/LRenderer.h>
#include <LCommon.h>
#include <Platform/Core/Renderer/Pipeline/LRenderStage.h>

namespace Lemonade {
    class LEMONADE_API GeometryPass : public ARenderPass{
    public:
        GeometryPass();
    protected:
        virtual bool Init() override;
        virtual void Update() override;
        virtual void Render( const LRenderingData& renderingData) override;
    private:
        std::shared_ptr<LUniformBuffer> m_lightignData;
        LRenderTarget m_geometryTarget;
        LRenderTarget m_gBuffer;
        LRenderLayer m_renderLayer;
    };
}