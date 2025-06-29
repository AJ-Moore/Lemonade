#include <LCommon.h>

namespace Lemonade{
    struct alignas(16) VertexData
    {
        glm::mat4 model;         // 64 bytes, aligned to 16
        glm::mat4 view;          // 64 bytes
        glm::mat4 projection;    // 64 bytes
        glm::vec3 cameraPosition; // 12 bytes
        float padding1;           // 4 bytes padding to align next bools as int
        int shadowPass;           // 4 bytes
        int unlit;                // 4 bytes
        int emission;             // 4 bytes
        int padding2;             // 4 bytes padding to make size multiple of 16 bytes
    };
}