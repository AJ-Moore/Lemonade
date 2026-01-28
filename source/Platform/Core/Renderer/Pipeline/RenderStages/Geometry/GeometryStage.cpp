#include <Platform/Core/Renderer/Pipeline/RenderStages/Shadows/Passes/ShadowPass.h>
#include <Platform/Core/Renderer/Pipeline/RenderStages/Geometry/GeometryStage.h>
#include <Platform/Core/Renderer/Pipeline/RenderStages/Geometry/Passes/GeometryPass.h>
#include <memory>

namespace Lemonade {

    bool GeometryStage::Init() 
    {
        std::shared_ptr<ShadowPass> shadowPass = std::make_shared<ShadowPass>();
        std::shared_ptr<GeometryPass> geometryPass = std::make_shared<GeometryPass>();

        geometryPass->SetShadowPass(shadowPass);

        AddPass(shadowPass);
        AddPass(geometryPass);
        return true;
    }
}