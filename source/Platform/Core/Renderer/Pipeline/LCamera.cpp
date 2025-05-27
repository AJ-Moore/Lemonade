#include <Platform/Core/Renderer/Pipeline/LCamera.h>

namespace Lemonade 
{
    void LCamera::CalculateProjMatrix(float width, float height)
    {
        float ratio = 0;
        if (height)
        {
            ratio = width / height;
        }

        if (m_orthographicMode)
        {	
            SetOrthographicDimensions(glm::vec4(0, 0, width, height));
            m_projMat = m_orthographic;
            //float sizeX = ratio * this->m_orthographicSize;
            //float sizeY = this->m_orthographicSize;
            //this->m_projMat = glm::ortho(-sizeX, sizeX, -sizeY, sizeY, m_nearClip, m_farClip);
        }
        else
        {
            m_projMat = glm::perspectiveLH(m_fov/(180/glm::pi<float>()), ratio, m_nearClip, m_farClip);
        }
    }

    void LCamera::SetOrthographicDimensions(glm::vec4 rect)
    {
        m_orthographic = glm::ortho(rect.x, rect.z, rect.y, rect.w);
    }

    void LCamera::CalculateViewMatrix()
    {
        glm::vec3 position = m_transform->GetWorldPosition();
        glm::vec3 up = m_transform->GetUp();
        glm::vec3 forward = m_transform->GetForward();

        m_viewMat = glm::lookAtLH(position, position + forward, up);
    }

    void LCamera::CalculateViewProjMatrix(){

        if (m_orthographicMode)
        {
            m_viewProjMatrix = m_orthographic;
            return;
        }

        m_viewProjMatrix = this->m_projMat * this->m_viewMat;
    }
}