#pragma once
#include "Platform/Core/Renderer/RenderBlock/ARenderBlock.h"
#include <Platform/Core/LObject.h>
#include <LCommon.h>

namespace Lemonade 
{
    class LEMONADE_API RenderComponent : public LObject
    {
    public:
        RenderComponent();
        
        virtual bool Init();
        virtual void Unload();
        virtual void Update();
        virtual void Render();
    private:
        std::unique_ptr<ARenderBlock> m_renderBlock;
    };
}