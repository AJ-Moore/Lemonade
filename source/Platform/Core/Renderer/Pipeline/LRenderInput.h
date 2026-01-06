
#include <Platform/Core/Renderer/Lighting/Light.h>
#include <LCommon.h>

namespace Lemonade 
{
    struct LEMONADE_API LightPtr
    {
        Light* LightPtr;
        uint32_t Count;
    };

    struct LEMONADE_API LRenderInput{
        LightPtr LightData;
    };
}