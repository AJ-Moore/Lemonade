#include <Platform/Core/Components/Shapes/LLines.h>
#include <glm/fwd.hpp>
#include <memory>
#include <vector>

namespace Lemonade
{
    LLines::LLines(CitrusCore::ResourcePtr<Material> material, std::shared_ptr<std::vector<glm::vec3>> vertices)
    {
        m_vertices = vertices;
        SetMaterial(material);
    }

    bool LLines::Init()
    {
        LMeshRenderer::Init();
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

        mesh->SetVertices(m_vertices);
        mesh->SetShouldGenerateTangents(false);
        SetMesh(mesh);

        return true;
    }
}
