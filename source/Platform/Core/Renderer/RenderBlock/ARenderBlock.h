#pragma once

#include <Common.h>
#include <Core/Interface/IManagedObject.h>
#include <Renderer/Transform.h>
#include <Renderer/Mesh.h>
#include <Renderer/Material.h>
#include <Renderer/PrimitiveMode.h>

namespace Lemonade
{
	class Renderer;

	/** Render Block */
	class LEMONADE_API ARenderBlock : public IManagedObject
	{
		friend class URenderLayer;
		friend class URenderer;
		friend class UMeshRenderer;
	public:
		ARenderBlock() = default;
		virtual ~ARenderBlock() {}

		virtual void SetDrawMode(PrimitiveMode mode) = 0;
		virtual void SetMaterial(std::shared_ptr<Material> Material) = 0;
		void SetTransform(std::shared_ptr<Transform> Transform) { m_transform = Transform; }
		void SetMesh(std::shared_ptr<Mesh> Mesh) { m_mesh = Mesh; }
		void SetDirty() { m_bufferDirty = true; }
		PrimitiveMode GetDrawMode() const { return (PrimitiveMode)m_primitiveMode; }
	protected:
		/// Dumps the vertex data to the buffer
		virtual void DumpBufferData() {};

		/// Dump animation data.
		virtual void DumpAnimationData() {}

		// Set/ configure vertex attributes.
		virtual void ConfigureVertexAttributes() {};

		// Should be moved to OGL render block, unless applicable to other renderers.
		void VertexAttribPointer(GLuint shaderProgram, std::string name, GLint size, GLuint type, GLboolean normalized, GLsizei stride, const void* pointer) {}
		GLint EnableVertexAttribArray(GLuint shaderProgram, std::string name) { return -1; }

		/// Bound buffer used by this render block
		uint32 m_vertexArrayObject = 0;
		uint32 m_bufferId = 0;

		/// Does the buffer need an update 
		bool m_bufferDirty = true;

		/// Do we need to update the animation data.
		bool m_animationBufferDirty = true;

		/// The transform of the component that we are rendering
		std::shared_ptr<Transform> m_transform;

		/// The mesh being rendered
		std::shared_ptr<Mesh> m_mesh;

		/// The material used by this mesh 
		std::shared_ptr<Material> m_material;

		/// Holds all the render state information
		uint32 m_renderState = 0;

		/// Holds the primitive mode, 0x4 triangles ogl, lazy
		uint32 m_primitiveMode = 0x4;

		/// Reference to the renderer
		Renderer* m_renderer = nullptr;
	};
}
