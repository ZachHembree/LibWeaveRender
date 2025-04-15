#pragma once
#include <unordered_map>
#include <memory>
#include "WeaveEffects/ShaderLibBuilder/ShaderEntrypoint.hpp"
#include "WeaveEffects/ShaderDataSerialization.hpp"

namespace Weave::Effects
{
	using std::unique_ptr;

	class VariantPreprocessor;
	class BlockAnalyzer;
	class SymbolTable;
	class ShaderGenerator;
	class ShaderRegistryBuilder;
	class ScopeHandle;

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
		void AddRepo(string_view name, string_view libPath, string_view libSrc);

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
		/// Returns a serializable library handle containing all preprocessed source 
		/// data and their variants added via AddRepo().
		/// </summary>
		ShaderLibDef::Handle GetDefinition() const;

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

		struct VariantSrcBuf
		{
			string libText;
			uint vID;
		};

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
		VariantSrcBuf libBufs[4];
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

		/// <summary>
		/// Initializes the library variants and corresponding flags
		/// </summary>
		void InitVariants(VariantRepoDef& lib);

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