#pragma once
#include <unordered_set>
#include <memory>
#include "ShaderLibGen/ShaderEntrypoint.hpp"
#include "ShaderLibGen/ShaderDataSerializers.hpp"

namespace Replica::Effects
{
	using std::unique_ptr;

	class VariantPreprocessor;
	class BlockAnalyzer;
	class SymbolTable;
	class ShaderGenerator;

	/// <summary>
	/// Generates preprocessed and precompiled shader and effect variants and corresponding
	/// metadata from source
	/// </summary>
	class ShaderLibGen
	{
	public:
		MAKE_MOVE_ONLY(ShaderLibGen)

		ShaderLibGen();

		~ShaderLibGen();

		ShaderLibDef GetLibrary(string_view libPath, string_view libSrc);

		void Clear();

	private:
		unique_ptr<VariantPreprocessor> pVariantGen;
		unique_ptr<BlockAnalyzer> pAnalyzer;
		unique_ptr<SymbolTable> pTable;
		unique_ptr<ShaderGenerator> pShaderGen;

		string libBuf;
		std::unordered_set<string_view> epSet;
		UniqueVector<ShaderEntrypoint> entrypoints;
		string shaderBuf;

		/// <summary>
		/// Initializes the library variants and corresponding flags
		/// </summary>
		void InitLibrary(ShaderLibDef& lib);

		/// <summary>
		/// Generates shader definitions for every shader in a variant
		/// </summary>
		void GetShaderDefs(string_view libPath, DynamicArray<ShaderDef>& shaders);

		/// <summary>
		/// Identifies shaders in the source and buffers their entrypoint symbols
		/// </summary>
		void GetEntryPoints();

		/// <summary>
		/// Resets tables for next variant
		/// </summary>
		void ClearVariant();
	};
}