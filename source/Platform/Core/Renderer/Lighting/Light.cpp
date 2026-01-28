#include <Platform/Core/Renderer/Lighting/Light.h>

namespace Lemonade{
    void Light::Dump()
    {
        m_lightingData.LightType = (uint32)m_lightType;
    }
}