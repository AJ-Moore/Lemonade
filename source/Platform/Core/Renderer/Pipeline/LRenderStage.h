#pragma once
#include <Platform/Core/Renderer/Pipeline/ARenderPass.h>
#include <LCommon.h>
#include <Platform/Core/LObject.h>
#include <memory>
#include <vector>

namespace Lemonade 
{
    class LRenderingData;
    class LEMONADE_API LRenderStage : public LObject
    {
        friend class LRenderer;
    protected:
        ~LRenderStage() = default;
        virtual bool Init(){ return true; }
        virtual void Update(){}
        virtual void Render(){}
        virtual void Render(const LRenderingData& renderingData);

        void AddPass(std::shared_ptr<ARenderPass> renderPass);
    private:
        std::vector<std::shared_ptr<ARenderPass>> m_renderPasses;
    };
}