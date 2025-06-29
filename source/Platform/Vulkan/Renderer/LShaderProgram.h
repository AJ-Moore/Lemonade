#include <LCommon.h>
#include <Platform/Core/Renderer/Materials/AShaderProgram.h>
#include <vector>

namespace Lemonade {
    class LEMONADE_API LShaderProgram : public AShaderProgram
    {
    public:  
        friend class LShader;
        virtual void Bind() override{}

        std::vector<VkPipelineShaderStageCreateInfo>& GetShaderCreateInfo() { return m_shaderCreateInfos; }
    protected:
        virtual bool Init() override{ return true; }
    private:
        void UpdateVkPipelineShaders();
        std::vector<VkPipelineShaderStageCreateInfo> m_shaderCreateInfos;
    };
}