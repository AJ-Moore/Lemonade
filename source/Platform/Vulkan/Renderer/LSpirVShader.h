#pragma once

#ifdef NOPE

#include <Platform/Core/Renderer/Materials/AShader.h>
#include <LCommon.h>
#include <memory>

#if defined(RENDERER_VULKAN) || defined(RENDERER_OGL)

// IID_IDxcLibrary
static const GUID IID_IDxcLibrary = 
{0xE5204DC7, 0xD18C, 0x4C3C, {0xBD, 0xFB, 0x85, 0x16, 0x73, 0x98, 0x0F, 0xE7}};

// IID_IDxcCompiler
static const GUID IID_IDxcCompiler =
{0x8c210bf3, 0x011f, 0x4422, { 0x8d, 0x70, 0x6f, 0x9a, 0xcb, 0x8d, 0xb6, 0x17 } };


// IID_IDxcOperationResult "CEDB484A-D4E9-445A-B991-CA21CA157DC2"
static const GUID IID_IDxcOperationResult =
{ 0xCEDB484A, 0xD4E9, 0x445A, { 0xb9, 0x91, 0xCA, 0x21, 0xCA, 0x15, 0x7D, 0xC2 } };

//4605c4cb_2019_492a_ada4_65f20bb7d67f
static const GUID IID_IDxcUtils =
{ 0x4605c4cb, 0x2019, 0x492a, { 0xad, 0xa4, 0x65, 0xf2, 0x0b, 0xb7, 0xd6, 0x7f } };

// IDxcResult IID 58346cda_dde7_4497_9461_6f87af5e0659
static const GUID IID_IDxcResult = 
	{ 0x58346cda, 0xdde7, 0x4497, {0x94, 0x61, 0x6F, 0x87, 0xAF, 0x5E, 0x06, 0x59}};


namespace Lemonade{
    class LEMONADE_API LSpirVShader : public AShader
    {
	public:
		virtual ~LSpirVShader();
		virtual void Bind()override{}
    protected:
		virtual void OnShaderCompiled(CComPtr<IDxcBlob> code){}
		virtual bool LoadResource(std::string path) override;
		virtual void UnloadResource() override{}
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

#endif