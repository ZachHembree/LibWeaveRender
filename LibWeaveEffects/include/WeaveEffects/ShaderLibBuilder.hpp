#pragma once
#include <unordered_map>
#include <memory>
#include "WeaveEffects/ShaderData.hpp"
#include "WeaveEffects/ShaderLibBuilder/ShaderEntrypoint.hpp"

namespace Weave::Effects
{
	using std::unique_ptr;

	class VariantPreprocessor;
	class BlockAnalyzer;
	class SymbolTable;
	class ShaderGenerator;
	class ShaderRegistryBuilder;
	class ScopeHandle;
	class ShaderLibMap;

	struct ShaderLibCacheStats
	{
		uint cachedRepoCount;
		uint cachedShaderCount;
		uint cachedEffectCount;
		uint cachedResourceCount;
	};

	/// <summary>
	/// Generates preprocessed, precompiled shader and effect variants with corresponding
	/// metadata from source
	/// </summary>
	class ShaderLibBuilder
	{
	public:
		MAKE_MOVE_ONLY(ShaderLibBuilder)

		ShaderLibBuilder();

		~ShaderLibBuilder();

		/// <summary>
		/// Preprocesses the given effect source file and adds it to the builder.
		/// Invalidates definition handles.
		/// </summary>
		void AddRepo(string_view repoPath, string_view libSrc);

		/// <summary>
		/// Sets the name for the library being built.
		/// </summary>
		void SetName(string_view name);

		/// <summary>
		/// Sets target graphics API
		/// </summary>
		void SetTarget(PlatformTargets target);

		/// <summary>
		/// Configures shader model feature level
		/// </summary>
		void SetFeatureLevel(string_view featureLevel);

		/// <summary>
		/// Enables/disables debugging in precompiled shaders
		/// </summary>
		void SetDebug(bool isDebugging);

		/// <summary>
		/// Assigns a preexisting shader library to be used as a cache, allowing definitions to 
		/// be reused if their source hasn't changed in the cache.
		/// </summary>
		void SetCache(const ShaderLibDef::Handle& cachedDef);

		/// <summary>
		/// Assigns a preexisting shader library to be used as a cache, allowing definitions to 
		/// be reused if their source hasn't changed in the cache.
		/// </summary>
		void SetCache(ShaderLibDef&& cachedDef);

		/// <summary>
		/// Returns a serializable library handle containing all preprocessed source 
		/// data and their variants added via AddRepo().
		/// </summary>
		ShaderLibDef::Handle GetDefinition();

		/// <summary>
		/// Resets the builder for reuse. Invalidates definition handles.
		/// </summary>
		void Clear();

	private:
		struct PassBlock
		{
			uint nameID;
			uint shaderStart;
			uint shaderCount;
		};

		struct EffectBlock
		{
			uint nameID;
			uint passStart;
			uint passCount;
		};

		struct RepoSrcBuf
		{
			string libText;
			uint configID;
		};

		string name;
		PlatformDef platform;
		UniqueVector<VariantRepoDef> repos;
		bool isDebugging;

		// Parsing, code gen and reflection
		unique_ptr<ShaderRegistryBuilder> pShaderRegistry;
		unique_ptr<VariantPreprocessor> pVariantGen;

		unique_ptr<BlockAnalyzer> pAnalyzer;
		unique_ptr<SymbolTable> pTable;
		unique_ptr<ShaderGenerator> pShaderGen;

		// Variant buffers
		RepoSrcBuf libBufs[4];
		uint libBufIndex;
		string hlslBuf;

		// Shader mains
		UniqueVector<ShaderEntrypoint> entrypoints;
		// nameID -> shaderID
		std::unordered_map<uint, uint> epNameShaderIDMap;

		// Effect buffers
		UniqueVector<EffectBlock> effectBlocks;
		UniqueVector<PassBlock> effectPasses;
		UniqueVector<uint> effectShaders;

		// Caching
		ShaderLibCacheStats cacheStats;
		unique_ptr<ShaderLibMap> pCacheMap;
		std::unordered_map<string_view, uint> pathRepoMap;
		Vector<const VariantRepoDef*> cacheHits;
		ShaderLibDef::Handle lastDefHandle;

		/// <summary>
		/// Initializes the variant repo and corresponding flags
		/// </summary>
		void InitRepo(VariantRepoDef& lib);

		/// <summary>
		/// Preprocesses all effects and shaders in the variant repo configuration
		/// </summary>
		void AddRepoConfiguration(string_view repoPath, const uint configID, const uint repoID, VariantRepoDef& repo);

		/// <summary>
		/// Copies unchanged definitions from the cache into the final definition
		/// </summary>
		void MergeCacheHits();

		/// <summary>
		/// Attempts to retrieve a repo from the cache based on its original source path
		/// </summary>
		const VariantRepoDef* TryGetCachedRepo(string_view path) const;

		/// <summary>
		/// Identifies shaders in the source and buffers their entrypoint symbols
		/// </summary>
		void GetEntryPoints();

		/// <summary>
		/// Identifies effects and passes defined
		/// </summary>
		void GetEffects();

		/// <summary>
		/// Adds an effect pass to the buffer to be later converted into a definition
		/// </summary>
		void AddPass(const ScopeHandle& effectScope, string_view name);

		/// <summary>
		/// Generates shader definitions for every shader in a variant
		/// </summary>
		void GetShaderDefs(string_view libPath, DynamicArray<ShaderVariantDef>& variants, uint resID);

		/// <summary>
		/// Generates effect definitions for every effect in a variant
		/// </summary>
		void GetEffectDefs(DynamicArray<EffectVariantDef>& effects, uint resID);

		/// <summary>
		/// Resets tables for next variant
		/// </summary>
		void ClearVariant();
	};
}