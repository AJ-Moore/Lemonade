#pragma once

#include <Containers/IndexedVector.h>
#include <Util/UID.h>
#include <Platform/Core/Renderer/Pipeline/AUniformBuffer.h>
#include <Events/Event.h>
#include <Resources/ResourceHandle.h>
#include <Spatial/Transform.h>
#include <LCommon.h>
#include <Platform/Core/LObject.h>
#include <Platform/Core/Renderer/Geometry/Mesh.h>
#include <Platform/Core/Renderer/Materials/Material.h>
#include <Platform/Core/Renderer/Geometry/PrimitiveMode.h>
#include <memory>

namespace Lemonade
{
	struct ShaderProperties
	{
		glm::mat4 wvpShadowMat;
		glm::mat4 modelMat; 
		glm::mat4 viewMat; 
		glm::mat4 projMat;
		bool shadowPass = false;
		glm::vec3 cameraPosition;
		glm::vec3 cameraLook;
	};

	class Renderer;

	/** Render Block */
	class LEMONADE_API ARenderBlock : public LObject
	{
		friend class LRenderLayer;
		friend class LRenderer;
		friend class RenderComponent;
	public:
		ARenderBlock(){
			// Fallback.
			m_transform = std::make_shared<CitrusCore::Transform>();
		}
		virtual ~ARenderBlock() {}

		virtual void SetDrawMode(PrimitiveMode mode) = 0;
		virtual void SetMaterial(CitrusCore::ResourcePtr<Material> Material) { m_material = Material; }
		void SetTransform(std::shared_ptr<CitrusCore::Transform> Transform) { m_transform = Transform; }
		void SetMesh(std::shared_ptr<Mesh> Mesh) { m_mesh = Mesh; }
		void SetDirty() { m_bufferDirty = true; }
		PrimitiveMode GetDrawMode() const { return (PrimitiveMode)m_primitiveMode; }

		CitrusCore::Event<ARenderBlock*> OnPipelineBound;

		void SetBlendEnabled(bool value){m_bBlendEnabled = value;}
		bool AddUniformBuffer(std::shared_ptr<AUniformBuffer>);
		bool RemoveUniformBuffer(CitrusCore::UID uniformBufferuid);

		CitrusCore::ResourcePtr<Material> GetMaterial() const { return m_material; }
	protected:
		/// Dumps the vertex data to the buffer
		virtual void DumpBufferData() = 0;

		/// Dump animation data.
		virtual void DumpAnimationData() {}

		// Set/ configure vertex attributes.
		virtual void ConfigureVertexAttributes() {};

		/// Bound buffer used by this render block
		uint32 m_vertexArrayObject = 0;
		uint32 m_bufferId = 0;

		/// Does the buffer need an update 
		bool m_bufferDirty = true;

		/// Do we need to update the animation data.
		bool m_animationBufferDirty = true;

		/// The transform of the component that we are rendering
		std::shared_ptr<CitrusCore::Transform> m_transform;

		/// The mesh being rendered
		std::shared_ptr<Mesh> m_mesh;

		/// The material used by this mesh 
		CitrusCore::ResourcePtr<Material>  m_material;

		/// Holds all the render state information
		uint32 m_renderState = 0;

		/// Holds the primitive mode, 0x4 triangles ogl, lazy
		uint32 m_primitiveMode = 0x4;

		/// Reference to the renderer
		Renderer* m_renderer = nullptr;

		bool m_bBlendEnabled = true;

		/// Uniform buffers to be passed to shader
		CitrusCore::IndexedVector<CitrusCore::UID, std::shared_ptr<AUniformBuffer>> m_uniformBuffers;
	};
}
