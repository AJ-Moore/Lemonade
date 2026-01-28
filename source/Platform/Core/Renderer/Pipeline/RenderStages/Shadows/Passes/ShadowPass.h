#pragma once 

#include <LCommon.h>
#include <Platform/Vulkan/Renderer/LUniformBuffer.h>
#include <Platform/Core/Renderer/Pipeline/LRenderLayer.h>
#include <Platform/Vulkan/Renderer/LRenderTarget.h>
#include <Platform/Core/Renderer/Pipeline/LRenderer.h>
#include <Platform/Core/Renderer/Pipeline/LRenderStage.h>
#include <glm/fwd.hpp>

namespace Lemonade {
    class LEMONADE_API ShadowPass : public ARenderPass{
    public:
        ShadowPass();
        LRenderTarget& GetShadowRenderTarget() { return m_shadows; }

        uint32_t GetMaxShadowMaps() const { return m_maxShadowMaps; }
    protected:
        virtual bool Init() override;
        virtual void Update() override;
        virtual void Render(const LRenderingData& renderingData) override;
    private:
        void RenderShadow(const glm::mat4& view, const glm::mat4& projection, int layer);

        const uint32_t m_maxShadowMaps = 128;
        LRenderTarget m_shadows;
    };
}