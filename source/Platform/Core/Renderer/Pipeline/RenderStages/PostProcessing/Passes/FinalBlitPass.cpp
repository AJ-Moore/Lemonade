#include <Platform/Core/Renderer/Pipeline/ARenderTarget.h>
#include <Platform/Core/Services/GraphicsServices.h>
#include <Platform/Vulkan/Renderer/LRenderTarget.h>
#include <Platform/Vulkan/WindowManager/LWindow.h>
#include <Platform/Core/Renderer/Pipeline/RenderStages/PostProcessing/Passes/FinalBlitPass.h>
#include <glm/fwd.hpp>

namespace Lemonade
{
    FinalBlitPass::FinalBlitPass() : 
        m_renderLayer(GraphicsServices::GetGraphicsResources()->GetMaterialHandle("Assets/Materials/fullscreen.mat.json")),
        m_sky(GraphicsServices::GetGraphicsResources()->GetMaterialHandle("Assets/Materials/skyray.mat.json"))
    {

    }

    bool FinalBlitPass::Init()
    {
        m_renderLayer.Init();
        m_sky.Init();
        m_sky.SetBlendEnabled(false);
        return true;
    }

    void FinalBlitPass::Render( const LRenderingData& renderingData)
    {
        // Get the screen target 
        LRenderTarget* screenTarget = static_cast<LRenderTarget*>(LRenderTarget::GetDefault().GetScreenTarget(renderingData.ActiveWindow));

        ARenderTarget* previousTarget = GraphicsServices::GetRenderer()->GetActiveRenderTarget();
        m_renderLayer.SetRenderTarget(previousTarget);

        screenTarget->BeginRenderPass();
        screenTarget->setClearColour(glm::vec4(0,0,0,1));
        m_sky.Render();
        m_renderLayer.Render();
        screenTarget->EndRenderPass();
    }

    void FinalBlitPass::Update()
    {
        m_sky.Update();
        m_renderLayer.Update();
    }
}