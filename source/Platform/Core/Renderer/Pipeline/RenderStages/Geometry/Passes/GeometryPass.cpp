
#include "Platform/Core/Renderer/Pipeline/ARenderTarget.h"
#include "Platform/Core/Services/GraphicsServices.h"
#include <Platform/Core/Renderer/Pipeline/RenderStages/Geometry/Passes/GeometryPass.h>
namespace Lemonade
{
    GeometryPass::GeometryPass() : 
        m_renderLayer(GraphicsServices::GetGraphicsResources()->GetMaterialHandle("Assets/Materials/fullscreen.mat.json"))
    {

    }

    bool GeometryPass::Init() 
    {
        m_renderLayer.Init();
        m_geometryTarget.Init();
		m_geometryTarget.SetColourAttachments(1, false);
        //m_geometryTarget.AddDepthAttachment();
        return true;
    }

    void GeometryPass::Render( const LRenderingData& renderingData)
    {
        m_geometryTarget.BeginRenderPass();
        m_geometryTarget.setClearColour(glm::vec4(0,1,0,1));
        m_geometryTarget.Clear((uint)LBufferBit::COLOUR);// | (uint)LBufferBit::DEPTH);
        GraphicsServices::GetRenderer()->RenderScene();
        //m_renderLayer.Render();
        m_geometryTarget.EndRenderPass();
    }

    void GeometryPass::Update() 
    {
        m_renderLayer.Update();
    }
}