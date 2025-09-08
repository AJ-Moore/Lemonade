

#include "Geometry/Rect.h"
#include "Platform/Core/Renderer/RenderBlock/ARenderBlock.h"
#include "Platform/Vulkan/Materials/Texture.h"
#include <Platform/Core/Renderer/Geometry/Mesh.h>
#include <Platform/Core/Renderer/Pipeline/LRenderLayer.h>
#include <memory>

#ifdef RENDERER_VULKAN
#include <Platform/Vulkan/Renderer/LRenderBlock.h>
#endif 

#ifdef RENDERER_OPENGL
#include <Platform/OGL/Renderer/LRenderBlock.h>
#endif

namespace Lemonade 
{
	bool LRenderLayer::Init()
	{
		const auto vertices = std::make_shared<std::vector<glm::vec3>>(std::vector<glm::vec3>
		{
			{-1, -1, 0},
			{-1,  1, 0},
			{ 1,  1, 0},
			{-1, -1, 0},
			{ 1,  1, 0},
			{ 1, -1, 0}
		});

		const auto uvs = std::make_shared<std::vector<glm::vec2>>(std::vector<glm::vec2>
		{
			{0, 0},
			{0, 1},
			{1, 1},
			{0, 0},
			{1, 1},
			{1, 0},
		});

		m_renderBlock = std::make_shared<LRenderBlock>();

		m_renderBlock->SetMaterial(m_material);
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		mesh->SetShouldGenerateTangents(false);
		mesh->SetVertices(vertices);
		mesh->SetUVS(uvs);
		m_renderBlock->SetMesh(mesh);
		m_renderBlock->Init();
        return true;
	}

	void LRenderLayer::Render()
	{
		//m_material->getShader().setUniformfv("width", window.Width);
		//m_material->getShader().setUniformfv("height", window.Height);
        // BIND SSBO with layer dimensions?
		m_renderBlock->OnPipelineBound.AddListener([this](ARenderBlock* block){
			if (m_sourceTarget != nullptr)
			{
				//SetRenderBlock ugliness -> Hopefully temporary but who knows, needed access to descriptor/ vertex data for updating descriptor with target.
				m_sourceTarget->SetRenderBlock(block);
				m_sourceTarget->BindColourAttachments();
			}
		});
		m_renderBlock->Render();
	}

	void LRenderLayer::Update()
	{
		m_renderBlock->Update();
	}
}
