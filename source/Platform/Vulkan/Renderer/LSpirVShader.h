#pragma once

#include "Events/Event.h"
#include "Platform/Core/Renderer/Materials/AShader.h"
#include <LCommon.h>
#include <memory>
#include <dxc/dxcapi.h>
#include <dxc/WinAdapter.h>

#if defined(RENDERER_VULKAN) || defined(RENDERER_OGL)

// IID_IDxcLibrary
static const GUID IID_IDxcLibrary =
{ 0xE5204DC7, 0xD18C, 0x4C3C, { 0xBD, 0xFB, 0x85, 0x42, 0x94, 0xF5, 0x1D, 0xE2 } };

// IID_IDxcCompiler
static const GUID IID_IDxcCompiler =
{ 0x8CA3E215, 0xF728, 0x4CF3, { 0x8C, 0x84, 0xE8, 0x67, 0x57, 0x62, 0x3B, 0xDB } };

// IID_IDxcOperationResult
static const GUID IID_IDxcOperationResult =
{ 0xCEDB484A, 0xD4E9, 0x445A, { 0x81, 0x89, 0xEC, 0xFC, 0xEF, 0x62, 0x44, 0x36 } };

static const GUID IID_IDxcUtils =
{ 0x157F8C11, 0xD5A0, 0x4B7D, { 0xB8, 0x0E, 0x0D, 0x04, 0xB7, 0x3A, 0xB9, 0x4A } };

// IDxcResult IID
static const GUID IID_IDxcResult = 
	{ 0x58346cda, 0xdde7, 0x4497, {0x94, 0x3a, 0x00, 0x4d, 0xb0, 0x5c, 0x3e, 0xf2}};


namespace Lemonade{
    class LEMONADE_API LSpirVShader : public AShader
    {
	public:
		CitrusCore::Event<IDxcBlob*> OnShaderCompiled;
    protected:
		virtual uint32 LoadAndCompileShader(const std::string& shaderFile, ShaderType shaderType) = 0;
		virtual bool LoadResource(std::string path);
    private:
		void InitialiseDXC();
		void CompileShader(IDxcBlobEncoding* sourceBlob, std::vector<LPCWSTR>& arguments);
		bool m_bDoneDXCInit;

		std::shared_ptr<IDxcLibrary> m_library;
		std::shared_ptr<IDxcCompiler3> m_compiler; 
		std::shared_ptr<IDxcUtils> m_utils;		
    };
}

#endif 