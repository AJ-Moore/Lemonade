#include <LCommon.h>
#include <glm/fwd.hpp>

namespace Lemonade{
    struct alignas(16) DeferredData
    {
        int LightCount = 0;
        float Padding0[3];
    };
}