

#include "Geometry/Rect.h"
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
	void LRenderLayer::Init()
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
		mesh->SetTextureCoords(uvs);
		m_renderBlock->SetMesh(mesh);
		m_renderBlock->Init();
	}

	void LRenderLayer::Render()
	{
		CitrusCore::Rect window = UServiceLocator::getInstance()->getWindowManager()->getMainWindow()->GetWindowRect();
		//m_material->getShader().setUniformfv("width", window.Width);
		//m_material->getShader().setUniformfv("height", window.Height);
		m_renderBlock->Render();
	}

	void LRenderLayer::Update()
	{
		m_renderBlock->Update();
	}
}
