#include <Renderer/Mesh.h>

namespace CraftyBlocks
{
	void Mesh::CalculateNormalsPerFace()
	{
		if (m_vertices == nullptr)
		{
			Log(Logger::ERROR, "Unable to calculate face normals, no vertices found in mesh.");
			return;
		}

		if (m_vertices->size() % 3 != 0)
		{
			// Invalid.
			Log(Logger::ERROR, "Invalid number of vertices.");
			return;
		}

		if (m_faceNormals == nullptr)
		{
			m_faceNormals = std::make_shared<std::vector<glm::vec3>>();
		}

		m_faceNormals->clear();
		m_faceNormals->resize(m_vertices->size() / 3);

		for (int i = 0; i < m_vertices->size(); i += 3)
		{
			glm::vec3 U = m_vertices->at(i + 1) - m_vertices->at(i);
			glm::vec3 V = m_vertices->at(i + 2) - m_vertices->at(i);
			(*m_faceNormals)[i / 3] = glm::normalize(glm::cross(U, V));
		}
	}

	void Mesh::CalculateNormalsPerVertex()
	{
		CalculateNormalsPerFace();

		if (m_faceNormals == nullptr || m_vertices == nullptr)
		{
			Log(Logger::ERROR, "Error calculating normals.");
			return;
		}

		if (m_faceNormals->size() != m_vertices->size() / 3)
		{
			Log(Logger::ERROR, "Error calculating normals, mismatch.");
			return;
		}

		if (m_normals == nullptr)
		{
			m_normals = std::make_shared<std::vector<glm::vec3>>();
		}

		m_normals->clear();
		m_normals->resize(m_vertices->size());

		for (int i = 0; i < m_vertices->size(); i += 3)
		{
			// FLAT SHADING.
			glm::vec3 flatNormal = m_faceNormals->at(i / 3);
			(*m_normals)[i] = flatNormal;
			(*m_normals)[i + 1] = flatNormal;
			(*m_normals)[i + 2] = flatNormal;

			// SMOOTH SHADING REQUIRES INDICES to get adjacent faces TODO!!! => TODO generate indices
		}

	}

	void Mesh::Clear()
	{
		m_vertices.reset();
		m_uvs.reset();
		m_colours.reset();
		m_normals.reset();
		m_tangents.reset();
		m_bitangents.reset();
		m_uvs3d.reset();
		m_bShouldGenerateTangents = true;
	}

	const std::shared_ptr<std::vector<glm::vec3>> Mesh::GetTangents()
	{
		if (m_bDirtyTangents && m_bShouldGenerateTangents)
		{
			GenerateTangents();
		}

		return m_tangents;
	}

	const std::shared_ptr<std::vector<glm::vec3>> Mesh::GetBiTangents()
	{
		if (m_bDirtyTangents && m_bShouldGenerateTangents)
		{
			GenerateTangents();
		}

		return m_bitangents;
	}

	void Mesh::GenerateTangents()
	{
		if (!m_bShouldGenerateTangents)
		{
			// Consider adding a force generate tangents option.
			return;
		}

		if (m_drawMode < PrimitiveMode::TRIANGLES)
		{
			// Not supported
			Log(Logger::ERROR, "Unsupported type generating tangenets.");
			return;
		}

		m_tangents = std::make_shared<std::vector<glm::vec3>>();
		m_bitangents = std::make_shared<std::vector<glm::vec3>>();

		for (int i = 0; i < m_vertices->size(); i += 3)
		{
			glm::vec3 vert1 = m_vertices->at(i + 1) - m_vertices->at(i + 0);
			glm::vec3 vert2 = m_vertices->at(i + 2) - m_vertices->at(i + 0);

			if (m_uvs == nullptr)
			{
				m_uvs = std::make_shared<std::vector<glm::vec2>>();

				Log(Logger::WARN, "Planar fallback no uvs found.");
				for (int p = 0; p < m_vertices->size(); p++)
				{
					m_uvs->push_back(glm::vec2(m_vertices->at(p).x, m_vertices->at(p).z) * m_defaultPlanarScale);
				}
			}

			glm::vec2 uv1 = m_uvs->at(i + 1) - m_uvs->at(i + 0);
			glm::vec2 uv2 = m_uvs->at(i + 2) - m_uvs->at(i + 0);

			float r = 1.0f / (uv1.x * uv2.y - uv1.y * uv2.x);
			glm::vec3 tangent = (vert1 * uv2.y - vert2 * uv1.y) * r;
			glm::vec3 bitangent = (vert2 * uv1.x - vert1 * uv2.x) * r;

			m_tangents->push_back(tangent);
			m_tangents->push_back(tangent);
			m_tangents->push_back(tangent);

			m_bitangents->push_back(bitangent);
			m_bitangents->push_back(bitangent);
			m_bitangents->push_back(bitangent);
		}

		m_bDirtyTangents = false;
	}
}
