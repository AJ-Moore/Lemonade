#pragma once
#include <LCommon.h>
#include <Platform/Core/LObject.h>
#include <memory>
#include <vector>

namespace Lemonade 
{
    class LEMONADE_API LRenderPass : public LObject 
    {

    };

    class LEMONADE_API LRenderStage : public LObject
    {
    protected:
        virtual bool Init() = 0;
        virtual void Render();

        void AddPass(std::shared_ptr<LRenderPass> renderPass);
    private:
        std::vector<std::shared_ptr<LRenderPass>> m_renderPasses;
        virtual void Render(ARenderTarget source, ARenderTarget destination)
    };
}