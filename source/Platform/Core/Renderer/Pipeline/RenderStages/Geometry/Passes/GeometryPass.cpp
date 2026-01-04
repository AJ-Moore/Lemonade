#include <Platform/Core/Renderer/Pipeline/ARenderTarget.h>
#include <Platform/Core/Services/GraphicsServices.h>
#include <Platform/Core/Renderer/Pipeline/RenderStages/Geometry/Passes/GeometryPass.h>

namespace Lemonade
{
    GeometryPass::GeometryPass() : 
        m_renderLayer(GraphicsServices::GetGraphicsResources()->GetMaterialHandle("Assets/Materials/deferred.mat.json"))
    {

    }

    bool GeometryPass::Init() 
    {
        m_renderLayer.Init();
        m_geometryTarget.Init();
        m_gBuffer.Init();
        m_gBuffer.SetColourAttachments(1, false);
		m_geometryTarget.SetColourAttachments(4, false);
        m_geometryTarget.AddDepthAttachment();
        return true;
    }

    void GeometryPass::Render( const LRenderingData& renderingData)
    {
        m_geometryTarget.BeginRenderPass();
        m_geometryTarget.setClearColour(glm::vec4(0,0,0, 0));
        m_geometryTarget.Clear((uint)LBufferBit::COLOUR);// | (uint)LBufferBit::DEPTH);
        GraphicsServices::GetRenderer()->RenderScene();
        m_geometryTarget.EndRenderPass();
        
        m_gBuffer.BeginRenderPass();
        m_gBuffer.setClearColour(glm::vec4(0,0,0, 0));
        m_gBuffer.Clear((uint)LBufferBit::COLOUR);
        m_renderLayer.SetRenderTarget(&m_geometryTarget);
        m_renderLayer.Render();
        m_gBuffer.EndRenderPass();
    }

    void GeometryPass::Update() 
    {
        m_renderLayer.Update();
    }
}