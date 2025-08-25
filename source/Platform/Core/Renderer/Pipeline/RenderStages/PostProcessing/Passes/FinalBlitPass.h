#pragma once 

#include <Platform/Core/Renderer/Pipeline/LRenderLayer.h>
#include <Platform/Vulkan/Renderer/LRenderTarget.h>
#include <Platform/Core/Renderer/Pipeline/LRenderer.h>
#include <LCommon.h>
#include <Platform/Core/Renderer/Pipeline/LRenderStage.h>

namespace Lemonade {
    class LEMONADE_API FinalBlitPass : public ARenderPass{
    public:
        FinalBlitPass();
    protected:
        virtual bool Init() override;
        virtual void Render( const LRenderingData& renderingData) override;
        virtual void Update() override;
    private:
        LRenderLayer m_renderLayer;
        LRenderLayer m_sky;
    };
}