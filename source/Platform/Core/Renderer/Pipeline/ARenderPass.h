#pragma once 

#include "Platform/Core/LObject.h"
#include "Platform/Core/Renderer/Pipeline/LRenderer.h"
#include <LCommon.h>

namespace Lemonade {
    class LEMONADE_API ARenderPass : public LObject 
    {
        friend class LRenderStage;
    protected:
        virtual bool Init() = 0;
        virtual void Render(){}
        virtual void Render(const LRenderingData& renderingData) = 0; 
        virtual void Update(){}
        virtual void Unload(){}
    private:
    };
}