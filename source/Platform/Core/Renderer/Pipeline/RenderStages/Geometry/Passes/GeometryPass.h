#pragma once 

#include <Platform/Vulkan/Renderer/LRenderTarget.h>
#include <Platform/Core/Renderer/Pipeline/LRenderer.h>
#include <LCommon.h>
#include <Platform/Core/Renderer/Pipeline/LRenderStage.h>

namespace Lemonade {
    class LEMONADE_API GeometryPass : public ARenderPass{
    protected:
        bool Init();
        virtual void Render( const LRenderingData& renderingData);
    private:
        LRenderTarget m_geometryTarget;
    };
}