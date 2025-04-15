#include "pch.hpp"
#include "WeaveEffects/ShaderLibBuilder/ShaderParser/SymbolPatterns.hpp"

using namespace Weave;
using namespace Weave::Effects;

const LexBlockTypes BlockQualifier::Wild = LexBlockTypes::Unknown;

BlockQualifier::BlockQualifier() :
	type(LexBlockTypes::Unknown),
	qualifier(MatchQualifiers::None)
{}

BlockQualifier::BlockQualifier(LexBlockTypes type) :
	type(type),
	qualifier(MatchQualifiers::None)
{ }

BlockQualifier::BlockQualifier(LexBlockTypes type, MatchQualifiers qualifier) :
	type(type),
	qualifier(qualifier)
{ }

TokenQualifier::TokenQualifier() :
	type(TokenTypes::Unknown),
	qualifier(MatchQualifiers::None)
{ }

TokenQualifier::TokenQualifier(TokenTypes type) :
	type(type),
	qualifier(MatchQualifiers::None)
{ }

TokenQualifier::TokenQualifier(TokenTypes type, MatchQualifiers qualifier) :
	type(type),
	qualifier(qualifier)
{ }

MatchPattern::MatchPattern() :
	pCapPatterns(nullptr),
	qualifiers(MatchQualifiers::None)
{ }

MatchPattern::MatchPattern(
	const std::initializer_list<BlockQualifier>& blocks,
	const std::initializer_list<CapturePattern>& capPatterns,
	MatchQualifiers qualifers
) :
	DynamicArray<BlockQualifier>(blocks),
	pCapPatterns(nullptr),
	qualifiers(qualifers)
{
	int capCount = 0;

	for (const CapturePattern& cap : capPatterns)
	{
		if (cap.GetIsValid())
			capCount++;
	}

	if (capCount > 0)
	{
		int i = 0;
		pCapPatterns = new DynamicArray<CapturePattern>(capCount);

		for (const CapturePattern& cap : capPatterns)
		{
			if (cap.GetIsValid())
			{
				(*pCapPatterns)[i] = cap;
				i++;
			}
		}
	}
}

MatchPattern::MatchPattern(
	const std::initializer_list<BlockQualifier>& blocks, 
	const CapturePattern& capPattern,
	MatchQualifiers qualifers
) :
	MatchPattern(blocks, {capPattern}, qualifers)
{ }

MatchPattern::MatchPattern(
	BlockQualifier block, 
	const CapturePattern& capPattern,
	MatchQualifiers qualifers
) :
	MatchPattern({block}, {capPattern}, qualifers)
{ }

MatchPattern::MatchPattern(
	BlockQualifier block,
	const std::initializer_list<CapturePattern>& capPatterns,
	MatchQualifiers qualifers
) :
	MatchPattern({block}, capPatterns, qualifers)
{ }

MatchPattern::MatchPattern(
	const std::initializer_list<BlockQualifier>& blocks, 
	CapturePattern&& capPattern,
	MatchQualifiers qualifers
) noexcept :
	MatchPattern(blocks, {std::move(capPattern)}, qualifers)
{ }

MatchPattern::MatchPattern(
	BlockQualifier block, 
	CapturePattern&& capPattern,
	MatchQualifiers qualifers
) noexcept :
	MatchPattern({block}, {std::move(capPattern)}, qualifers)
{ }

MatchPattern::~MatchPattern()
{
	if (pCapPatterns != nullptr)
		delete pCapPatterns;
}

MatchPattern::MatchPattern(const MatchPattern& other) :
	DynamicArray<BlockQualifier>(other),
	pCapPatterns(other.pCapPatterns != nullptr ? (new DynamicArray<CapturePattern>(*other.pCapPatterns)) : nullptr),
	qualifiers(other.qualifiers)
{ }

MatchPattern::MatchPattern(MatchPattern&& other) noexcept:
	DynamicArray<BlockQualifier>(std::move(other)),
	pCapPatterns(other.pCapPatterns != nullptr ? ( new DynamicArray<CapturePattern>(std::move(*other.pCapPatterns)) ) : nullptr),
	qualifiers(other.qualifiers)
{ 
	other.pCapPatterns = nullptr;
}

MatchPattern& MatchPattern::operator=(const MatchPattern& other)
{
	DynamicArray<BlockQualifier>::operator=(other);
	pCapPatterns = other.pCapPatterns != nullptr ? (new DynamicArray<CapturePattern>(*other.pCapPatterns)) : nullptr;
	qualifiers = other.qualifiers;
	return *this;
}

MatchPattern& MatchPattern::operator=(MatchPattern&& other) noexcept
{
	DynamicArray<BlockQualifier>::operator=(std::move(other));
	pCapPatterns = other.pCapPatterns != nullptr ? ( new DynamicArray<CapturePattern>(std::move(*other.pCapPatterns)) ) : nullptr;
	qualifiers = other.qualifiers;
	return *this;
}

bool MatchPattern::GetHasFlags(MatchQualifiers flags) const { return (flags & qualifiers) == flags; }

bool MatchPattern::GetHasCapPattern() const { return pCapPatterns != nullptr && pCapPatterns->GetLength() > 0; }

const IDynamicArray<CapturePattern>& MatchPattern::GetCapPatterns() const { return *pCapPatterns; }

CapturePattern::CapturePattern() :
	tokenType(TokenTypes::Unknown),
	symbolType(SymbolTypes::Unknown)
{ }

CapturePattern::CapturePattern(TokenTypes token, SymbolTypes symbol) :
	tokenType(token),
	symbolType(symbol)
{ }

CapturePattern::CapturePattern(const std::initializer_list<TokenQualifier>& predicates, TokenTypes type) :
	tokenType(type),
	symbolType(SymbolTypes::Unknown),
	DynamicArray<TokenQualifier>(predicates)
{ }

CapturePattern::CapturePattern(const std::initializer_list<TokenQualifier>& predicates, SymbolTypes type) :
	tokenType(TokenTypes::Identifier),
	symbolType(type),
	DynamicArray<TokenQualifier>(predicates)
{ }

CapturePattern::CapturePattern(const std::initializer_list<TokenQualifier>& predicates, TokenTypes ident, SymbolTypes symbol) :
	tokenType(ident),
	symbolType(symbol),
	DynamicArray<TokenQualifier>(predicates)
{ }

bool CapturePattern::GetIsValid() const { return GetLength() > 0 || tokenType != TokenTypes::Unknown; }

CaptureBlock::CaptureBlock() : blockID(0), mainCap(-1), pPatterns(nullptr) { }

CaptureBlock::CaptureBlock(const int blockIndex, const IDynamicArray<CapturePattern>& capPatterns, const int mainCap) :
	blockID(blockIndex),
	pPatterns(&capPatterns),
	mainCap(mainCap)
{ }

MatchNode::MatchNode() :
	pMatchPatterns(nullptr),
	pNodes(nullptr),
	parentToken(TokenTypes::Unknown),
	matchQualifiers(MatchQualifiers::None),
	isEntrypoint(false),
	isForward(true)
{ }

MatchNode::~MatchNode()
{
	if (pMatchPatterns != nullptr)
	{
		delete pMatchPatterns;
	}

	if (pNodes != nullptr)
	{
		delete pNodes;
	}
}

MatchNode::MatchNode(
	const std::initializer_list<MatchPattern>& blockPatterns,
	TokenTypes parentToken,
	MatchQualifiers qualifier,
	bool isEntrypoint
) :
	pMatchPatterns(new DynamicArray<MatchPattern>(blockPatterns)),
	pNodes(nullptr),
	parentToken(parentToken),
	matchQualifiers(qualifier),
	isEntrypoint(isEntrypoint),
	isForward(true)
{ }

MatchNode::MatchNode(
	const std::initializer_list<MatchNode>& subpatterns,
	TokenTypes parentToken,
	MatchQualifiers qualifier,
	bool isEntrypoint
)  :
	pMatchPatterns(nullptr),
	pNodes(new DynamicArray<MatchNode>(subpatterns)),
	parentToken(parentToken),
	matchQualifiers(qualifier),
	isEntrypoint(isEntrypoint),
	isForward(true)
{
	PresortPattern();
}

MatchNode::MatchNode(
	const std::initializer_list<MatchPattern>& blockPatterns,
	MatchQualifiers qualifier,
	bool isEntrypoint
) :
	pMatchPatterns(new DynamicArray<MatchPattern>(blockPatterns)),
	pNodes(nullptr),
	matchQualifiers(qualifier),
	parentToken(TokenTypes::Unknown),
	isEntrypoint(isEntrypoint),
	isForward(true)
{ }

MatchNode::MatchNode(
	MatchPattern&& blockPattern,
	TokenTypes parentToken,
	MatchQualifiers qualifier,
	bool isEntrypoint
)  noexcept :
	MatchNode({ std::move(blockPattern) }, parentToken, qualifier, isEntrypoint)
{ }

MatchNode::MatchNode(
	MatchPattern&& blockPattern,
	MatchQualifiers qualifier,
	bool isEntrypoint
)  noexcept :
	MatchNode({ blockPattern }, {}, qualifier, isEntrypoint)
{ }

MatchNode::MatchNode(
	const std::initializer_list<MatchNode>& subpatterns,
	MatchQualifiers qualifier,
	bool isEntrypoint
) :
	MatchNode(subpatterns, TokenTypes::Unknown, qualifier, isEntrypoint)
{ }

template<typename T>
static DynamicArray<T>* TryGetArrCopy(const DynamicArray<T>* pArr)
{
	return pArr != nullptr ? new DynamicArray<T>(*pArr) : nullptr;
}

MatchNode::MatchNode(const MatchNode& other) :
	pMatchPatterns(TryGetArrCopy(other.pMatchPatterns)),
	pNodes(TryGetArrCopy(other.pNodes)),
	parentToken(other.parentToken),
	matchQualifiers(other.matchQualifiers),
	isEntrypoint(other.isEntrypoint),
	isForward(other.isForward)
{ }

MatchNode::MatchNode(MatchNode&& other) noexcept
{
	memcpy(this, &other, sizeof(MatchNode));
	memset(&other, 0, sizeof(MatchNode));
}

MatchNode& MatchNode::operator=(const MatchNode& other)
{
	pMatchPatterns = TryGetArrCopy(other.pMatchPatterns);
	pNodes = TryGetArrCopy(other.pNodes);
	parentToken = other.parentToken;
	matchQualifiers = other.matchQualifiers;
	isEntrypoint = other.isEntrypoint;
	isForward = other.isForward;

	return *this;
}

MatchNode& MatchNode::operator=(MatchNode&& other) noexcept
{
	memcpy(this, &other, sizeof(MatchNode));
	memset(&other, 0, sizeof(MatchNode));
	return *this;
}

TokenTypes MatchNode::GetParentToken() const { return parentToken; }

bool MatchNode::GetIsForward() const { return isForward; }

bool MatchNode::GetHasFlags(MatchQualifiers flags) const
{
	return (matchQualifiers & flags) == flags;
}

bool MatchNode::GetHasMatchPatterns() const { return pMatchPatterns != nullptr; }

const IDynamicArray<MatchPattern>& MatchNode::GetMatchPatterns() const
{
	FX_ASSERT_MSG(pMatchPatterns != nullptr, "Attempted to return null match patterns by reference");
	return *pMatchPatterns;
}

bool MatchNode::GetHasMatchNodes() const { return pNodes != nullptr; }

const IDynamicArray<MatchNode>& MatchNode::GetMatchNodes() const
{
	FX_ASSERT_MSG(pNodes != nullptr, "Attempted to return null match nodes by reference");
	return *pNodes;
}

void MatchNode::PresortPattern()
{
	DynamicArray<MatchNode>& nodes = *pNodes;

	// Get entrypoint
	int ep = 0;

	for (int i = 0; i < nodes.GetLength(); i++)
	{
		if ((nodes[i].isEntrypoint))
		{
			ep = i;
			break;
		}
	}

	// Reverse backward matching patterns
	if (ep > 0)
	{
		FX_ASSERT_MSG(!isEntrypoint, "Multiple entrypoints cannot exist within the same matching pattern");

		for (int i = 0; i < ep; i++)
			nodes[i].ReversePattern();

		std::reverse(nodes.begin(), nodes.begin() + ep);
	}
}

void MatchNode::ReversePattern()
{
	FX_ASSERT_MSG(!isEntrypoint, "Multiple entrypoints cannot exist within the same matching pattern");
	FX_ASSERT_MSG(isForward, "Attempted to reverse matching pattern twice");
	isForward = false;

	if (pMatchPatterns != nullptr)
		std::reverse(pMatchPatterns->begin(), pMatchPatterns->end());

	if (pNodes != nullptr)
	{
		std::reverse(pNodes->begin(), pNodes->end());

		for (MatchNode& node : *pNodes)
			node.ReversePattern();
	}
}

MatchNodeGroup::MatchNodeGroup(
	const std::initializer_list<TokenTypes>& startingTypes, 
	const std::initializer_list<MatchNode>& patterns
) noexcept :
	symbolTypes(startingTypes),
	rootNodes(patterns)
{ }

bool MatchNodeGroup::GetHasFlags(TokenTypes flags) const
{
	for (const TokenTypes& startFlags : symbolTypes)
	{
		if ((startFlags & flags) == startFlags)
			return true;
	}

	return false;
}
