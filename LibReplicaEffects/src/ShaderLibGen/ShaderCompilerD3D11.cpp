#include "pch.hpp"
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <memory>
#include "ShaderLibGen/ShaderCompiler.hpp"
#include <ReplicaWinUtils.hpp>
#include "ParseExcept.hpp"

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

#define TMP_STR_BUF(tmpName, src, size, errMsg)\
REP_ASSERT_MSG(src.length() < size, errMsg)\
char tmpName[size];\
std::copy(src.begin(), src.end(), (char*)&tmpName);\
tmpName[src.length()] = '\0';

using namespace Replica;
using namespace Replica::Effects;

constexpr string_view D3D11Targets[] =
{
	"Unknown",
	"vs_" RPFX_D3D11_FEATURE_LEVEL,
	"hs_" RPFX_D3D11_FEATURE_LEVEL,
	"ds_" RPFX_D3D11_FEATURE_LEVEL,
	"gs_" RPFX_D3D11_FEATURE_LEVEL,
	"ps_" RPFX_D3D11_FEATURE_LEVEL,
	"cs_" RPFX_D3D11_FEATURE_LEVEL
};

static ShaderTypes GetResourceType(const D3D11_SHADER_INPUT_BIND_DESC& resDesc)
{
	ShaderTypes flags = ShaderTypes::Void;

	switch (resDesc.Dimension)
	{
	case D3D_SRV_DIMENSION_BUFFER:
	case D3D_SRV_DIMENSION_BUFFEREX:
		flags |= ShaderTypes::Buffer;
		break;
	case D3D_SRV_DIMENSION_TEXTURE1D: 
		flags |= ShaderTypes::Texture1D;
		break;
	case D3D_SRV_DIMENSION_TEXTURE1DARRAY:
		flags |= ShaderTypes::Texture1DArray;
		break;
	case D3D_SRV_DIMENSION_TEXTURE2D:
		flags |= ShaderTypes::Texture2D;
		break;
	case D3D_SRV_DIMENSION_TEXTURE2DARRAY:
		flags |= ShaderTypes::Texture2DArray;
		break;
	case D3D_SRV_DIMENSION_TEXTURE3D:
		flags |= ShaderTypes::Texture3D;
		break;
	case D3D_SRV_DIMENSION_TEXTURECUBE:
		flags |= ShaderTypes::TextureCube;
		break;
	case D3D_SRV_DIMENSION_TEXTURECUBEARRAY:
		flags |= ShaderTypes::TextureCubeArray;
		break;
	}

	switch (resDesc.Type)
	{
	case D3D_SIT_SAMPLER:
		flags |= ShaderTypes::Sampler;
		break;
	case D3D_SIT_UAV_RWTYPED:
		flags |= ShaderTypes::RandomWrite;
		break;
	case D3D_SIT_STRUCTURED:
		flags |= ShaderTypes::Structured;
		break;
	case D3D_SIT_UAV_RWSTRUCTURED:
		flags |= ShaderTypes::RandomWrite | ShaderTypes::Structured;
		break;
	}

	return flags;
}

/// <summary>
/// Returns precompiled shader bytecode for D3D11 with the shading stage specified, targeting SM 5.0
/// </summary>
static void GetPrecompShaderD3D11(string_view srcFile, string_view srcText, ShadeStages stage, string_view mainName, ShaderDef& def)
{
	REP_ASSERT_MSG((int)stage > 0 && (int)stage < std::size(D3D11Targets), "Invalid stage specified")

	// Create temporary null-terminated string copies
	TMP_STR_BUF(fileBuf, srcFile, 256, "File name length cannot exceed 255 characters");
	TMP_STR_BUF(epBuf, mainName, 256, "Entrypoint name length cannot exceed 255 characters");
		
	// Disallow deprecated features
	uint compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
	compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

	// Compile
	ComPtr<ID3DBlob> binSrc, errors;
	const HRESULT hr = D3DCompile
	(
		srcText.data(), srcText.size(),
		(char*)&fileBuf,
		NULL, NULL,
		(char*)&epBuf, // Entrypoint name
		D3D11Targets[(int)stage].data(), // Shader target model
		compileFlags, 0,
		&binSrc,
		&errors
	);

	// Validate
	if (FAILED(hr))
	{
		if (errors.Get() != nullptr)
			PARSE_ERR_FMT("{}", (char*)errors->GetBufferPointer())
		else
			PARSE_ERR("Unknown compiler error")
	}

	def.fileName = srcFile;
	def.name = mainName;
	def.stage = stage;
	// Copy binary to definition
	def.binSrc = DynamicArray<byte>(static_cast<const byte*>(binSrc->GetBufferPointer()), binSrc->GetBufferSize());
}

static uint GetParamSize(const D3D11_SIGNATURE_PARAMETER_DESC& paramDesc)
{
	uint componentCount = 0;

	for (int i = 0; i < 4; i++)
		componentCount += (paramDesc.Mask >> i) & 1;

	return componentCount;
}

static void GetIOElementDef(IOElementDef& layout, const D3D11_SIGNATURE_PARAMETER_DESC& paramDesc)
{
	const uint componentSize = (paramDesc.ComponentType != D3D_REGISTER_COMPONENT_UNKNOWN) ? 4 : 0;

	layout.semanticName = paramDesc.SemanticName;
	layout.semanticIndex = paramDesc.SemanticIndex;
	layout.dataType = paramDesc.ComponentType;
	layout.componentCount = GetParamSize(paramDesc);
	layout.size = layout.componentCount * 4;
}

static void GetIOLayout(ID3D11ShaderReflection* pReflect, const D3D11_SHADER_DESC& shaderDesc, ShaderDef& shaderDef)
{
	// Input params
	if (shaderDesc.InputParameters > 0)
	{
		shaderDef.inLayout = IOLayoutDef(shaderDesc.InputParameters);
		IOLayoutDef& inLayout = shaderDef.inLayout;

		for (uint i = 0; i < shaderDesc.InputParameters; i++)
		{
			D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
			WIN_THROW_HR(pReflect->GetInputParameterDesc(i, &paramDesc));
			GetIOElementDef(inLayout[i], paramDesc);
		}
	}

	// Output params
	if (shaderDesc.OutputParameters > 0)
	{
		shaderDef.outLayout = IOLayoutDef(shaderDesc.OutputParameters);
		IOLayoutDef& outLayout = shaderDef.outLayout;

		for (uint i = 0; i < shaderDesc.OutputParameters; i++)
		{
			D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
			WIN_THROW_HR(pReflect->GetOutputParameterDesc(i, &paramDesc));
			GetIOElementDef(outLayout[i], paramDesc);
		}
	}
}

static void GetConstantBuffers(ID3D11ShaderReflection* pReflect, const D3D11_SHADER_DESC& shaderDesc, ShaderDef& shaderDef)
{
	// Constant buffers
	if (shaderDesc.ConstantBuffers > 0)
	{
		shaderDef.constBufs = DynamicArray<ConstBufLayout>(shaderDesc.ConstantBuffers);
		DynamicArray<ConstBufLayout>& constBufs = shaderDef.constBufs;

		for (uint i = 0; i < shaderDesc.ConstantBuffers; i++)
		{
			ID3D11ShaderReflectionConstantBuffer& cbuf = *pReflect->GetConstantBufferByIndex(i);
			D3D11_SHADER_BUFFER_DESC cbufDesc;
			WIN_THROW_HR(cbuf.GetDesc(&cbufDesc));

			ConstBufLayout& layout = constBufs[i];
			layout.name = cbufDesc.Name;
			layout.members = DynamicArray<ConstDef>(cbufDesc.Variables);
			layout.size = cbufDesc.Size;

			for (uint j = 0; j < cbufDesc.Variables; j++)
			{
				ID3D11ShaderReflectionVariable& var = *cbuf.GetVariableByIndex(j);
				D3D11_SHADER_VARIABLE_DESC varDesc;
				WIN_THROW_HR(var.GetDesc(&varDesc));

				ConstDef& varDef = layout.members[j];
				varDef.name = varDesc.Name;
				varDef.offset = varDesc.StartOffset;
				varDef.size = varDesc.Size;
			}
		}
	}
}

static void GetResources(ID3D11ShaderReflection* pReflect, const D3D11_SHADER_DESC& shaderDesc, ShaderDef& shaderDef)
{
	// Constant buffers are considered resources, but handled separately
	const auto resourceCount = (int)shaderDesc.BoundResources - (int)shaderDesc.ConstantBuffers;

	// Resources
	if (resourceCount > 0)
	{
		shaderDef.res = DynamicArray<ResourceDef>(resourceCount);
		DynamicArray<ResourceDef>& resources = shaderDef.res;
		uint resIndex = 0;

		for (uint i = 0; i < shaderDesc.BoundResources; i++)
		{
			D3D11_SHADER_INPUT_BIND_DESC resDesc;
			WIN_THROW_HR(pReflect->GetResourceBindingDesc(i, &resDesc));

			if (resDesc.Type != D3D_SIT_CBUFFER)
			{
				resources[resIndex].name = resDesc.Name;
				resources[resIndex].type = GetResourceType(resDesc);
				resources[resIndex].slot = resDesc.BindPoint;
				resIndex++;
			}
		}
	}
}

static void GetReflectedMetadata(ShaderDef& def)
{
	// Reflect
	ComPtr<ID3D11ShaderReflection> pReflect;
	WIN_THROW_HR(D3DReflect(
		def.binSrc.GetPtr(), GetArrSize(def.binSrc),
		__uuidof(ID3D11ShaderReflection),
		&pReflect
	));

	D3D11_SHADER_DESC shaderDesc;
	WIN_THROW_HR(pReflect->GetDesc(&shaderDesc));

	GetIOLayout(pReflect.Get(), shaderDesc, def);
	GetConstantBuffers(pReflect.Get(), shaderDesc, def);
	GetResources(pReflect.Get(), shaderDesc, def);

	if (def.stage == ShadeStages::Compute)
		pReflect->GetThreadGroupSize(&def.threadGroupSize.x, &def.threadGroupSize.y, &def.threadGroupSize.z);
}

/// <summary>
/// Precompiles the given HLSL source and generates metadata for resources required by the shader
/// </summary>
void Replica::Effects::GetShaderDefD3D11(string_view srcFile, string_view srcText, ShadeStages stage, string_view mainName, ShaderDef& def)
{
	// Precompile
	GetPrecompShaderD3D11(srcFile, srcText, stage, mainName, def);
	// Reflect binary
	GetReflectedMetadata(def);
}