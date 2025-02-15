#pragma once
#include <UCommon.h>

#ifdef RENDERER_VULKAN
#include <Renderer/ARenderBlock.h>

namespace UpsilonEngine
{
	class DLLExport URenderer;

	/** URender Block Class - Used to store render state information */
	class DLLExport URenderBlock : public ARenderBlock
	{
		friend class URenderLayer;
		friend class URenderer;
		friend class UMeshRenderer;
	public:
		URenderBlock() {}
		virtual ~URenderBlock() {}

		/// Sets the draw mode used by this block.
		virtual void setDrawMode(uPrimitiveMode mode) { m_primitiveMode = (uint32)mode; }

		void setTransform(SharedPtr<UTransform> Transform) { m_transform = Transform; }
		void setMaterial(SharedPtr<UMaterial> Material){ m_material = Material; }
		void removeRenderState(URenderState RenderState) { m_renderState &= ~(uint32)RenderState; }
		void setRenderState(URenderState RenderState) { m_renderState |= (uint32)RenderState; }
		void setMesh(SharedPtr<UMesh> Mesh) { m_mesh = Mesh; };
		void setDirty() { m_bufferDirty = true; }
		uPrimitiveMode getDrawMode() const { return (uPrimitiveMode)m_primitiveMode; }

	protected:
		/// Called to perform initialisation 
		virtual bool init() { return true; }

		/// Called prior to object deletion to perform any unload actions
		virtual void unload() {}

		/// Called prior to object deletion when an object has been called to be destroyed.
		virtual void onDestroy() {}

		/// Called to perform buffer updates
		virtual void update() {}

		/// Called to render the block 
		virtual void render() {}

		/// Dumbs the vertex data to the buffer
		virtual void dumpBufferData() {}

		/// Dmps the animation buffer data 
		virtual void dumpAnimationData() {}

		// Set/ configure vertex attributes.
		virtual void setVertexAttributes(){}

		// Should be moved to OGL render block, unless applicable to other renderers.
		void vertexAttribPointer(GLuint shaderProgram, std::string name, GLint size, GLuint type, GLboolean normalized, GLsizei stride, const void* pointer) {}
		GLint enableVertexAttribArray(GLuint shaderProgram, std::string name) { return -1; }

		VkBuffer m_vertexBuffer;

		static GLuint m_animationBuffer;
		static int m_animationSize;
		GLuint m_animationLockBuffer = 0;
		GLuint m_animationBufferTextureId = 0;
	};
}
#endif
