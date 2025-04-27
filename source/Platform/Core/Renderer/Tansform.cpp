#include "Platform/Core/Services/Services.h"
#include <Platform/Core/Renderer/Transform.h>
#include <memory>
//#include "../Core/UServiceLocator.h"
//#include "../Entity/UEntity.h"

namespace Lemonade
{
	Transform::Transform()
	{
		m_bDirty = true;
		m_scale = glm::vec3(1, 1, 1);
	}

	Transform::Transform(glm::mat4 mat) {
		m_bDirty = true;
		m_scale = glm::vec3(1, 1, 1);
		m_localTransform = mat;
	}

	Transform::~Transform()
	{
	}

	bool Transform::Init()
	{
		return true;
	}

	void Transform::SetPosition(glm::vec3 Position)
	{	
		if (Position != m_position)
		{
			m_position = Position;
			m_bDirty = true;
		}
	}

	void Transform::SetPosition(float x, float y, float z)
	{
		SetPosition(glm::vec3(x, y, z));
	}

	void Transform::SetPosition(float x, float y)
	{
		SetPosition(glm::vec3(x, y, m_position.z));
	}

	void Transform::SetRotationFromEuler(glm::vec3 Rotation)
	{
		glm::quat quat = glm::quat(Rotation);
		if (quat != m_rotation) {
			m_rotation = quat;
			m_bDirty = true;
			UpdateLocalTransform();
		}
	}

	void Transform::SetRotationFromEuler(double x, double y, double z)
	{
		SetRotationFromEuler(glm::vec3(x, y, z));
	}

	void Transform::SetRotation(glm::quat quat)
	{
		if (quat != m_rotation)
		{
			m_rotation = quat;
			m_bDirty = true;
			UpdateLocalTransform();
		}
	}

	void Transform::SetScale(glm::vec3 Scale)
	{
		if (m_scale != Scale) {
			m_scale = Scale;
			m_bDirty = true;
		}
	}

	void Transform::Move(glm::vec3 MoveVector)
	{
		SetPosition(GetPosition() + MoveVector);
	}

	void Transform::Rotate(glm::vec3 eulerAngles)
	{
		glm::quat quat = glm::quat(eulerAngles);
		SetRotation(m_rotation * quat);
	}

	glm::vec3 Transform::GetPosition()
	{
		return m_position; 
	}

	glm::vec2 Transform::GetPosition2d()
	{
		return glm::vec2(m_position);
	}

	glm::vec3 Transform::GetLocalPosition()
	{
		return glm::vec3(m_localTransform[3]);
	}

	glm::vec3 Transform::GetWorldPosition()
	{
		UpdateGlobalTransform();
		return m_worldPosition;
	}

	glm::quat Transform::GetWorldRotation()
	{
		const glm::mat4 worldMatrix = GetWorldMatrix();
		return glm::quat_cast(worldMatrix);
	}

	glm::vec3 Transform::GetForward()
	{
		const glm::mat4 world = GetWorldMatrix(); 
		return glm::normalize(glm::vec3(world[2]));
	}

	glm::vec3 Transform::GetRight()
	{
		return glm::cross(GetForward(), GetUp());
	}

	glm::vec3 Transform::GetUp()
	{
		const glm::mat4 world = GetWorldMatrix();
		return glm::normalize(glm::vec3(world[1]));
	}

	glm::vec3 Transform::GetEulerRotation()
	{
		return glm::eulerAngles(m_rotation);
	}

	glm::quat Transform::GetRotation()
	{
		return m_rotation;
	}

	const glm::mat4& Transform::GetWorldMatrix()
	{
		UpdateGlobalTransform();
		return m_globalTransform;
	}

	void Transform::SetLocal(glm::mat4 mat)
	{
		m_localTransform = mat;
		m_position = GetLocalPosition();
		m_rotation = glm::quat_cast(m_localTransform);
	}

	glm::mat4 Transform::GetLocal()
	{
		UpdateGlobalTransform();
		return m_localTransform;
	}

	void Transform::MakeParent(std::shared_ptr<Transform> transform, std::shared_ptr<Transform> parent)
	{
		parent->m_children.push_back(transform);
		parent->m_bDirty = true;
		transform->m_parent = parent.get();
	}

	std::shared_ptr<Transform> Transform::RemoveChild(Transform* child)
	{
		int index = 0;
		bool bFound = false; 
		std::shared_ptr<Transform> transformToReturn;

		for (std::shared_ptr<Transform> transform : m_children)
		{
			if (transform->GetUID() == child->GetUID())
			{
				transformToReturn = transform;
				bFound = true; 
				break; 
			}
			index++;
		}

		if (!bFound)
		{
			return nullptr; 
		}

		m_children.erase(m_children.begin() + index);

		return transformToReturn;
	}

	Transform* Transform::GetParentTransform() const
	{
		return m_parent;
	}

	//UEntity* Transform::getParentEntity()
	//{
	//	return m_entity;
	//}

	void Transform::UpdateLocalTransform()
	{
		m_localTransform = glm::translate(glm::mat4(1.0f), m_position);
		m_localTransform = m_localTransform * glm::mat4_cast(m_rotation);
		m_localTransform = glm::scale(m_localTransform, m_scale);
	}

	void Transform::UpdateGlobalTransform()
	{
		double time = Services::GetTime()->GetTimeElapsed(); 

		// Check datestamp to see if it was updated this frame.
		if ((m_datestamp != time) || m_bDirty)
		{
			m_datestamp = time;
			UpdateLocalTransform();
			if (m_parent != nullptr) {
				m_parent->UpdateGlobalTransform();
			}
			else {
				m_globalTransform = m_localTransform;
				m_worldPosition = glm::vec3(m_globalTransform[3]);
				m_bDirty = false; 
				//if (m_entity != nullptr) {
				//	m_entity->onTransformChanged();
				//}

				return;
			}

			// Calc matrix 
			m_globalTransform = m_parent->m_globalTransform * m_localTransform;
			m_worldPosition = glm::vec3(m_globalTransform[3]);

			// Called when the entitys transform is updated
			//if (m_entity != nullptr) {
			//	m_entity->onTransformChanged();
			//}

			m_bDirty = false;
		}

	}
}