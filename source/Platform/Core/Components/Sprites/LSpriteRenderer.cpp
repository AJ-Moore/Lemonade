
#include "Platform/Core/Renderer/Geometry/Mesh.h"
#include "Resources/ResourceHandle.h"
#include <Platform/Core/Components/Sprites/LSpriteRenderer.h>
#include <memory>

namespace Lemonade {
    bool LSpriteRenderer::Init()
    {
        if (!LMeshRenderer::Init())
        {
            return false;
        }

        const std::shared_ptr<std::vector<glm::vec3>> vertices = std::make_shared<std::vector<glm::vec3>>(std::vector<glm::vec3>
        {
            { -0.5f, -0.5f, 0 },
            { -0.5f,  0.5f, 0 },
            {  0.5f,  0.5f, 0 },
            { -0.5f, -0.5f, 0 },
            {  0.5f,  0.5f, 0 },
            {  0.5f, -0.5f, 0 },
        });

        m_uvs = std::make_shared<std::vector<glm::vec2>>(std::vector<glm::vec2>
        {
            { 0.0f, 0.0f,},
            { 0.0f, 1.0f,},
            { 1.0f, 1.0f,},
            { 0.0f, 0.0f,},
            { 1.0f, 1.0f,},
            { 1.0f, 0.0f,},
        });

        const std::shared_ptr<std::vector<glm::vec3>> normals = std::make_shared<std::vector<glm::vec3>>(std::vector<glm::vec3>
        {
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f },
        });

        SetFrame(0);

        m_mesh = std::make_shared<Mesh>();
        m_mesh->SetShouldGenerateTangents(false);
        m_mesh->SetVertices(vertices);
        m_mesh->SetUVS(m_uvs);
        //m_mesh->SetNormals(normals);
        SetMesh(m_mesh);
        return true;
    }

    LSpriteRenderer::LSpriteRenderer(CitrusCore::ResourcePtr<Material> material)
    {
        SetMaterial(material);
    }

    void LSpriteRenderer::SetMaterial(CitrusCore::ResourcePtr<Material> material)
    {
        LMeshRenderer::SetMaterial(material);
        SetFrame(0);
    }

    void LSpriteRenderer::SetFrame(uint32 frame)
	{
		if (m_material == nullptr || m_uvs == nullptr)
		{
			return;
		}
//todo
		// Get the texture coords for the current frame!
		//std::array<glm::vec2, 4> coords = m_material->GetTextureCoordsForFrame(frame);
//
		//(*m_uvs)[0] = coords[0];
		//(*m_uvs)[1] = coords[1];
		//(*m_uvs)[2] = coords[2];
		//(*m_uvs)[3] = coords[0];
		//(*m_uvs)[4] = coords[2];
		//(*m_uvs)[5] = coords[3];
//
		//this->setDirty();
	}
}