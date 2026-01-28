#pragma once 

#include <Platform/Core/Renderer/Pipeline/LRenderStage.h>

namespace Lemonade 
{
    class LEMONADE_API ShadowStage : public LRenderStage
    {
    protected:
        virtual bool Init() override;
    };
}