
#include <Platform/Core/Components/RenderComponent.h>
#include <memory>

#ifdef RENDERER_OPENGL
#include <Platform/OGL/Renderer/LRenderBlock.h>
#endif

#ifdef RENDERER_VULKAN
#include <Platform/Vulkan/Renderer/LRenderBlock.h>
#endif

#ifdef RENDERER_PS5
#include <Platform/PS5/Renderer/LRenderBlock.h>
#endif

namespace Lemonade{

    RenderComponent::RenderComponent()
    {
        m_renderBlock = std::make_unique<LRenderBlock>();
    }

    bool RenderComponent::Init()
    {
        m_renderBlock->Init();
        return true;
    }

    void RenderComponent::Unload()
    {
        m_renderBlock->Unload();
    }

    void RenderComponent::Update()
    {
        m_renderBlock->Update();
    }

    void RenderComponent::Render()
    {
        m_renderBlock->Render();
    }

    void RenderComponent::SetTransform(std::shared_ptr<CitrusCore::Transform> trans)
    {
        m_transform = trans;
        m_renderBlock->SetTransform(trans);
    }
}