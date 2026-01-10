#pragma once
#include <Platform/Core/LObject.h>
#include <LCommon.h>

namespace Lemonade 
{
    enum class LEMONADE_API LBufferType
    {
        Uniform, 
        Storage
    };

    class LEMONADE_API AUniformBuffer : public LObject
    {
    public:
        AUniformBuffer() = delete; 
        AUniformBuffer(LBufferType buffertype, void* dataPtr, size_t dataSize) { m_bufferType = buffertype; m_data = dataPtr; m_dataSize = dataSize;}
    protected:
		virtual bool Init() override = 0;
		virtual void Unload() override{}
		virtual void Update() override{}
		virtual void Render() override{}

        void* GetData() const { return m_data; }
        size_t GetSize() const { return m_dataSize; }
        LBufferType GetBufferType() const { return m_bufferType; }
    private: 
        LBufferType m_bufferType;
        size_t m_dataSize = 0;
        void* m_data = nullptr;
    };
}