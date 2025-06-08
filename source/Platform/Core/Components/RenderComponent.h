#pragma once
#include "Platform/Core/Renderer/RenderBlock/ARenderBlock.h"
#include "Spatial/Transform.h"
#include <Platform/Core/LObject.h>
#include <LCommon.h>
#include <memory>

namespace Lemonade 
{
    class LEMONADE_API RenderComponent : public LObject
    {
    public:
        RenderComponent();
        
        virtual bool Init() override;
        virtual void Unload() override;
        virtual void Update() override;
        virtual void Render() override;

        void AssignTransform(std::shared_ptr<CitrusCore::Transform> trans);
    protected:
        std::unique_ptr<ARenderBlock> m_renderBlock;
        std::shared_ptr<CitrusCore::Transform> m_transform;
    };
}