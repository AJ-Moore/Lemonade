#pragma once 

#include <Platform/Core/Renderer/Pipeline/LRenderStage.h>

namespace Lemonade 
{
    class LEMONADE_API PostProcessingStage : public LRenderStage
    {
    protected:
        virtual bool Init() override;
    };
}