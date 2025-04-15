#include "pch.hpp"
#include "WeaveEffects/ShaderLibBuilder.hpp"
#include "WeaveEffects/ShaderLibBuilder/ShaderGenerator.hpp"
#include "WeaveEffects/ShaderLibBuilder/ShaderParser/BlockAnalyzer.hpp"

using namespace Weave::Effects;

static void AppendLineDirective(int line, string& srcOut)
{
	if (!srcOut.empty() && srcOut.back() != '\n')
		srcOut.push_back('\n');

	std::format_to(std::back_inserter(srcOut), "#line {}\n", line);
}

SourceMask::SourceMask() : altText(), startBlock(0), blockCount(0) { }

SourceMask::SourceMask(TextBlock altText, int startBlock, int blockCount) :
	altText(altText), startBlock(startBlock), blockCount(blockCount)
{}

bool SourceMask::GetIsMasking() const { return altText.GetData() != nullptr; }

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
	globalVarBuf.Clear();
	globalVarDefBuf.clear();
	sourceMasks.Clear();
}

static void GetCorrectedMask(SourceMask& a, SourceMask& b)
{
	// Push superset masks back toward the front
	if (a.startBlock > b.startBlock)
		std::swap(a, b);

	const int endA = a.startBlock + a.blockCount,
		endB = b.startBlock + b.blockCount;

	if (endA >= endB) // A is a superset of B
	{
		b.startBlock = 0;
		b.blockCount = 0;
	}
	else // Partial overlap
	{ 
		const int maxLastBlock = b.startBlock - 1,
			lastBlock = std::min(a.startBlock + a.blockCount - 1, maxLastBlock);

		a.blockCount = std::max(lastBlock - a.startBlock + 1, 0);
		a.startBlock = std::min(a.startBlock, maxLastBlock);
	}
}

/*
	Generates final source by applying masks, first sorting them and resolving overlaps.
	Masks are sorted ascending by last block index, then a backward pass uses GetCorrectedMask
	on adjacent pairs to resolve overlaps. GetCorrectedMask nullifies fully contained masks
	or truncates earlier masks in partial overlaps, ensuring a final, non-overlapping set
	for generation before applying them to the source blocks.
*/
void ShaderGenerator::GetMaskedSource(const IDynamicArray<LexBlock>& srcBlocks, std::string& srcOut)
{
	// Sort masks in ascending order
	std::sort(sourceMasks.begin(), sourceMasks.end(), [](const SourceMask& a, const SourceMask& b)
	{
		return a.GetLastBlock() < b.GetLastBlock();
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
		if (mask.altText.GetLength() > 0)
		{
			const int lastLine = srcBlocks[mask.GetLastBlock()].GetLastLine();
			const int startLine = srcBlocks[mask.startBlock].startLine;

			if (line != startLine)
				AppendLineDirective(srcBlocks[mask.startBlock].startLine, srcOut);
				
			srcOut.append(mask.altText);
			line = startLine + mask.altText.FindCount('\n');
		}

		blockIndex = mask.startBlock + mask.blockCount;
	}

	AddBlockRange(srcBlocks, blockIndex, (int)srcBlocks.GetLength() - 1, srcOut, line);
}

void ShaderGenerator::GetGlobalVariables(const SymbolTable& table, const int main)
{
	globalVarBuf.Clear();
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
					globalVarBuf.EmplaceBack(child.GetID());
			}
		}

		scope = scope->GetParentScope();

	} while (scope != std::nullopt);

	std::sort(globalVarBuf.begin(), globalVarBuf.end());
}

void ShaderGenerator::GetSourceMask(const SymbolTable& table, const IDynamicArray<LexBlock>& srcBlocks,
	const ShaderEntrypoint& main, const IDynamicArray<ShaderEntrypoint>& shaders)
{
	sourceMasks.Clear();

	// Mask replica tokens
	for (int i = 0; i < table.GetTokenCount(); i++)
	{
		TokenNodeHandle token = table.GetToken(i);

		if (token.GetHasFlags(TokenTypes::AttribShaderDecl))
			sourceMasks.EmplaceBack("", token.GetBlockStart(), token.GetBlockCount());
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

		if (symbol.GetHasFlags(SymbolTypes::Weave) && !symbol.GetHasFlags(SymbolTypes::Shader))
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
				globalVarDefBuf.push_back(srcBlocks[blockID].src.GetFront());
			else if (srcBlocks[blockID].GetHasFlags(LexBlockTypes::EndContainer))
				globalVarDefBuf.push_back(srcBlocks[blockID].src.GetBack());
			else
				globalVarDefBuf.append(srcBlocks[blockID].src);
		}

		globalVarDefBuf.append("\n");
		sourceMasks.EmplaceBack("", firstBlock, lastBlock - firstBlock + 1);
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
		sourceMasks.EmplaceBack("", firstBlock, lastBlock - firstBlock + 1);
	else
	{
		sourceMasks.EmplaceBack("", firstBlock, scope.GetBlockStart() - firstBlock + 1);
		sourceMasks.EmplaceBack("", lastBlock, 1);
	}
}

void ShaderGenerator::AddBlockRange(const IDynamicArray<LexBlock>& srcBlocks, int start, const int end, std::string& srcOut, int& line)
{
	while (start <= end)
	{
		const LexBlock& block = srcBlocks[start];

		if ((start + 1) <= end && block.GetHasFlags(LexBlockTypes::LineDirectiveName))
		{
			FXSYNTAX_ASSERT_MSG(srcBlocks[start + 1].GetHasFlags(LexBlockTypes::LineDirectiveBody),
				"Expected line directive body after #line");

			const LexBlock& body = srcBlocks[start + 1];
			line = body.startLine + 1;

			if (!srcOut.empty() && srcOut.back() != '\n')
				srcOut.push_back('\n');

			srcOut.append(block.src);
			srcOut.append(body.src);
			start += 2;
		}
		else
		{ 
			if (!block.GetHasFlags(LexBlockTypes::EndContainer))
			{
				if ((block.startLine - line) > 3)
				{
					AppendLineDirective(block.startLine, srcOut);
					line = block.startLine;
				}
				else while (line < block.startLine)
				{
					srcOut.push_back('\n');
					line++;
				}
			}

			if (block.GetHasFlags(LexBlockTypes::StartContainer))
				srcOut.push_back(block.src.GetFront());
			else if (block.GetHasFlags(LexBlockTypes::EndContainer))
				srcOut.push_back(block.src.GetBack());
			else if (block.GetHasFlags(LexBlockTypes::DirectiveName))
			{
				if (!srcOut.empty() && srcOut.back() != '\n')
					srcOut.push_back('\n');

				srcOut.append(block.src);
				line += block.lineCount;
			}
			else
			{
				srcOut.append(block.src);
				line += block.lineCount;
			}

			start++;
		}
	}
}
