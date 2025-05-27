#pragma once
#include <LCommon.h>

#ifdef RENDERER_VULKAN

#include <Platform/Core/Renderer/Pipeline/AViewport.h>

namespace Lemonade 
{
    class LEMONADE_API LViewport : public AViewport
    {
    protected:
        virtual void SetViewport(int x, int y, int width, int height);
        virtual void SetScissor(int x, int y, int width, int height); 
        virtual void SetDepthRange(float near, float far);
    };
}

#endif 