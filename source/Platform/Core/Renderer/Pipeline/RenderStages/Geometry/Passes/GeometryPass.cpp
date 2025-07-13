
#include "Platform/Core/Renderer/Pipeline/ARenderTarget.h"
#include "Platform/Core/Services/GraphicsServices.h"
#include <Platform/Core/Renderer/Pipeline/RenderStages/Geometry/Passes/GeometryPass.h>
namespace Lemonade
{
    bool GeometryPass::Init() 
    {
        m_geometryTarget.Init();
		m_geometryTarget.SetColourAttachments(1, true);
        m_geometryTarget.AddDepthAttachment();
        return true;
    }

    void GeometryPass::Render( const LRenderingData& renderingData)
    {
        m_geometryTarget.BeginRenderPass();
        m_geometryTarget.Clear((uint)LBufferBit::COLOUR | (uint)LBufferBit::DEPTH);
        //GraphicsServices::GetRenderer()->RenderScene();
        m_geometryTarget.EndRenderPass();
    }
}