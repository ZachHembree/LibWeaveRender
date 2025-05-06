#pragma once
#include "SymbolTable.hpp"
#include "WeaveUtils/TextBlock.hpp"

namespace Weave::Effects
{
	struct LexBlock;
	class SymbolTable;
	struct ShaderEntrypoint;

	struct SourceMask
	{
		TextBlock altText;
		int startBlock;
		int blockCount;

		SourceMask();

		SourceMask(TextBlock altText, int startBlock = -1, int blockCount = -1);

		bool GetIsMasking() const;

		int GetLastBlock() const;
	};

	/// <summary>
	/// Generates HLSL from a library file
	/// </summary>
	class ShaderGenerator
	{
	public:
		ShaderGenerator();

		void GetShaderSource(const SymbolTable& table, const IDynamicArray<LexBlock>& srcBlocks, const ShaderEntrypoint& main,
			const IDynamicArray<ShaderEntrypoint>& shaders, std::string& srcOut);

		void Clear();

	private:
		UniqueVector<int> globalVarBuf;
		string globalVarDefBuf;
		UniqueVector<SourceMask> sourceMasks;

		/// <summary>
		/// Writes a copy of the source with masking applied
		/// </summary>
		void GetMaskedSource(const IDynamicArray<LexBlock>& srcBlocks, std::string& srcOut);

		/// <summary>
		/// Identifies all loose global variable symbols within the scopes visible to the given entrypoint
		/// and stores them in order
		/// </summary>
		void GetGlobalVariables(const SymbolTable& table, const int main);

		/// <summary>
		/// Generates source masks needed to produce native HLSL for the given shader
		/// </summary>
		void GetSourceMask(const SymbolTable& table, const IDynamicArray<LexBlock>& srcBlocks,
			const ShaderEntrypoint& main, const IDynamicArray<ShaderEntrypoint>& shaders);

		/// <summary>
		/// Dynamically generates a cbuffer for loose/global cbuffer variables, and masks out
		/// the originals.
		/// </summary>
		void GenerateGlobalCBuffer(const SymbolTable& table, const IDynamicArray<LexBlock>& srcBlocks);

		/// <summary>
		/// Masks out a scope along with its declaring symbol. Masks content of the scope unless
		/// otherwise indicated.
		/// </summary>
		void AddScopeMask(SymbolHandle symbol, bool isContentMasked = true);

		/// <summary>
		/// Appends the given range of blocks to the output, while ensuring the line numbers
		/// of the output remain consistent
		/// </summary>
		void AddBlockRange(const IDynamicArray<LexBlock>& srcBlocks, int start, const int end, std::string& srcOut, int& line);
	};
}