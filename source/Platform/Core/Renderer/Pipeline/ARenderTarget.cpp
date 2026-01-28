#include <Platform/Core/Renderer/Pipeline/ARenderTarget.h>

Lemonade::ARenderTarget::~ARenderTarget() = default;
namespace Lemonade
{
    ARenderTarget::ARenderTarget(glm::ivec2 dimensions, uint32 layerCount)
    {
        m_layerCount = layerCount; 
        m_colourDimensions = dimensions;
        m_depthDimensions = dimensions;
    }

    ARenderTarget::ARenderTarget(glm::ivec2 colourDimensions, glm::ivec2 depthDimensions, uint32 layerCount, bool arrayTexture)
    {
        m_layerCount = layerCount; 
        m_colourDimensions = colourDimensions;
        m_depthDimensions = depthDimensions;
        m_bArrayTexture = arrayTexture;
    }

    void ARenderTarget::SetDimensions(glm::ivec2 dimensions)
    {
        if (dimensions == m_colourDimensions)
        {
            return;
        }
    
        m_colourDimensions = dimensions;
        m_dirtyBuffer = true;
    }
}
