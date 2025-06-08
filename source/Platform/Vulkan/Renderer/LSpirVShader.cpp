#include "Platform/Core/Services/GraphicsServices.h"
#include <dxc/dxcapi.h>
#include <Platform/Vulkan/Renderer/LSpirVShader.h>
#include <memory>
#include <string>

namespace Lemonade 
{
    uint32 LSpirVShader::LoadAndCompileShader(const std::string& shaderFile, ShaderType shaderType)
    {
        return 0;
    }

    bool LSpirVShader::LoadResource(std::string path)
	{
		InitialiseDXC();
        std::wstring wsPath(path.begin(), path.end());

		// Attempt to load resource from disk 
		// Load the HLSL text shader from disk
		uint32_t codePage = DXC_CP_ACP;
		CComPtr<IDxcBlobEncoding> sourceBlob;
        HRESULT hres;
		hres = m_utils->LoadFile(wsPath.c_str(), &codePage, &sourceBlob);
		if (FAILED(hres)) {
			throw std::runtime_error("Could not load shader file");
			return false;
		}

		// Select target profile based on shader file extension
		LPCWSTR targetProfile{};
		size_t idx = path.rfind('.');
		if (idx != std::string::npos) {
			std::string extension = path.substr(idx + 1);
			if (extension == "vert") {
				targetProfile = L"vs_6_1";
			}
			if (extension == "frag") {
				targetProfile = L"ps_6_1";
			}
			// Mapping for other file types go here (cs_x_y, lib_x_y, etc.)
		}

		// Configure the compiler arguments for compiling the HLSL shader to SPIR-V
		std::vector<LPCWSTR> arguments = {
			// (Optional) name of the shader file to be displayed e.g. in an error message
			wsPath.c_str(),
			// Shader main entry point
			L"-E", L"main",
			// Shader target profile
			L"-T", targetProfile,
			// Compile to SPIRV
			L"-spirv"
		};

        CompileShader(sourceBlob, arguments);
        //?release maybe?
        sourceBlob.Release();

		return true;
	}

    void LSpirVShader::CompileShader(IDxcBlobEncoding* sourceBlob, std::vector<LPCWSTR>& arguments)
	{
		// Compile shader
		DxcBuffer buffer{};
		buffer.Encoding = DXC_CP_ACP;
		buffer.Ptr = sourceBlob->GetBufferPointer();
		buffer.Size = sourceBlob->GetBufferSize();

		HRESULT hres;
		IDxcResult* result{ nullptr };
        //m_compiler->Compile
		hres = m_compiler->Compile(
			&buffer,
			arguments.data(),
			(uint32_t)arguments.size(),
			nullptr,
			IID_IDxcResult,
			(void**)(&result));

		if (SUCCEEDED(hres)) {
			result->GetStatus(&hres);
		}

		// Output error if compilation failed
		if (FAILED(hres) && (result)) {
			CComPtr<IDxcBlobEncoding> errorBlob;
			hres = result->GetErrorBuffer(&errorBlob);
			if (SUCCEEDED(hres) && errorBlob) {
				//std::cerr << "Shader compilation failed :\n\n" << (const char*)errorBlob->GetBufferPointer();
				throw std::runtime_error("Compilation failed");
			}
		}

		// Get compilation result
		CComPtr<IDxcBlob> code;
		result->GetResult(&code);
        OnShaderCompiled.Invoke(code);

		/// VULKAN SPECIFIC BIT, REST CAN BE MADE COMMON! 
		// Create a Vulkan shader module from the compilation result
		VkShaderModuleCreateInfo shaderModuleCI{};
		shaderModuleCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCI.codeSize = code->GetBufferSize();
		shaderModuleCI.pCode = (uint32_t*)code->GetBufferPointer();
		VkShaderModule shaderModule;
		vkCreateShaderModule(GraphicsServices::GetContext()->GetVulkanDevice().GetVkDevice(), &shaderModuleCI, nullptr, &shaderModule);

        //> ??
        code.Release();

        // Release?
        result->Release();
	}

	void LSpirVShader::InitialiseDXC()
	{
		if (m_bDoneDXCInit)
		{
			return;
		}

		HRESULT hres;
		IDxcLibrary* library = nullptr;
		hres = DxcCreateInstance(CLSID_DxcLibrary, IID_IDxcLibrary, (void**)&library);
		if (FAILED(hres)) {
			throw std::runtime_error("Could not init DXC Library");
		}

        m_library = std::shared_ptr<IDxcLibrary>(library,[](IDxcLibrary* p){
            if (p) p->Release();
        });

		// Initialize DXC compiler
		IDxcCompiler3* compiler;
		hres = DxcCreateInstance(CLSID_DxcCompiler, IID_IDxcCompiler, (void**)compiler);
		if (FAILED(hres)) {
			throw std::runtime_error("Could not init DXC Compiler");
		}

        m_compiler = std::shared_ptr<IDxcCompiler3>(compiler,[](IDxcCompiler3* p){
            if (p) p->Release();
        });

		// Initialize DXC utility
		IDxcUtils* utils;
		hres = DxcCreateInstance(CLSID_DxcUtils, IID_IDxcUtils, (void**)utils);
		if (FAILED(hres)) {
			throw std::runtime_error("Could not init DXC Utiliy");
		}

        m_utils = std::shared_ptr<IDxcUtils>(utils,[](IDxcUtils* p){
            if (p) p->Release();
        });

		m_bDoneDXCInit = true;
	}
}