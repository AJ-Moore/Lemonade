#pragma once
#include <Platform/Core/LObject.h>
#include <LCommon.h>

namespace Lemonade 
{
    class LEMONADE_API ABinding : public LObject
    {
    public:
        ABinding() = delete; 
        ABinding(uint32 bindLocation) { m_bindLocation = bindLocation;}

        uint32 GetBindLocation() const { return m_bindLocation; }
    protected:
		virtual bool Init() override = 0;
		virtual void Unload() override{}
		virtual void Update() override{}
		virtual void Render() override{}
    private: 
        uint32 m_bindLocation = 0;
    };
}