#include <Platform/Core/Components/Shapes/LCube.h>
#include <glm/fwd.hpp>
#include <memory>
#include <vector>

namespace Lemonade
{
    LCube::LCube(CitrusCore::ResourcePtr<Material> material, glm::vec3 dimensions)
    {
        m_dimensions = dimensions;
        SetMaterial(material);
    }

    bool LCube::Init()
    {
        const std::shared_ptr<std::vector<glm::vec3>> CubeVertices = std::make_shared<std::vector<glm::vec3>>(std::vector<glm::vec3>
        {
            // Top
            {-0.5f, 0.5f, -0.5f},
            { -0.5f, 0.5f, 0.5f },
            { 0.5f, 0.5f, 0.5f },
            { -0.5f, 0.5f, -0.5f },
            { 0.5f, 0.5f, 0.5f },
            { 0.5f, 0.5f, -0.5f },
    
            // Bottom
            { 0.5f, -0.5f, -0.5f },
            { 0.5f, -0.5f, 0.5f },
            { -0.5f, -0.5f, 0.5f },
            { 0.5f, -0.5f, -0.5f },
            { -0.5f, -0.5f, 0.5f },
            { -0.5f, -0.5f, -0.5f },
    
            // Front
            { -0.5f, -0.5f, -0.5f },
            { -0.5f, 0.5f, -0.5f },
            { 0.5f, 0.5f, -0.5f },
            { -0.5f, -0.5f, -0.5f },
            { 0.5f, 0.5f, -0.5f },
            { 0.5f, -0.5f, -0.5f },
    
            // Back
            { 0.5f, -0.5f, 0.5f },
            { 0.5f, 0.5f, 0.5f },
            { -0.5f, 0.5f, 0.5f },
            { 0.5f, -0.5f, 0.5f },
            { -0.5f, 0.5f, 0.5f },
            { -0.5f, -0.5f, 0.5f },
    
            // Left
            { -0.5f, -0.5f, 0.5f },
            { -0.5f, 0.5f, 0.5f },
            { -0.5f, 0.5f, -0.5f },
            { -0.5f, -0.5f, 0.5f },
            { -0.5f, 0.5f, -0.5f },
            { -0.5f, -0.5f, -0.5f },
    
            // Right
            { 0.5f, 0.5f, 0.5f },
            { 0.5f, -0.5f, 0.5f },
            { 0.5f, -0.5f, -0.5f },
            { 0.5f, 0.5f, 0.5f },
            { 0.5f, -0.5f, -0.5f },
            { 0.5f, 0.5f, -0.5f },
        });
    
        const std::shared_ptr<std::vector<glm::vec3>> CubeNormals = std::make_shared<std::vector<glm::vec3>>(std::vector<glm::vec3>
        {
            {0.0f, 1.0f, 0.0f},
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
    
            { 0.0f, -1.0f, 0.0f },
            { 0.0f, -1.0f, 0.0f },
            { 0.0f, -1.0f, 0.0f },
            { 0.0f, -1.0f, 0.0f },
            { 0.0f, -1.0f, 0.0f },
            { 0.0f, -1.0f, 0.0f },
    
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f },
    
            { 0.0f, 0.0f, -1.0f },
            { 0.0f, 0.0f, -1.0f },
            { 0.0f, 0.0f, -1.0f },
            { 0.0f, 0.0f, -1.0f },
            { 0.0f, 0.0f, -1.0f },
            { 0.0f, 0.0f, -1.0f },
    
            { 1.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 0.0f },
    
            { -1.0f, 0.0f, 0.0f },
            { -1.0f, 0.0f, 0.0f },
            { -1.0f, 0.0f, 0.0f },
            { -1.0f, 0.0f, 0.0f },
            { -1.0f, 0.0f, 0.0f },
            { -1.0f, 0.0f, 0.0f }
        });
    
        const std::shared_ptr<std::vector<glm::vec2>> UVS = std::make_shared<std::vector<glm::vec2>>(std::vector<glm::vec2>
        {
            { 0.0f, 0.0f,},
            { 0.0f, 1.0f,},
            { 1.0f, 1.0f,},
            { 1.0f, 0.0f,},
        });

        LMeshRenderer::Init();
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

        std::shared_ptr<std::vector<glm::vec3>> vertices = std::make_shared<std::vector<glm::vec3>>();

        for (const auto& vertex : *CubeVertices)
        {
            vertices->push_back(vertex * m_dimensions);
        }

        mesh->SetVertices(vertices);
        mesh->SetShouldGenerateTangents(false);
        mesh->SetNormals(CubeNormals);

        SetMesh(mesh);

        //if (m_colour.a != 1)
        //{
        //    getParent()->setRenderPriority((uint)URenderPriority::Transparent);
        //}

        std::shared_ptr<std::vector<glm::vec2>> coords = std::make_shared<std::vector<glm::vec2>>();

        for (int i = 0; i < 6; i++)
        {
            coords->push_back(UVS->at(0));
            coords->push_back(UVS->at(1));
            coords->push_back(UVS->at(2));
            coords->push_back(UVS->at(0));
            coords->push_back(UVS->at(2));
            coords->push_back(UVS->at(3));
        }
        mesh->SetUVS(coords);
        return true;
    }
}
