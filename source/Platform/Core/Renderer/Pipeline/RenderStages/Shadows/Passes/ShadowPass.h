#pragma once 

#include <LCommon.h>
#include <Platform/Vulkan/Renderer/LUniformBuffer.h>
#include <Platform/Core/Renderer/Pipeline/LRenderLayer.h>
#include <Platform/Vulkan/Renderer/LRenderTarget.h>
#include <Platform/Core/Renderer/Pipeline/LRenderer.h>
#include <Platform/Core/Renderer/Pipeline/LRenderStage.h>

namespace Lemonade {
    class LEMONADE_API ShadowPass : public ARenderPass{
    public:
        ShadowPass();
    protected:
        virtual bool Init() override;
        virtual void Update() override;
        virtual void Render(const LRenderingData& renderingData) override;
    private:
        const int m_maxShadowMaps = 1;
        LRenderTarget m_shadows;
    };
}