#include "Platform/Core/Renderer/Pipeline/ARenderTarget.h"
#include "Platform/Core/Services/GraphicsServices.h"
#include <Platform/Vulkan/Renderer/LRenderTarget.h>
#include <Platform/Vulkan/WindowManager/LWindow.h>
#include <Platform/Core/Renderer/Pipeline/RenderStages/PostProcessing/Passes/FinalBlitPass.h>
#include <glm/fwd.hpp>

namespace Lemonade
{
    FinalBlitPass::FinalBlitPass() : 
        m_renderLayer(GraphicsServices::GetGraphicsResources()->GetMaterialHandle("Assets/Materials/fullscreen.mat.json"))
    {

    }

    bool FinalBlitPass::Init()
    {
        m_renderLayer.Init();
        return true;
    }

    void FinalBlitPass::Render( const LRenderingData& renderingData)
    {
        // Get the screen target 
        LRenderTarget* screenTarget = static_cast<LRenderTarget*>(LRenderTarget::GetDefault().GetScreenTarget(renderingData.ActiveWindow));

        ARenderTarget* previousTarget = GraphicsServices::GetRenderer()->GetActiveRenderTarget();
        m_renderLayer.SetRenderTarget(GraphicsServices::GetRenderer()->GetActiveRenderTarget());

        screenTarget->BeginRenderPass();
        screenTarget->setClearColour(glm::vec4(1,0,1,1));
        //previousTarget->bindColourAttachments();
        //screenTarget->bindColourAttachments();
        //screenTarget->BindColourAttachment(LColourAttachment::LEMON_COLOR_ATTACHMENT0);
        //screenTarget->Clear((uint)LBufferBit::COLOUR);
        m_renderLayer.Render();
        screenTarget->EndRenderPass();
//
        //m_geometryTarget.BeginRenderPass();
        //m_geometryTarget.Clear((uint)LBufferBit::COLOUR | (uint)LBufferBit::DEPTH);
        //GraphicsServices::GetRenderer()->RenderScene();
        //m_geometryTarget.EndRenderPass();
    }

    void FinalBlitPass::Update()
    {
        m_renderLayer.Update();
    }
}