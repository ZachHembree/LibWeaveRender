#pragma once
#include "pch.hpp"
#include "ReplicaEffects/ShaderLibGen/ShaderParser/BlockAnalyzer.hpp"
#include "ReplicaEffects/ShaderLibGen/SymbolTable.hpp"
#include "ReplicaEffects/ShaderLibGen/ShaderGenerator.hpp"
#include "ReplicaEffects/ShaderLibGen/ShaderCompiler.hpp"
#include "ReplicaEffects/ShaderLibGen/VariantPreprocessor.hpp"
#include "ReplicaEffects/ShaderLibGen/ShaderRegistryBuilder.hpp"
#include "ReplicaEffects/ShaderLibGen.hpp"

using namespace Replica::Effects;

ShaderLibGen::ShaderLibGen() :
	pVariantGen(new VariantPreprocessor()),
	pAnalyzer(new BlockAnalyzer()),
	pTable(new SymbolTable()),
	pShaderGen(new ShaderGenerator()),
	pShaderRegistry(new ShaderRegistryBuilder()),
	featureLevel("5_0"),
	target(PlatformTargets::DirectX11)
{ }

ShaderLibGen::~ShaderLibGen() = default;

ShaderLibDef ShaderLibGen::GetLibrary(string_view libPath, string_view libSrc)
{
	Clear();

	ShaderLibDef lib;
	pVariantGen->SetSrc(libPath, libSrc);

	for (int vID = 0; vID < pVariantGen->GetVariantCount(); vID++)
	{
		// Preprocess and parse
		pVariantGen->GetVariant(vID, libTexBuf, entrypoints);
		const bool isDuplicate = libTexBuf == libTexLast;

		if (!isDuplicate)
		{
			pAnalyzer->AnalyzeSource(libTexBuf);
			pTable->ParseBlocks(pAnalyzer->GetBlocks());

			if (vID == 0)
				InitLibrary(lib);

			LOG_INFO() << "Generating variant: " << vID;
			GetEntryPoints();
			GetEffects();

			lib.variants[vID].shaders = DynamicArray<ShaderVariantDef>(entrypoints.GetLength());
			lib.variants[vID].effects = DynamicArray<EffectVariantDef>(effectBlocks.GetLength());

			GetShaderDefs(libPath, lib.variants[vID].shaders, vID);
			GetEffectDefs(lib.variants[vID].effects, vID);
		}
		else
		{
			LOG_WARN() << "Unused flag/mode combination detected. ID: " << vID << ". Skipping...";

			// Copy variant mappings and update ID
			lib.variants[vID] = lib.variants[vID - 1];

			for (ShaderVariantDef& shader : lib.variants[vID].shaders)
				shader.variantID = vID;

			for (EffectVariantDef& effect : lib.variants[vID].effects)
				effect.variantID = vID;
		}

		ClearVariant();
	}

	lib.regData = pShaderRegistry->ExportDefinition();
	return lib;
}

void ShaderLibGen::SetTarget(PlatformTargets target) { this->target = target; }

void ShaderLibGen::SetFeatureLevel(string_view featureLevel) { this->featureLevel = featureLevel; }

void ShaderLibGen::Clear()
{
	ClearVariant();
	libTexLast.clear();
	pVariantGen->Clear();
	pShaderRegistry->Clear();
}

void ShaderLibGen::InitLibrary(ShaderLibDef& lib)
{
	// Temporary
	lib.platform = 
	{
		.compilerVersion = string(GetCompilerVersionD3D11()),
		.featureLevel = string(featureLevel),
		.target = target
	};

	const IDynamicArray<string_view>& flags = pVariantGen->GetVariantFlags();
	lib.flagIDs = DynamicArray<uint>(flags.GetLength());

	for (int i = 0; i < flags.GetLength(); i++)
		lib.flagIDs[i] = pShaderRegistry->GetOrAddStringID(flags[i]);

	const IDynamicArray<string_view>& modes = pVariantGen->GetVariantModes();
	lib.modeIDs = DynamicArray<uint>(modes.GetLength());

	for (int i = 0; i < modes.GetLength(); i++)
		lib.modeIDs[i] = pShaderRegistry->GetOrAddStringID(modes[i]);

	lib.variants = DynamicArray<VariantDef>(pVariantGen->GetVariantCount());

	LOG_INFO() << "Compiler: " << lib.platform.compilerVersion;
	LOG_INFO() << "Shader Model: " << lib.platform.featureLevel;
	LOG_INFO() << "Variants declared: " << lib.variants.GetLength();

}

void ShaderLibGen::GetEntryPoints()
{
	// Attribute tags
	for (int i = 0; i < pTable->GetSymbolCount(); i++)
	{
		SymbolHandle symbol = pTable->GetSymbol(i);

		if (symbol.GetHasFlags(SymbolTypes::FuncDefinition))
		{
			TokenNodeHandle funcIdent = symbol.GetIdent();

			for (int j = 0; j < funcIdent.GetChildCount(); j++)
			{
				if (funcIdent[j].GetHasFlags(TokenTypes::AttribShaderDecl))
				{
					string_view name = funcIdent.GetValue();

					if (!epSet.contains(name))
					{
						ShaderEntrypoint& ep = entrypoints.emplace_back();
						epSet.emplace(name);
						ep.name = name;
						ep.stage = GetStageFromFlags(funcIdent[j].GetFlags());
						ep.symbolID = i;
					}

					break;
				}
			}
		}
	}

	// Pragma shaders
	for (int i = 0; i < entrypoints.GetLength(); i++)
	{
		ShaderEntrypoint& ep = entrypoints[i];
		ScopeHandle global = pTable->GetScope(0);
		const IDList* pFuncs = global.TryGetFuncOverloads(ep.name);

		if (pFuncs != nullptr && !pFuncs->empty())
		{
			if (!epSet.contains(ep.name))
			{
				epSet.emplace(ep.name);
				ep.symbolID = pFuncs->front();
			}
		}
		else
			PARSE_ERR("Shader declared in pragma but not defined")
	}
	
	// Shader blocks
	for (int i = 0; i < pTable->GetSymbolCount(); i++)
	{
		SymbolHandle symbol = pTable->GetSymbol(i);

		if (symbol.GetHasFlags(SymbolTypes::ShaderDef))
		{
			ScopeHandle scope = *symbol.GetScope();
			string_view name = symbol.GetName();
			const IDList* pFuncs = scope.TryGetFuncOverloads(name);

			if (pFuncs != nullptr && !pFuncs->empty())
			{
				if (!epSet.contains(name))
				{
					ShaderEntrypoint& ep = entrypoints.emplace_back();
					epSet.emplace(name);
					ep.name = name;
					ep.stage = GetStageFromFlags(symbol.GetFlags());
					ep.symbolID = pFuncs->front();
				}
			}
			else
				PARSE_ERR("Shader block declared without an entrypoint definition")
		}
	}
}

void ShaderLibGen::GetEffects()
{
	for (int i = 0; i < pTable->GetSymbolCount(); i++)
	{
		SymbolHandle symbol = pTable->GetSymbol(i);

		if (symbol.GetHasFlags(SymbolTypes::TechniqueDef))
		{
			string_view name = symbol.GetName();
			effectBlocks.emplace_back(EffectBlock
			{
				.name = name
			});
		}
	}
}

void ShaderLibGen::GetShaderDefs(string_view libPath, DynamicArray<ShaderVariantDef>& shaders, const int vID)
{
	for (int i = 0; i < entrypoints.GetLength(); i++)
	{
		shaderBuf.clear();

		const ShaderEntrypoint& ep = entrypoints[i];
		pShaderGen->GetShaderSource(*pTable, pAnalyzer->GetBlocks(), ep, entrypoints, shaderBuf);

		shaders[i].shaderID = GetShaderDefD3D11(libPath, shaderBuf, featureLevel, ep.stage, ep.name, *pShaderRegistry);
		shaders[i].variantID = vID;
	}
}

void ShaderLibGen::GetEffectDefs(DynamicArray<EffectVariantDef>& effects, const int vID)
{
	for (int i = 0; i < effectBlocks.GetLength(); i++)
	{
		const uint nameID = pShaderRegistry->GetOrAddStringID(effectBlocks[i].name);
		const uint effectID = pShaderRegistry->GetOrAddEffect(EffectDef
		{
			.nameID = nameID
		});		
		effects[i] = EffectVariantDef 
		{
			.effectID = effectID,
			.variantID = (uint)vID
		};
	}
}

void ShaderLibGen::ClearVariant()
{
	pTable->Clear();
	pAnalyzer->Clear();
	pShaderGen->Clear();

	entrypoints.clear();
	effectBlocks.clear();
	epSet.clear();

	std::swap(libTexBuf, libTexLast);
	libTexBuf.clear();
	shaderBuf.clear();
}
