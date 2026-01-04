#pragma once
#include <Platform/Core/LObject.h>
#include <LCommon.h>

namespace Lemonade 
{
    class LEMONADE_API AUniformBuffer : public LObject
    {
    public:
        AUniformBuffer() = delete; 
        AUniformBuffer(void* dataPtr, size_t dataSize) { m_data = dataPtr; m_dataSize = dataSize;}
    protected:
		virtual bool Init() override = 0;
		virtual void Unload() override{}
		virtual void Update() override{}
		virtual void Render() override{}

        void* GetData() const { return m_data; }
        size_t GetSize() const { return m_dataSize; }
    private: 
        size_t m_dataSize = 0;
        void* m_data = nullptr;
    };
}