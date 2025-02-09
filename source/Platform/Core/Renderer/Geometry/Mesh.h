#pragma once 

#include <Common.h>
#include <memory>
#include <vector>
#include <Renderer/PrimitiveMode.h>
#include <Renderer/Bone.h>
#include <Renderer/Animation.h>

namespace CraftyBlocks
{
	constexpr int Vec3Size() { return sizeof(glm::vec3) / sizeof(float); };
	constexpr int Vec4Size() { return sizeof(glm::vec4) / sizeof(float); };

	class Mesh
	{
		friend class ARenderBlock;
		friend class RenderBlock;
	public:
		void Clear();

		void SetVertices(std::shared_ptr<std::vector<glm::vec3>> vertices) { m_bDirtyTangents = true; m_vertices = vertices; GenerateTangents(); }
		void SetNormals(std::shared_ptr<std::vector<glm::vec3>> normals) { m_normals = normals; }
		void SetTangents(std::shared_ptr<std::vector<glm::vec3>> tangents) { m_bShouldGenerateTangents = false; m_tangents = tangents; }
		void SetBiTangents(std::shared_ptr<std::vector<glm::vec3>> tangents) { m_bShouldGenerateTangents = false; m_bitangents = tangents; }
		void SetTextureCoords(std::shared_ptr<std::vector<glm::vec2>> textureCoords) { m_bDirtyTangents = true; m_uvs = textureCoords; }
		void SetTexture3dCoords(std::shared_ptr<std::vector<glm::vec3>> textureCoords) { m_uvs3d = textureCoords; }
		void SetColours(std::shared_ptr<std::vector<glm::vec4>> Colours) { m_colours = Colours; }
		void setBones(std::shared_ptr<std::vector<std::shared_ptr<Bone>>> bones) { m_bones = bones; }
		void setBoneWeights(std::shared_ptr<std::vector<glm::vec4>> bones) { m_boneWeights = bones; }
		void setBoneIds(std::shared_ptr<std::vector<glm::vec4>> ids) { m_boneIds = ids; }
		void setAnimation(std::shared_ptr<std::vector<std::shared_ptr<Animation>>> anim) { m_animationsRef = anim; }

		void SetDrawMode(PrimitiveMode mode) { m_drawMode = mode; }
		PrimitiveMode GetDrawMode() const { return m_drawMode; }
		float GetLineThickness() const { return m_lineThickness; }
		void SetLineThickness(const float& value) { m_lineThickness = value; }

		void CalculateNormalsPerFace();
		void CalculateNormalsPerVertex();
		bool ShouldRenderBackFaces() { return m_bRenderBackfaces; }
		void SetShouldRenderBackFaces(bool b) { m_bRenderBackfaces = b; }
		void SetShouldGenerateTangents(bool value) { m_bShouldGenerateTangents = value; }

		const std::shared_ptr<std::vector<glm::vec3>> GetTangents();
		const std::shared_ptr<std::vector<glm::vec3>> GetBiTangents();
		const std::shared_ptr<std::vector<glm::vec3>> GetNormals() const { return m_normals; }
		const std::shared_ptr<std::vector<glm::vec3>> GetVertices() const { return m_vertices; }
		const std::shared_ptr<std::vector<glm::vec4>> GetBoneWeights() const { return m_boneWeights; }
		const std::shared_ptr<std::vector<glm::vec4>> GetBoneIds() const { return m_boneIds; }

		size_t GetVertexBufferSize() { return (m_vertices) ? m_vertices->size() * Vec3Size() : 0; }
		size_t GetTextureBufferSize() { return (m_uvs) ? m_uvs->size() * 2 : 0; }
		size_t GetTexture3dBufferSize() { return (m_uvs3d) ? m_uvs3d->size() * Vec3Size() : 0; }
		size_t GetNormalBufferSize() { return (m_normals) ? m_normals->size() * Vec3Size() : 0; }
		size_t GetColourBufferSize() { return (m_colours) ? m_colours->size() * Vec4Size() : 0; }
		size_t GetTangentBufferSize() { return (m_tangents) ? m_tangents->size() * Vec3Size() : 0; }
		size_t GetBiTangentBufferSize() { return (m_bitangents) ? m_bitangents->size() * Vec3Size() : 0; }
		size_t GetWeightBufferSize() { return (m_boneWeights) ? m_boneWeights->size() * Vec4Size() : 0; }
		size_t GetBoneIdBufferSize() { return (m_boneIds) ? m_boneIds->size() * Vec4Size() : 0; }
		size_t GetVertexSize() { return (m_vertices) ? m_vertices->size() : 0; }
		size_t GetTextureSize() { return (m_uvs) ? m_uvs->size() : 0; }
		size_t GetTexture3DSize() { return (m_uvs3d) ? m_uvs3d->size() : 0; }
		size_t GetNormalSize() { return (m_normals) ? m_normals->size() : 0; }
		size_t GetColourSize() { return (m_colours) ? m_colours->size() : 0; }
		size_t GetTangentSize() { return (m_tangents) ? m_tangents->size() : 0; }
		size_t GetBiTangentSize() { return (m_bitangents) ? m_bitangents->size() : 0; }
	private:
		void GenerateTangents();

		std::shared_ptr<std::vector<glm::vec3>> m_vertices = nullptr;
		std::shared_ptr<std::vector<glm::vec3>> m_normals = nullptr;
		std::shared_ptr<std::vector<glm::vec3>> m_faceNormals = nullptr;
		std::shared_ptr<std::vector<glm::vec4>> m_colours = nullptr;
		std::shared_ptr<std::vector<glm::vec2>> m_uvs = nullptr;
		std::shared_ptr<std::vector<glm::vec3>> m_uvs3d = nullptr;
		std::shared_ptr<std::vector<glm::vec3>> m_tangents = nullptr;
		std::shared_ptr<std::vector<glm::vec3>> m_bitangents = nullptr;
		std::shared_ptr<std::vector<glm::vec4>> m_boneWeights = nullptr;
		std::shared_ptr<std::vector<glm::vec4>> m_boneIds = nullptr;
		std::shared_ptr<std::vector<std::shared_ptr<Bone>>> m_bones = nullptr;
		std::shared_ptr<std::vector<std::shared_ptr<Animation>>> m_animationsRef = nullptr;

		bool m_bShouldGenerateTangents = true;
		bool m_bDirtyTangents = true;
		bool m_bRenderBackfaces = false;
		float m_lineThickness = 1.0f;
		float m_defaultPlanarScale = 0.1f;
		PrimitiveMode m_drawMode = PrimitiveMode::TRIANGLES;

		// This string is used by the animation system to identify this mesh.
		std::string m_meshAnimName;
	};
}
