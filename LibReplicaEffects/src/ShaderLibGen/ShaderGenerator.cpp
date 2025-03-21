#include "pch.hpp"
#include "ShaderLibGen.hpp"
#include "ShaderLibGen/ShaderGenerator.hpp"
#include "ShaderLibGen/ShaderParser/BlockAnalyzer.hpp"

using namespace Replica::Effects;

SourceMask::SourceMask() : altText(nullptr, 0), startBlock(0), blockCount(0) { }

SourceMask::SourceMask(string_view altText, int startBlock, int blockCount) :
	altText(altText), startBlock(startBlock), blockCount(blockCount)
{}

bool SourceMask::GetIsMasking() const { return altText.data() != nullptr; }

int SourceMask::GetLastBlock() const { return startBlock + blockCount - 1; }

ShaderGenerator::ShaderGenerator()
{ }

void ShaderGenerator::GetShaderSource(const SymbolTable& table, const IDynamicArray<LexBlock>& srcBlocks, const ShaderEntrypoint& main,
	const IDynamicArray<ShaderEntrypoint>& shaders, std::string & srcOut)
{
	Clear();
	GetGlobalVariables(table, main.symbolID);
	GetSourceMask(table, srcBlocks, main, shaders);
	GetMaskedSource(srcBlocks, srcOut);
}

void ShaderGenerator::Clear()
{
	globalVarBuf.clear();
	globalVarDefBuf.clear();
	sourceMasks.clear();
}

static void GetCorrectedMask(SourceMask& a, const SourceMask& b)
{
	const int maxLastBlock = b.startBlock - 1,
		lastBlock = std::min(a.startBlock + a.blockCount - 1, maxLastBlock);

	a.blockCount = std::max(lastBlock - a.startBlock + 1, 0);
	a.startBlock = std::min(a.startBlock, maxLastBlock);
}

void ShaderGenerator::GetMaskedSource(const IDynamicArray<LexBlock>& srcBlocks, std::string& srcOut)
{
	// Sort masks in ascending order
	std::sort(sourceMasks.begin(), sourceMasks.end(), [](const SourceMask& a, const SourceMask& b)
	{
		return (a.startBlock + a.GetLastBlock()) < (b.startBlock + b.GetLastBlock());
	});

	// Correct overlapping masks
	for (int i = (int)sourceMasks.GetLength() - 1; i > 0; i--)
		GetCorrectedMask(sourceMasks[i - 1], sourceMasks[i]);

	int line = 1;
	int blockIndex = 0;

	for (const SourceMask& mask : sourceMasks)
	{
		if (mask.blockCount == 0)
			continue;

		// Preceeding unmasked range
		AddBlockRange(srcBlocks, blockIndex, mask.startBlock - 1, srcOut, line);

		// Append alternate text
		if (mask.altText.length() > 0)
		{
			const int lastLine = srcBlocks[mask.GetLastBlock()].GetLastLine();
			std::format_to(std::back_inserter(srcOut), "\n#line {}\n", srcBlocks[mask.startBlock].startLine);
			srcOut.append(mask.altText);
			std::format_to(std::back_inserter(srcOut), "\n#line {}\n", lastLine);
			line = lastLine;
		}

		blockIndex = mask.startBlock + mask.blockCount;
	}

	AddBlockRange(srcBlocks, blockIndex, (int)srcBlocks.GetLength() - 1, srcOut, line);
}

void ShaderGenerator::GetGlobalVariables(const SymbolTable& table, const int main)
{
	globalVarBuf.clear();
	optional<ScopeHandle> scope = table.GetSymbol(main).GetScope()->GetParentScope();

	do
	{
		for (int j = 0; j < scope->GetChildCount(); j++)
		{
			SymbolHandle child = scope->GetChild(j);

			if (child.GetHasFlags(SymbolTypes::Variable) && !child.GetHasFlags(SymbolTypes::Definition) &&
				!child.GetHasFlags(SymbolTypes::Ambiguous))
			{
				VarHandle var = *child.GetAsVar();
				ShaderTypeInfo type;
				TokenTypes modifiers;
				var.GetType(type, modifiers);

				if (!type.GetHasFlags(ShaderTypes::Resource) && (int)(modifiers & TokenTypes::TypeModifier) == 0)
					globalVarBuf.emplace_back(child.GetID());
			}
		}

		scope = scope->GetParentScope();

	} while (scope != std::nullopt);

	std::sort(globalVarBuf.begin(), globalVarBuf.end());
}

void ShaderGenerator::GetSourceMask(const SymbolTable& table, const IDynamicArray<LexBlock>& srcBlocks,
	const ShaderEntrypoint& main, const IDynamicArray<ShaderEntrypoint>& shaders)
{
	sourceMasks.clear();

	// Mask replica tokens
	for (int i = 0; i < table.GetTokenCount(); i++)
	{
		TokenNodeHandle token = table.GetToken(i);

		if (token.GetHasFlags(TokenTypes::AttribShaderDecl))
			sourceMasks.emplace_back("", token.GetBlockStart(), token.GetBlockCount());
	}

	// Mask other shaders, but retain contents of main shader block if one exists
	for (const ShaderEntrypoint& ep : shaders)
	{
		SymbolHandle func = table.GetSymbol(ep.symbolID);
		ScopeHandle scope = *func.GetScope()->GetParentScope();

		if (!scope.GetIsGlobal()) // Block declared shader
			AddScopeMask(scope.GetSymbol(), &ep != &main);
		else if (&ep != &main) // Globally declared shader
			AddScopeMask(func);
	}

	// Mask techniques and other replica symbols
	for (int symbolID = 0; symbolID < table.GetSymbolCount(); symbolID++)
	{
		SymbolHandle symbol = table.GetSymbol(symbolID);

		if (symbol.GetHasFlags(SymbolTypes::ReplicaBlock) && !symbol.GetHasFlags(SymbolTypes::Shader))
			AddScopeMask(symbol);
	}

	if (globalVarBuf.GetLength() > 0)
		GenerateGlobalCBuffer(table, srcBlocks);
}

void ShaderGenerator::GenerateGlobalCBuffer(const SymbolTable& table, const IDynamicArray<LexBlock>& srcBlocks)
{
	globalVarDefBuf.clear();
	globalVarDefBuf.append("cbuffer _EffectGlobals\n{\n");
	const int bufMaskIndex = (int)sourceMasks.GetLength();

	for (int varID : globalVarBuf)
	{
		SymbolHandle var = table.GetSymbol(varID);
		TokenNodeHandle ident = var.GetIdent();
		int firstBlock = ident.GetBlockStart(), lastBlock = firstBlock;

		// Find minimum and maximum bounds of each variable, including attributes
		for (int childID = 0; childID < ident.GetChildCount(); childID++)
		{
			TokenNodeHandle child = ident[childID];
			int block = child.GetBlockStart();

			firstBlock = std::min(firstBlock, block);
			lastBlock = std::max(lastBlock, block);
		}

		for (int blockID = firstBlock; blockID <= lastBlock; blockID++)
		{
			// Containers' contents are included in child blocks, only their bounding 
			// characters are needed
			if (srcBlocks[blockID].GetHasFlags(LexBlockTypes::StartContainer))
				globalVarDefBuf.push_back(*srcBlocks[blockID].src.GetFirst());
			else if (srcBlocks[blockID].GetHasFlags(LexBlockTypes::EndContainer))
				globalVarDefBuf.push_back(*srcBlocks[blockID].src.GetLast());
			else
				globalVarDefBuf.append(srcBlocks[blockID].src);
		}

		globalVarDefBuf.append("\n");
		sourceMasks.emplace_back("", firstBlock, lastBlock - firstBlock + 1);
	}

	globalVarDefBuf.append("}");
	sourceMasks[bufMaskIndex].altText = globalVarDefBuf;
}

void ShaderGenerator::AddScopeMask(SymbolHandle symbol, bool isContentMasked)
{
	ScopeHandle scope = *symbol.GetScope();
	TokenNodeHandle ident = symbol.GetIdent();
	int firstBlock = ident.GetBlockStart(),
		lastBlock = scope.GetBlockStart() + scope.GetBlockCount() - 1;

	if (isContentMasked)
		sourceMasks.emplace_back("", firstBlock, lastBlock - firstBlock + 1);
	else
	{
		sourceMasks.emplace_back("", firstBlock, scope.GetBlockStart() - firstBlock + 1);
		sourceMasks.emplace_back("", lastBlock, 1);
	}
}

void ShaderGenerator::AddBlockRange(const IDynamicArray<LexBlock>& srcBlocks, int start, const int end, std::string& srcOut, int& line)
{
	while (start <= end)
	{
		const LexBlock& block = srcBlocks[start];

		if (!block.GetHasFlags(LexBlockTypes::EndContainer))
		{
			if ((block.startLine - line) > 3)
			{
				std::format_to(std::back_inserter(srcOut), "\n#line {}\n", block.startLine);
				line = block.startLine;
			}
			else while (line < block.startLine)
			{
				srcOut.push_back('\n');
				line++;
			}
		}

		if (block.GetHasFlags(LexBlockTypes::StartContainer))
			srcOut.push_back(*block.src.GetFirst());
		else if (block.GetHasFlags(LexBlockTypes::EndContainer))
				srcOut.push_back(*block.src.GetLast());
		else
		{
			srcOut.append(block.src);
			line += block.lineCount;
		}

		start++;
	}
}
