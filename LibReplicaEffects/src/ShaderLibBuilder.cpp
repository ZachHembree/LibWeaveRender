#pragma once
#include "pch.hpp"
#include "ReplicaEffects/ShaderLibBuilder/ShaderParser/BlockAnalyzer.hpp"
#include "ReplicaEffects/ShaderLibBuilder/SymbolTable.hpp"
#include "ReplicaEffects/ShaderLibBuilder/ShaderGenerator.hpp"
#include "ReplicaEffects/ShaderLibBuilder/ShaderCompiler.hpp"
#include "ReplicaEffects/ShaderLibBuilder/VariantPreprocessor.hpp"
#include "ReplicaEffects/ShaderLibBuilder/ShaderRegistryBuilder.hpp"
#include "ReplicaEffects/ShaderLibBuilder.hpp"

using namespace Replica::Effects;

ShaderLibBuilder::ShaderLibBuilder() :
	pVariantGen(new VariantPreprocessor()),
	pAnalyzer(new BlockAnalyzer()),
	pTable(new SymbolTable()),
	pShaderGen(new ShaderGenerator()),
	pShaderRegistry(new ShaderRegistryBuilder()),
	isDebugging(false),
	libBufIndex(0)
{
	platform = PlatformDef
	{
		.compilerVersion = string(GetCompilerVersionD3D11()),
		.featureLevel = string("5_0"),
		.target = PlatformTargets::DirectX11
	};
}

ShaderLibBuilder::~ShaderLibBuilder() = default;

void ShaderLibBuilder::AddRepo(string_view name, string_view libPath, string_view libSrc)
{
	const uint repoID = (uint)repos.GetLength() << g_VariantGroupOffset;
	VariantRepoDef& lib = repos.EmplaceBack();
	lib.src.name = name;
	lib.src.path = libPath;
	pVariantGen->SetSrc(libPath, libSrc);

	for (uint configID = 0; configID < pVariantGen->GetVariantCount(); configID++)
	{
		const uint vID = repoID | configID;
		VariantSrcBuf* pBuf = &libBufs[libBufIndex];
		pBuf->libText.clear();
		pBuf->vID = configID;

		// Generate variant
		pVariantGen->GetVariant(configID, pBuf->libText, entrypoints);
		bool isDuplicate = false;

		for (int i = 0; i < std::size(libBufs); i++)
		{
			if (libBufIndex != i && pBuf->libText == libBufs[i].libText)
			{
				isDuplicate = true;
				pBuf = &libBufs[i];
				break;
			}
		}

		if (!isDuplicate) // Parse and precompile variants
		{
			const uint resCount = pShaderRegistry->GetUniqueResCount();

			pAnalyzer->AnalyzeSource(libPath, pBuf->libText);
			pTable->ParseBlocks(*pAnalyzer);

			if (configID == 0)
				InitVariants(lib);

			// Shaders
			GetEntryPoints();
			lib.variants[configID].shaders = DynamicArray<ShaderVariantDef>(entrypoints.GetLength());
			GetShaderDefs(libPath, lib.variants[configID].shaders, vID);

			// Effects
			GetEffects();
			lib.variants[configID].effects = DynamicArray<EffectVariantDef>(effectBlocks.GetLength());
			GetEffectDefs(lib.variants[configID].effects, vID);

			if (resCount == pShaderRegistry->GetUniqueResCount())
				LOG_WARN() << "Unused flag/mode combination detected. ID: " << vID << ". Not skipped.";

			libBufIndex++;
			libBufIndex %= std::size(libBufs);
		}
		else // Skip processing
		{
			// Copy variant mappings and update ID
			lib.variants[configID] = lib.variants[pBuf->vID];

			for (ShaderVariantDef& shader : lib.variants[configID].shaders)
				shader.variantID = configID;

			for (EffectVariantDef& effect : lib.variants[configID].effects)
				effect.variantID = configID;

			LOG_WARN() << "Unused flag/mode combination detected. ID: " << vID << ". Skipped.";
		}

		ClearVariant();
	}
}

ShaderLibDef::Handle ShaderLibBuilder::GetDefinition() const
{
	return
	{
		.pPlatform = &platform,
		.pRepos = &repos,
		.regHandle = pShaderRegistry->GetDefinition(),
		.strMapHandle = pShaderRegistry->GetStringIDBuilder().GetDefinition()
	};
}

void ShaderLibBuilder::SetTarget(PlatformTargets target) { platform.target = target; }

void ShaderLibBuilder::SetFeatureLevel(string_view featureLevel) { platform.featureLevel = featureLevel; }

void ShaderLibBuilder::SetDebug(bool isDebugging) { this->isDebugging = isDebugging; }

void ShaderLibBuilder::InitVariants(VariantRepoDef& lib)
{
	const IDynamicArray<StringSpan>& flags = pVariantGen->GetVariantFlags();
	lib.flagIDs = DynamicArray<uint>(flags.GetLength());

	for (int i = 0; i < flags.GetLength(); i++)
		lib.flagIDs[i] = pShaderRegistry->GetOrAddStringID(flags[i]);

	const IDynamicArray<StringSpan>& modes = pVariantGen->GetVariantModes();
	lib.modeIDs = DynamicArray<uint>(modes.GetLength());

	for (int i = 0; i < modes.GetLength(); i++)
		lib.modeIDs[i] = pShaderRegistry->GetOrAddStringID(modes[i]);

	lib.variants = DynamicArray<VariantDef>(pVariantGen->GetVariantCount());

	LOG_INFO() << "Variants declared: " << lib.variants.GetLength();
}

void ShaderLibBuilder::GetEntryPoints()
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
					const uint nameID = pShaderRegistry->GetOrAddStringID(name);

					if (!epNameShaderIDMap.contains(nameID))
					{
						ShaderEntrypoint& ep = entrypoints.EmplaceBack();
						epNameShaderIDMap.emplace(nameID, -1);
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
			const uint nameID = pShaderRegistry->GetOrAddStringID(ep.name);

			if (!epNameShaderIDMap.contains(nameID))
			{
				epNameShaderIDMap.emplace(nameID, -1);
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
				const uint nameID = pShaderRegistry->GetOrAddStringID(name);

				if (!epNameShaderIDMap.contains(nameID))
				{
					ShaderEntrypoint& ep = entrypoints.EmplaceBack();
					epNameShaderIDMap.emplace(nameID, -1);
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

void ShaderLibBuilder::GetEffects()
{
	for (int i = 0; i < pTable->GetSymbolCount(); i++)
	{
		SymbolHandle symbol = pTable->GetSymbol(i);

		if (symbol.GetIsScope() && symbol.GetHasFlags(SymbolTypes::TechniqueDef))
		{
			ScopeHandle effectScope = *symbol.GetScope();
			EffectBlock& effect = effectBlocks.EmplaceBack();
			effect.nameID = pShaderRegistry->GetOrAddStringID(symbol.GetName());
			effect.passStart = 0;
			effect.passStart = (uint)effectPasses.GetLength();
			effect.passCount = 0; 
			bool isPassDefaulted = false;

			for (int j = 0; j < effectScope.GetChildCount(); j++)
			{
				SymbolHandle effectChild = effectScope[j];

				if (!isPassDefaulted && effectChild.GetHasFlags(SymbolTypes::TechniqueShaderDecl))
					isPassDefaulted = true;

				if (effectChild.GetHasFlags(SymbolTypes::TechniquePassDecl))
				{
					PARSE_ASSERT_MSG(!isPassDefaulted, 
						"Defaulted passes and explicit passes cannot be used in the same effect.")

					effect.passCount++;
				}
			}

			if (isPassDefaulted)
			{
				AddPass(effectScope, "DefaultedPass");
				effect.passCount = 1;
			}
			else
			{
				for (int j = 0; j < effectScope.GetChildCount(); j++)
				{
					SymbolHandle effectChild = effectScope[j];

					if (effectChild.GetHasFlags(SymbolTypes::TechniquePassDecl))
					{
						ScopeHandle passScope = *effectChild.GetScope();
						AddPass(passScope, effectChild.GetName());
					}
				}
			}
		}
	}
}

void ShaderLibBuilder::AddPass(const ScopeHandle& passScope, string_view name)
{
	PassBlock& pass = effectPasses.EmplaceBack();
	pass.nameID = pShaderRegistry->GetOrAddStringID(name);
	pass.shaderStart = (uint)effectShaders.GetLength();

	for (int j = 0; j < passScope.GetChildCount(); j++)
	{
		SymbolHandle effectChild = passScope[j];

		if (effectChild.GetHasFlags(SymbolTypes::TechniqueShaderDecl))
		{
			string_view shaderName = effectChild.GetName();
			const uint stringID = pShaderRegistry->GetOrAddStringID(shaderName);
			const uint shaderID = epNameShaderIDMap[stringID];
			effectShaders.EmplaceBack(shaderID);
		}
	}

	pass.shaderCount = (uint)effectShaders.GetLength() - pass.shaderStart;
}

void ShaderLibBuilder::GetShaderDefs(string_view libPath, DynamicArray<ShaderVariantDef>& variants, uint vID)
{
	for (int i = 0; i < entrypoints.GetLength(); i++)
	{
		hlslBuf.clear();

		const ShaderEntrypoint& ep = entrypoints[i];
		pShaderGen->GetShaderSource(*pTable, pAnalyzer->GetBlocks(), ep, entrypoints, hlslBuf);

		const uint shaderID = GetShaderDefD3D11(libPath, hlslBuf, platform.featureLevel, ep.stage, 
			ep.name, *pShaderRegistry, isDebugging);
		const uint nameID = pShaderRegistry->GetShader(shaderID).nameID;

		variants[i].shaderID = shaderID;
		variants[i].variantID = vID;
		// Update name -> shader ID key
		epNameShaderIDMap[nameID] = shaderID;
	}
}

void ShaderLibBuilder::GetEffectDefs(DynamicArray<EffectVariantDef>& effects, uint vID)
{
	// Effects
	for (uint i = 0; i < (uint)effectBlocks.GetLength(); i++)
	{
		const EffectBlock& block = effectBlocks[i];
		EffectDef effect;
		effect.nameID = block.nameID;
		Vector<uint> passBuf = pShaderRegistry->GetTmpIDBuffer();

		// Passes
		for (uint j = 0; j < block.passCount; j++)
		{
			PassBlock& pass = effectPasses[block.passStart + j];
			Vector<uint> idBuf = pShaderRegistry->GetTmpIDBuffer();

			// Shaders
			for (uint k = 0; k < pass.shaderCount; k++)
			{
				const uint shaderIndex = pass.shaderStart + k;
				idBuf.EmplaceBack(effectShaders[shaderIndex]);
			}

			passBuf.EmplaceBack(pShaderRegistry->GetOrAddIDGroup(idBuf));
			pShaderRegistry->ReturnTmpIDBuffer(std::move(idBuf));
		}

		effect.passGroupID = pShaderRegistry->GetOrAddIDGroup(passBuf);
		pShaderRegistry->ReturnTmpIDBuffer(std::move(passBuf));

		effects[i] = EffectVariantDef 
		{
			.effectID = pShaderRegistry->GetOrAddEffect(effect),
			.variantID = vID
		};
	}
}

void ShaderLibBuilder::Clear()
{
	ClearVariant();
	libBufIndex = 0;

	for (int i = 0; i < std::size(libBufs); i++)
	{
		libBufs[i].libText.clear();
		libBufs[i].vID = 0;
	}

	repos.Clear();
	pVariantGen->Clear();
	pShaderRegistry->Clear();
}

void ShaderLibBuilder::ClearVariant()
{
	pTable->Clear();
	pAnalyzer->Clear();
	pShaderGen->Clear();

	entrypoints.Clear();
	epNameShaderIDMap.clear();

	effectBlocks.Clear();
	effectPasses.Clear();
	effectShaders.Clear();

	hlslBuf.clear();
}
