#pragma once 

#include <Platform/Core/Renderer/Pipeline/LRenderStage.h>

namespace Lemonade 
{
    class LEMONADE_API GeometryStage : public LRenderStage
    {
    protected:
        bool Init();
    };
}