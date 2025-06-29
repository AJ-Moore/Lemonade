#ifdef NOPE

#include <Platform/Vulkan/Renderer/LSpirVShader.h>
#include <dxc/WinAdapter.h>
#include <string>

namespace Lemonade 
{
	// Load bearing
	LSpirVShader::~LSpirVShader() = default;  

    bool LSpirVShader::LoadResource(std::string path)
	{
		SetShaderType(GetShaderTypeForString(path));
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
			std::string extension = path.substr(idx - 2);
			if (extension == "vs.hlsl") {
				targetProfile = L"vs_6_0";
			}
			if (extension == "fs.hlsl") {
				targetProfile = L"ps_6_0";
			}
			// Mapping for other file types go here (cs_x_y, lib_x_y, etc.)
		}

		// Configure the compiler arguments for compiling the HLSL shader to SPIR-V
		std::vector<LPCWSTR> arguments = {
			// (Optional) name of the shader file to be displayed e.g. in an error message
			//wsPath.c_str(),
			// Shader main entry point
			L"-E", L"main",
			// Shader target profile
			L"-T", L"ps_6_0",
			//L"-Qstrip_debug", 
			// Compile to SPIRV
			//L"-spirv",
			//L"-fspv-target-env=vulkan1.2",
		};

        CompileShader(sourceBlob, arguments);
        //?release maybe?
        sourceBlob.Release();

		return true;
	}

    void LSpirVShader::CompileShader(IDxcBlobEncoding* sourceBlob, std::vector<LPCWSTR>& arguments)
	{
		// Compile shader
		//DxcBuffer buffer{};
		//buffer.Encoding = DXC_CP_UTF8;
		//buffer.Ptr = sourceBlob->GetBufferPointer();
		//buffer.Size = sourceBlob->GetBufferSize();

		const char* shaderSource = R"(
			float4 main(float4 pos : SV_Position) : SV_Target {
				return float4(1.0, 0.0, 0.0, 1.0);
			}
		)";

		// Create a blob for the source with UTF-8 encoding
		CComPtr<IDxcBlobEncoding> sourceBloby;
		HRESULT hr = m_library->CreateBlobWithEncodingFromPinned(
			shaderSource,
			(UINT32)strlen(shaderSource),
			DXC_CP_UTF8,
			&sourceBloby);

		DxcBuffer buffer = {};
		buffer.Ptr = sourceBloby->GetBufferPointer();
		buffer.Size = sourceBloby->GetBufferSize();
		buffer.Encoding = DXC_CP_UTF8;
		
		//DxcBuffer buffer = {};
		//buffer.Ptr = shaderSource;
		//buffer.Size = strlen(shaderSource);
		//buffer.Encoding = DXC_CP_UTF8; // Assume UTF-8 string

		HRESULT hres;
		IDxcResult* result{ nullptr };

		if (sourceBlob) {
			const char* text = reinterpret_cast<const char*>(sourceBlob->GetBufferPointer());
			size_t size = sourceBlob->GetBufferSize();
		
			// Add null terminator (optional, for safety)
			std::string str(text, size);
			printf("%s\n", str.c_str());
		}

        //m_compiler->Compile
		hres = m_compiler->Compile(
			&buffer,
			arguments.data(),
			(uint32_t)arguments.size(),
			nullptr,
			IID_IDxcResult,
			(void**)(&result));

		if (FAILED(hres)) {
			printf("Compile call failed: 0x%08X\n", hres);
		}

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
        OnShaderCompiled(code);

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
		hres = DxcCreateInstance(CLSID_DxcCompiler, IID_IDxcCompiler, (void**)&compiler);
		if (FAILED(hres)) {
			throw std::runtime_error("Could not init DXC Compiler");
		}

        m_compiler = std::shared_ptr<IDxcCompiler3>(compiler,[](IDxcCompiler3* p){
            if (p) p->Release();
        });

		// Initialize DXC utility
		IDxcUtils* utils;
		hres = DxcCreateInstance(CLSID_DxcUtils, IID_IDxcUtils, (void**)&utils);
		if (FAILED(hres)) {
			throw std::runtime_error("Could not init DXC Utiliy");
		}

        m_utils = std::shared_ptr<IDxcUtils>(utils,[](IDxcUtils* p){
            if (p) p->Release();
        });

		m_bDoneDXCInit = true;
	}
}
#endif