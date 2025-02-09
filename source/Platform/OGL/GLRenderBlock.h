#pragma once

#ifdef RENDERER_OPENGL

#include <Common.h>
#include <Renderer/ARenderBlock.h>
#include <Renderer/PrimitiveMode.h>
#include <Renderer/Transform.h>

namespace CraftyBlocks
{
	class RenderBlock : public ARenderBlock
	{
		friend class RenderLayer;
		friend class Renderer;
		friend class MeshRenderer;
	public:
		void SetTransform(const std::shared_ptr<Transform>& transform) { m_transform = transform; }
		void SetMaterial(const std::shared_ptr<Material>& material);
		void SetMesh(const std::shared_ptr<Mesh>& mesh) { m_mesh = mesh; };
		void SetDirty() { m_bufferDirty = true; }
		virtual void SetDrawMode(PrimitiveMode mode);
		PrimitiveMode GetDrawMode() const { return (PrimitiveMode)m_primitiveMode; }
	protected:
		virtual bool Init();
		virtual void Unload() {}
		virtual void Update();
		virtual void Render();

		/// Dumps the vertex data to the buffer.
		virtual void DumpBufferData();

		/// Dumps the animation buffer data.
		virtual void DumpAnimationData();

		// Set/ configure vertex attributes.
		virtual void ConfigureVertexAttributes();

		void VertexAttribPointer(GLuint shaderProgram, std::string name, GLint size, GLuint type, GLboolean normalized, GLsizei stride, const void* pointer);
		GLint EnableVertexAttribArray(GLuint shaderProgram, std::string name);

		static GLuint m_animationBuffer;
		static int m_animationSize;
		GLuint m_animationLockBuffer = 0;
		GLuint m_animationBufferTextureId = 0;
		bool m_bDoneInit = false;

		GLint m_shadowMat;
		GLint m_modelUniform;
		GLint m_viewUniform;
		GLint m_projUniform;
		GLint m_shadowUniform;
		GLint m_cameraUniform;
		GLint m_cameraLook;
	};
}
#endif
