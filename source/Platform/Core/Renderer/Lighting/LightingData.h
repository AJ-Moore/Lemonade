#include <LCommon.h>
#include <glm/fwd.hpp>

namespace Lemonade{
    struct alignas(16) LightingData
    {
        /// Light world space matrix
        glm::mat4 WorldMatrix = glm::mat4(1);

        /// Light position in world space 
        glm::vec3 LightPosition = {1,2,3};

        // Padding to 16 byte alignment.
        float Padding0;

        /// Light Direction 
        glm::vec3 LightDirection;

        // Padding to 16 byte alignment.
        float Padding1;

        /// Light colour
        glm::vec3 Colour; 

        // Padding to 16 byte alignment.
        float Padding2;

        /// See LightType emuration point/ spot/ directional etc...
        int LightType = 0;

        /// Light Intesnity in Lumens
        float LightIntensityLumens = 800;

        /// Cone Angle
        float LightConeAngle = 45;

        /// Max distance, -1 presumed to be infinite
        float MaxDistance = 100;
    };
}