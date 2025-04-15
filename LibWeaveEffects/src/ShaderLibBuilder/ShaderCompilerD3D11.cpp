#include "pch.hpp"
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <memory>
#include <thread>
#include "WeaveUtils/WinUtils.hpp"
#include "WeaveEffects/EffectParseException.hpp"
#include "WeaveEffects/ShaderLibBuilder/ShaderCompiler.hpp"

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace Weave;
using namespace Weave::Effects;

/// <summary>
/// Stores temporary configuration information
/// </summary>
class CompilerState
{
public:
	CompilerState() : 
		isDebugging(false)
	{ SetFeatureLevel("5_0"); }

	void SetDebugging(bool isDebugging) { this->isDebugging = isDebugging; }

	bool GetIsDebugging() const { return isDebugging; }

	/// <summary>
	/// Updates feature level and per-stage version strings
	/// </summary>
	void SetFeatureLevel(string_view fl)
	{
		if (fl == featureLevel)
			return;

		featureLevel = fl;

		for (int i = 0; i < TargetCount; i++)
		{
			string& target = targets[i];
			target.clear();
			target.append(TargetPrefixesD3D11[i]);
			target.append(featureLevel);
		}
	}

	/// <summary>
	/// Returns the current feature level string
	/// </summary>
	string_view GetFeatureLevel() const { return featureLevel; }

	/// <summary>
	/// Returns shader model target string for the given stage
	/// </summary>
	string_view GetTarget(ShadeStages stage) const { return targets[(int)stage]; }

	/// <summary>
	/// Returns the total number of compile target types
	/// </summary>
	static constexpr size_t GetTargetCount() { return TargetCount; }

private:
	// Per stage shader model prefixes
	static constexpr string_view TargetPrefixesD3D11[] =
	{
		"Unknown",
		"vs_",
		"hs_",
		"ds_",
		"gs_",
		"ps_",
		"cs_"
	};

	static constexpr size_t TargetCount = std::size(TargetPrefixesD3D11);

	bool isDebugging;

	// Feature level postfix, e.g. 5_0, 4_0, 4_0_level_9_1
	string_view featureLevel;
	string targets[TargetCount];
};

/// <summary>
/// Compiler state for the current thread
/// </summary>
static thread_local CompilerState s_State;

/// <summary>
/// Converts resource description into portable enum
/// </summary>
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
static void GetPrecompShaderD3D11(
	string_view srcFile, 
	string_view srcText, 
	ShadeStages stage, 
	string_view mainName, 
	ShaderDef& def, 
	ShaderRegistryBuilder& builder
)
{
	FXSYNTAX_CHECK_MSG((int)stage > 0 && (int)stage < CompilerState::GetTargetCount(), "Invalid stage specified");

	// Create temporary null-terminated string copies
	static thread_local string tmpPath;
	tmpPath.clear();
	tmpPath.append(srcFile);
	static thread_local string tmpMain;
	tmpMain.clear();
	tmpMain.append(mainName);

	// Disallow deprecated features
	uint compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;

	if (!s_State.GetIsDebugging())
		compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
	else
		compileFlags |= D3DCOMPILE_DEBUG;
	
	// Compile
	ComPtr<ID3DBlob> binSrc, errors;
	const HRESULT hr = D3DCompile
	(
		srcText.data(), srcText.size(),
		tmpPath.c_str(),
		NULL, NULL,
		tmpMain.c_str(), // Entrypoint name
		s_State.GetTarget(stage).data(), // Shader target model
		compileFlags, 0,
		&binSrc,
		&errors
	);

	// Validate
	if (FAILED(hr))
	{
		if (errors.Get() != nullptr)
			FX_THROW("{}", (char*)errors->GetBufferPointer());
		else
			FX_THROW("Unknown compiler error");
	}

	def.fileStringID = builder.GetOrAddStringID(srcFile);
	def.nameID = builder.GetOrAddStringID(mainName);
	def.stage = stage;

	const Span<byte> binSpan(static_cast<byte*>(binSrc->GetBufferPointer()), binSrc->GetBufferSize());
	Vector<byte> byteBuf = builder.GetTmpByteBuffer();
	byteBuf.AddRange(binSpan);

	def.byteCodeID = builder.GetOrAddShaderBin(byteBuf);
	builder.ReturnTmpByteBuffer(std::move(byteBuf));
}

/// <summary>
/// Counts the number of elements in a vector
/// </summary>
static uint GetParamSize(const D3D11_SIGNATURE_PARAMETER_DESC& paramDesc)
{
	uint componentCount = 0;

	for (int i = 0; i < 4; i++)
		componentCount += (paramDesc.Mask >> i) & 1;

	return componentCount;
}

/// <summary>
/// Generates a serializable definition for a given parameter or return value for a given stage
/// </summary>
static uint GetIOElementDef(const D3D11_SIGNATURE_PARAMETER_DESC& paramDesc, ShaderRegistryBuilder& builder)
{
	const uint componentSize = (paramDesc.ComponentType != D3D_REGISTER_COMPONENT_UNKNOWN) ? 4 : 0;
	IOElementDef element;

	element.semanticID = builder.GetOrAddStringID(string_view(paramDesc.SemanticName));
	element.semanticIndex = paramDesc.SemanticIndex;
	element.dataType = paramDesc.ComponentType;
	element.componentCount = GetParamSize(paramDesc);
	element.size = element.componentCount * 4; // Assume 4-byte/32-bit components

	return builder.GetOrAddIOElement(element);
}

/// <summary>
/// Creates serializable input and output signatures for the given shader
/// </summary>
static void GetIOLayout(ID3D11ShaderReflection* pReflect, const D3D11_SHADER_DESC& shaderDesc, 
	ShaderDef& def, ShaderRegistryBuilder& builder)
{
	// Input params
	if (shaderDesc.InputParameters > 0)
	{
		Vector<uint> idBuf = builder.GetTmpIDBuffer();

		for (uint i = 0; i < shaderDesc.InputParameters; i++)
		{
			D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
			WIN_CHECK_HR(pReflect->GetInputParameterDesc(i, &paramDesc));
			idBuf.EmplaceBack(GetIOElementDef(paramDesc, builder));
		}

		def.inLayoutID = builder.GetOrAddIDGroup(idBuf);
		builder.ReturnTmpIDBuffer(std::move(idBuf));
	}
	else
		def.inLayoutID = -1;

	// Output params
	if (shaderDesc.OutputParameters > 0)
	{
		Vector<uint> idBuf = builder.GetTmpIDBuffer();

		for (uint i = 0; i < shaderDesc.OutputParameters; i++)
		{
			D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
			WIN_CHECK_HR(pReflect->GetOutputParameterDesc(i, &paramDesc));
			idBuf.EmplaceBack(GetIOElementDef(paramDesc, builder));
		}

		def.outLayoutID = builder.GetOrAddIDGroup(idBuf);
		builder.ReturnTmpIDBuffer(std::move(idBuf));
	}
	else
		def.outLayoutID = -1;
}

/// <summary>
/// Creates serializable per-element definitions for all constant buffers used by the shader
/// </summary>
static void GetConstantBuffers(ID3D11ShaderReflection* pReflect, const D3D11_SHADER_DESC& shaderDesc, 
	ShaderDef& def, ShaderRegistryBuilder& builder)
{
	// Constant buffers
	if (shaderDesc.ConstantBuffers > 0)
	{
		Vector<uint> groupIDbuf = builder.GetTmpIDBuffer();

		for (uint i = 0; i < shaderDesc.ConstantBuffers; i++)
		{
			ID3D11ShaderReflectionConstantBuffer& cbuf = *pReflect->GetConstantBufferByIndex(i);
			D3D11_SHADER_BUFFER_DESC cbufDesc;
			WIN_CHECK_HR(cbuf.GetDesc(&cbufDesc));

			Vector<uint> constIDbuf = builder.GetTmpIDBuffer();
			ConstBufDef constants;
			constants.stringID = builder.GetOrAddStringID(string_view(cbufDesc.Name));
			constants.size = cbufDesc.Size;
			
			for (uint j = 0; j < cbufDesc.Variables; j++)
			{
				ID3D11ShaderReflectionVariable& var = *cbuf.GetVariableByIndex(j);
				D3D11_SHADER_VARIABLE_DESC varDesc;
				WIN_CHECK_HR(var.GetDesc(&varDesc));

				ConstDef varDef;
				varDef.stringID = builder.GetOrAddStringID(string_view(varDesc.Name));
				varDef.offset = varDesc.StartOffset;
				varDef.size = varDesc.Size;

				constIDbuf.EmplaceBack(builder.GetOrAddConstant(varDef));
			}

			constants.layoutID = builder.GetOrAddIDGroup(constIDbuf);
			groupIDbuf.EmplaceBack(builder.GetOrAddConstantBuffer(constants));
			builder.ReturnTmpIDBuffer(std::move(constIDbuf));
		}

		def.cbufGroupID = builder.GetOrAddIDGroup(groupIDbuf);
		builder.ReturnTmpIDBuffer(std::move(groupIDbuf));
	}
	else
		def.cbufGroupID = -1;
}

/// <summary>
/// Creates serializable definitions for resources that are NOT constant buffers, includes textures and samplers.
/// </summary>
static void GetResources(ID3D11ShaderReflection* pReflect, const D3D11_SHADER_DESC& shaderDesc, ShaderDef& def, ShaderRegistryBuilder& builder)
{
	// Constant buffers are considered resources, but handled separately
	const int resourceCount = (int)shaderDesc.BoundResources - (int)shaderDesc.ConstantBuffers;

	// Resources
	if (resourceCount > 0)
	{
		Vector<uint> idBuf = builder.GetTmpIDBuffer();
		uint resIndex = 0;

		for (uint i = 0; i < shaderDesc.BoundResources; i++)
		{
			D3D11_SHADER_INPUT_BIND_DESC resDesc;
			WIN_CHECK_HR(pReflect->GetResourceBindingDesc(i, &resDesc));

			if (resDesc.Type != D3D_SIT_CBUFFER)
			{
				ResourceDef res;
				res.stringID = builder.GetOrAddStringID(string_view(resDesc.Name));
				res.type = GetResourceType(resDesc);
				res.slot = resDesc.BindPoint;

				idBuf.EmplaceBack(builder.GetOrAddResource(res));
				resIndex++;
			}
		}

		def.resLayoutID = builder.GetOrAddIDGroup(idBuf);
		builder.ReturnTmpIDBuffer(std::move(idBuf));
	}
	else
		def.resLayoutID = -1;
}

/// <summary>
/// Gets serializable metadata from a precompiled D3D11 shader
/// </summary>
static void GetReflectedMetadata(ShaderDef& def, ShaderRegistryBuilder& builder)
{
	// Reflect
	const IDynamicArray<byte>& binSrc = builder.GetShaderBin(def.byteCodeID);

	ComPtr<ID3D11ShaderReflection> pReflect;
	WIN_CHECK_HR(D3DReflect(
		binSrc.GetData(), GetArrSize(binSrc),
		__uuidof(ID3D11ShaderReflection),
		&pReflect
	));

	D3D11_SHADER_DESC shaderDesc;
	WIN_CHECK_HR(pReflect->GetDesc(&shaderDesc));
	
	GetIOLayout(pReflect.Get(), shaderDesc, def, builder);
	GetConstantBuffers(pReflect.Get(), shaderDesc, def, builder);
	GetResources(pReflect.Get(), shaderDesc, def, builder);

	if (def.stage == ShadeStages::Compute)
		pReflect->GetThreadGroupSize(&def.threadGroupSize.x, &def.threadGroupSize.y, &def.threadGroupSize.z);
}

/// <summary>
/// Precompiles the given HLSL source and generates metadata for resources required by the shader
/// </summary>
uint Weave::Effects::GetShaderDefD3D11(
	string_view srcFile, 
	string_view srcText, 
	string_view featureLevel, 
	ShadeStages stage,
	string_view mainName, 
	ShaderRegistryBuilder& builder,
	bool isDebugging
)
{
	ShaderDef def;
	s_State.SetFeatureLevel(featureLevel);
	s_State.SetDebugging(isDebugging);

	// Precompile
	GetPrecompShaderD3D11(srcFile, srcText, stage, mainName, def, builder);
	// Reflect binary
	GetReflectedMetadata(def, builder);

	return builder.GetOrAddShader(def);
}

string_view Weave::Effects::GetCompilerVersionD3D11() { return D3DCOMPILER_DLL_A; }