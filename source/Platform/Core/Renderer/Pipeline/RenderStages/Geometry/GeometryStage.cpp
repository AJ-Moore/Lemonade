#include <Platform/Core/Renderer/Pipeline/RenderStages/Geometry/GeometryStage.h>
#include <Platform/Core/Renderer/Pipeline/RenderStages/Geometry/Passes/GeometryPass.h>
#include <memory>

namespace Lemonade {

    bool GeometryStage::Init() 
    {
        AddPass(std::make_shared<GeometryPass>());
        return true;
    }
}