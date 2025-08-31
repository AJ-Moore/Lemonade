#pragma once 

#include "LCommon.h"
#include "Platform/Core/LObject.h"

namespace Lemonade 
{
    enum class LTextureFilter
    {
        Nearest = 0,
        Linear = 1, 
        Cubic = 2,
    };

    enum class LSamplerAddressMode
    {
        Repeat = 0,
        MirrorRepeat = 1, 
        ClampToEdge = 2,
        ClampToBorder = 3,
        MirrorClampToEdge = 4,
    };

    enum class SamplerMipmapMode {
        Nearest = 0, 
        Linear = 1,
    };

    struct LEMONADE_API LSamplerInfo 
    {
        /// normalised uvw 0-1, unnormalised 0-texture width/height/depth
        bool NormalizedCoordinates = false; 
        bool CompareEnable = false;
        LSamplerAddressMode AddressModeU = LSamplerAddressMode::Repeat;
        LSamplerAddressMode AddressModeV = LSamplerAddressMode::Repeat; 
        LSamplerAddressMode AddressModeW = LSamplerAddressMode::Repeat;
        bool AnisotrophyEnabled = true;
        LTextureFilter MagFilter = LTextureFilter::Linear;
        LTextureFilter MinFilter = LTextureFilter::Linear; 
        float MaxAnisotropy = 1.0f;
    };

    class LEMONADE_API ASampler : public LObject
    {
    protected: 
        virtual bool Init() override = 0; 
        virtual void Update() override{}
        virtual void Render() override{}
        virtual void Unload() override{}
    private:
        LSamplerInfo m_samplerInfo;
    };
}