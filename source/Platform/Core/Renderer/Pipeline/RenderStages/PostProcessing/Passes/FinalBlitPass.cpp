#include "Platform/Core/Services/GraphicsServices.h"
#include <Platform/Vulkan/Renderer/LRenderTarget.h>
#include <Platform/Vulkan/WindowManager/LWindow.h>
#include <Platform/Core/Renderer/Pipeline/RenderStages/PostProcessing/Passes/FinalBlitPass.h>

namespace Lemonade
{
    FinalBlitPass::FinalBlitPass() : 
        m_renderLayer(GraphicsServices::GetGraphicsResources()->GetMaterialHandle("Assets/Materials/Fullscreen.mat.json"))
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

        screenTarget->BeginRenderPass();
        screenTarget->Clear((uint)LBufferBit::COLOUR);
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