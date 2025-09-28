#pragma once
#include "pch.hpp"
#include "WeaveUtils/Compression.hpp"
#include "WeaveEffects/ShaderLibBuilder/ShaderParser/BlockAnalyzer.hpp"
#include "WeaveEffects/ShaderLibBuilder/SymbolTable.hpp"
#include "WeaveEffects/ShaderLibBuilder/ShaderGenerator.hpp"
#include "WeaveEffects/ShaderLibBuilder/ShaderCompiler.hpp"
#include "WeaveEffects/ShaderLibBuilder/VariantPreprocessor.hpp"
#include "WeaveEffects/ShaderLibBuilder/ShaderRegistryBuilder.hpp"
#include "WeaveEffects/ShaderLibMap.hpp"
#include "WeaveEffects/ShaderLibBuilder.hpp"
#include "WeaveEffects/Version.hpp"

using namespace Weave::Effects;

ShaderLibBuilder::ShaderLibBuilder() :
	pVariantGen(new VariantPreprocessor()),
	pAnalyzer(new BlockAnalyzer()),
	pTable(new SymbolTable()),
	pShaderGen(new ShaderGenerator()),
	pShaderRegistry(new ShaderRegistryBuilder()),
	isDebugging(false),
	libBufIndex(0),
	cacheStats({}),
	lastDefHandle({})
{
	platform = PlatformDef
	{
		.preprocessorVersion = VERSION_STRING,
		.preprocessorBuild = VERSION_BUILD,
		.compilerVersion = string(GetCompilerVersionD3D11()),
		.featureLevel = "5_0",
		.target = PlatformTargets::DirectX11
	};
}

ShaderLibBuilder::~ShaderLibBuilder() = default;

/* 
	Configuration 
*/

void ShaderLibBuilder::SetName(string_view newName)
{
	name.clear();
	name.append(newName);
}

void ShaderLibBuilder::SetTarget(PlatformTargets target) { platform.target = target; }

void ShaderLibBuilder::SetFeatureLevel(string_view featureLevel) { platform.featureLevel = featureLevel; }

void ShaderLibBuilder::SetDebug(bool isDebugging) { this->isDebugging = isDebugging; }

void ShaderLibBuilder::SetCache(const ShaderLibDef::Handle& cachedDef)
{
	if (platform == *cachedDef.pPlatform)
	{
		if (pCacheMap.get() != nullptr && pCacheMap->GetName() == *cachedDef.pName)
			return;

		WV_LOG_INFO() << "Using shader cache for " << *cachedDef.pName;
		pCacheMap.reset(new ShaderLibMap(cachedDef));
		pathRepoMap.clear();

		const auto& repos = pCacheMap->GetRepos();

		for (uint i = 0; i < (uint)repos.GetLength(); i++)
			pathRepoMap.emplace(repos[i].path, i);
	}
	else
		WV_LOG_INFO() << "Shader cache version mismatch for " << *cachedDef.pName << ". Falling back to full reprocessing...";
}

void ShaderLibBuilder::SetCache(ShaderLibDef&& cachedDef)
{
	if (platform == cachedDef.platform)
	{
		if (pCacheMap.get() != nullptr && pCacheMap->GetName() == cachedDef.name)
			return;

		WV_LOG_INFO() << "Using shader cache for " << cachedDef.name;
		pCacheMap.reset(new ShaderLibMap(std::move(cachedDef)));
		pathRepoMap.clear();

		const auto& repos = pCacheMap->GetRepos();

		for (uint i = 0; i < (uint)repos.GetLength(); i++)
			pathRepoMap.emplace(repos[i].path, i);
	}
	else
		WV_LOG_INFO() << "Shader cache version mismatch for " << cachedDef.name << ". Falling back to full reprocessing...";
}

/* 
	Main Processing 
*/

void ShaderLibBuilder::AddRepo(string_view repoPath, string_view libSrc)
{
	FX_CHECK_MSG((!repoPath.empty() && !libSrc.empty()), "Invalid input: repoPath or libSrc is empty");

	const uint repoID = (uint)repos.GetLength() << g_VariantGroupOffset;
	const uint crc = GetCRC32(libSrc);
	pVariantGen->SetSrc(repoPath, libSrc);

	// Check repo cache
	if (const VariantRepoDef* pRepo = TryGetCachedRepo(repoPath); pRepo != nullptr)
	{
		if (libSrc.length() == pRepo->sourceSizeBytes && crc == pRepo->sourceCRC)
		{
			WV_LOG_DEBUG() << "Cache hit for repository: " << repoPath;
			cacheHits.Add(pRepo);
		}
		else
			WV_LOG_DEBUG() << "Cache miss for " << repoPath << ": source changed, reprocessing";
	}
	else // Fall back to full processing
	{	
		VariantRepoDef& repo = repos.EmplaceBack();
		repo.sourceSizeBytes = (uint)libSrc.length();
		repo.sourceCRC = crc;
		repo.path = repoPath;

		for (uint configID = 0; configID < pVariantGen->GetVariantCount(); configID++)
		{
			AddRepoConfiguration(repoPath, configID, repoID, repo);
			ClearVariant();
		}
	}
}

const VariantRepoDef* ShaderLibBuilder::TryGetCachedRepo(string_view path) const
{
	if (pCacheMap.get() == nullptr)
		return nullptr;

	const auto& it = pathRepoMap.find(path);
	return (it != pathRepoMap.end()) ? &pCacheMap->GetRepos()[it->second] : nullptr;
}

void ShaderLibBuilder::AddRepoConfiguration(string_view repoPath, const uint configID, const uint repoID, VariantRepoDef& repo)
{
	const uint vID = repoID | configID;
	RepoSrcBuf* pBuf = &libBufs[libBufIndex];
	pBuf->libText.clear();
	pBuf->configID = configID;

	// Generate variant
	pVariantGen->GetVariant(configID, pBuf->libText, entrypoints);

	// Update variant history and check for repeats
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

	// Initialize storage
	if (configID == 0)
		InitRepo(repo);

	if (!isDuplicate) // Parse and precompile variants
	{
		const uint resCount = pShaderRegistry->GetUniqueResCount();

		pAnalyzer->AnalyzeSource(repoPath, pBuf->libText);
		pTable->ParseBlocks(*pAnalyzer);

		// Shaders
		GetEntryPoints();
		repo.variants[configID].shaders = DynamicArray<ShaderVariantDef>(entrypoints.GetLength());
		GetShaderDefs(repoPath, repo.variants[configID].shaders, vID);

		// Effects
		GetEffects();
		repo.variants[configID].effects = DynamicArray<EffectVariantDef>(effectBlocks.GetLength());
		GetEffectDefs(repo.variants[configID].effects, vID);

		if (resCount == pShaderRegistry->GetUniqueResCount())
			WV_LOG_WARN() << "Unused flag/mode combination detected. ID: " << vID << ". Not skipped.";

		libBufIndex++;
		libBufIndex %= std::size(libBufs);
	}
	else // Skip processing
	{
		// Copy variant mappings and update ID
		repo.variants[configID] = repo.variants[pBuf->configID];

		for (ShaderVariantDef& shader : repo.variants[configID].shaders)
			shader.variantID = vID;

		for (EffectVariantDef& effect : repo.variants[configID].effects)
			effect.variantID = vID;

		WV_LOG_WARN() << "Unused flag/mode combination detected. ID: " << vID << ". Skipped.";
	}
}

void ShaderLibBuilder::InitRepo(VariantRepoDef& lib)
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

	WV_LOG_INFO() << "Variants declared: " << lib.variants.GetLength();
	FX_CHECK_MSG(lib.variants.GetLength() != 0, "No shaders found.");
}

ShaderLibDef::Handle ShaderLibBuilder::GetDefinition()
{
	// Cache merging deferred
	if (cacheHits.GetLength() > 0)
	{
		// Fully cached
		if (cacheHits.GetLength() == pCacheMap->GetRepos().GetLength() && repos.IsEmpty())
		{
			WV_LOG_INFO() << "No changes detected. Reusing shader cache.";
			cacheHits.Clear();
			lastDefHandle = pCacheMap->GetDefinition();
		}
		// Partially cached
		else
		{
			MergeCacheHits();

			WV_LOG_INFO() << "Reused "
				<< cacheStats.cachedEffectCount << " of " << pShaderRegistry->GetEffectCount() << " effects and "
				<< cacheStats.cachedShaderCount << " of " << pShaderRegistry->GetShaderCount() << " shaders from cache.";
		}
	}

	if (!lastDefHandle.GetIsValid())
	{
		lastDefHandle =
		{
			.pName = &name,
			.pPlatform = &platform,
			.pRepos = &repos,
			.regHandle = pShaderRegistry->GetDefinition(),
			.strMapHandle = pShaderRegistry->GetStringIDBuilder().GetDefinition()
		};
	}

	return lastDefHandle;
}

void ShaderLibBuilder::MergeCacheHits()
{
	const uint newRepoCount = (uint)repos.GetLength();
	const uint newShaderCount = pShaderRegistry->GetShaderCount();
	const uint newEffectCount = pShaderRegistry->GetEffectCount();
	const uint newResourceCount = pShaderRegistry->GetResourceCount();

	for (const VariantRepoDef* pRepo : cacheHits)
	{
		const uint repoID = (uint)repos.GetLength() << g_VariantGroupOffset;
		VariantRepoDef& repo = repos.EmplaceBack(*pRepo);
		const StringIDMap& oldStrings = pCacheMap->GetStringMap();

		// Remap define names
		for (uint& flagID : repo.flagIDs)
			flagID = pShaderRegistry->GetOrAddStringID(oldStrings.GetString(flagID));

		for (uint& modeID : repo.modeIDs)
			modeID = pShaderRegistry->GetOrAddStringID(oldStrings.GetString(modeID));

		// Remap shaders
		for (VariantDef& variant : repo.variants)
		{
			for (ShaderVariantDef& shader : variant.shaders)
			{
				const ShaderDefHandle handle = pCacheMap->GetShader(shader.shaderID);
				shader.shaderID = pShaderRegistry->GetOrAddShader(handle);
				shader.variantID = (shader.variantID & g_VariantMask) | repoID;
			}

			for (EffectVariantDef& effect : variant.effects)
			{
				const EffectDefHandle handle = pCacheMap->GetEffect(effect.effectID);
				effect.effectID = pShaderRegistry->GetOrAddEffect(handle);
				effect.variantID = (effect.variantID & g_VariantMask) | repoID;
			}
		}
	}

	cacheHits.Clear();
	cacheStats.cachedRepoCount = (uint)repos.GetLength() - newRepoCount;
	cacheStats.cachedShaderCount = pShaderRegistry->GetShaderCount() - newShaderCount;
	cacheStats.cachedEffectCount = pShaderRegistry->GetEffectCount() - newEffectCount;
	cacheStats.cachedResourceCount = pShaderRegistry->GetResourceCount() - newResourceCount;
}

void ShaderLibBuilder::Clear()
{
	ClearVariant();
	libBufIndex = 0;

	for (int i = 0; i < std::size(libBufs); i++)
	{
		libBufs[i].libText.clear();
		libBufs[i].configID = 0;
	}

	repos.Clear();
	pVariantGen->Clear();
	pShaderRegistry->Clear();
	name.clear();

	pCacheMap.reset();
	pathRepoMap.clear();
	cacheHits.Clear();
	cacheStats = {};
	lastDefHandle = {};
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

/* 
	Metadata Analysis 
*/

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
			FXBLOCK_THROW(*pAnalyzer, global.GetBlockStart(), 
				"Definition for shader '{}' declared in pragma not found", ep.name);
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
				FXBLOCK_THROW(*pAnalyzer, symbol.GetIdent().GetBlockStart(),
					"Could not find entrypoint for shader block '{}'", name);
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
					FXBLOCK_CHECK_MSG(!isPassDefaulted, *pAnalyzer, symbol.GetIdent().GetBlockStart(),
						"Illegal use of defaulted and explicit passes in the same effect '{}'", symbol.GetName());
						
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
			const auto& it = epNameShaderIDMap.find(stringID);

			FXBLOCK_CHECK_MSG(it != epNameShaderIDMap.end(), *pAnalyzer, effectChild.GetIdent().GetBlockStart(),
				"Unrecognised shader name '{}' declared in effect pass", shaderName);

			const uint shaderID = it->second;
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
