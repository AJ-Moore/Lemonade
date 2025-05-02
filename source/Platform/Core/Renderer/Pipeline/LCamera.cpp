#include <Platform/Core/Renderer/Pipeline/LCamera.h>

namespace Lemonade 
{
    void LCamera::calculateProjMatrix(float width, float height)
    {
        float ratio = 0;
        if (height)
        {
            ratio = width / height;
        }

        if (m_orthographicMode)
        {	
            //setOrthographicDimensions(glm::vec4(0, 0, width, height));
            m_projMat = m_orthographic;
            //float sizeX = ratio * this->m_orthographicSize;
            //float sizeY = this->m_orthographicSize;
            //this->m_projMat = glm::ortho(-sizeX, sizeX, -sizeY, sizeY, m_nearClip, m_farClip);
        }
        else
        {
            //m_projMat = glm::perspectiveLH(m_fov/(180/glm::pi<float>()), ratio, m_nearClip, m_farClip);
        }
    }

    void LCamera::calculateViewMatrix()
    {
        //glm::vec3 position = getParent()->getTransform()->getWorldPosition();
        //glm::vec3 up = getParent()->getTransform()->getUp();
        //glm::vec3 forward = getParent()->getTransform()->getForward();
//
        //m_viewMat = glm::lookAtLH(position, position + forward, up);
    }

    void LCamera::calculateViewProjMatrix(){

        if (m_orthographicMode)
        {
            m_viewProjMatrix = m_orthographic;
            return;
        }

        m_viewProjMatrix = this->m_projMat * this->m_viewMat;
    }
}