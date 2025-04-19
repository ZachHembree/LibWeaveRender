#pragma once
#include "SymbolEnums.hpp"
#include "BlockAnalyzer.hpp"

namespace Weave::Effects
{
	struct LexBlock;

	enum class MatchQualifiers : byte
	{
		/// <summary>
		/// Default. Matches one object of a given type
		/// </summary>
		None = 0x0,

		/// <summary>
		/// Can match 0 or 1 objects of a given type
		/// </summary>
		Optional = 1 << 0,

		/// <summary>
		/// Matches one or more items, not optional
		/// </summary>
		OneOrMore = 1 << 1,

		/// <summary>
		/// Indicates that any one MatchNode, MatchPattern or MatchQualifier in the corresponding
		/// container object can satisfy a matching condition. Does not apply to CapturePatterns or
		/// individual qualifiers.
		/// </summary>
		Alternation = 1 << 2,

		/// <summary>
		/// Can match any number of objects of a given type
		/// </summary>
		ZeroOrMore = OneOrMore | Optional
	};

	/// <summary>
	/// Qualified LexBlockTypes used in a BlockPattern that may repeat multiple 
	/// times, once, or may be entirely optional, depending on the MatchQualifier.
	/// </summary>
	struct BlockQualifier
	{
		/// <summary>
		/// Matches one block of any type
		/// </summary>
		static const LexBlockTypes Wild;

		LexBlockTypes type;

		MatchQualifiers qualifier;

		BlockQualifier();
		BlockQualifier(LexBlockTypes type);
		BlockQualifier(LexBlockTypes type, MatchQualifiers qualifier);

		bool GetHasFlags(LexBlockTypes flags) const { return (type & flags) == flags; }

		bool GetHasFlags(MatchQualifiers flags) const { return (qualifier & flags) == flags; }
	};

	/// <summary>
	/// Qualified token type flag used in a CapturePattern that may repeat multiple 
	/// times, once, or may be entirely optional, depending on the MatchQualifier.
	/// </summary>
	struct TokenQualifier
	{
		TokenTypes type;

		MatchQualifiers qualifier;

		TokenQualifier();

		TokenQualifier(TokenTypes type);
		TokenQualifier(TokenTypes type, MatchQualifiers qualifier);

		bool GetHasFlags(TokenTypes flags) const { return (type & flags) == flags; }

		bool GetHasFlags(MatchQualifiers flags) const { return (qualifier & flags) == flags; }
	};

	/// <summary>
	/// A sequence of SymbolQualifiers used to capture specific tokens and symbols
	/// </summary>
	class CapturePattern : public DynamicArray<TokenQualifier>
	{
	public:
		/// <summary>
		/// Type of token captured by the pattern, assumes an identifier if a symbol
		/// type is specified
		/// </summary>
		TokenTypes tokenType;

		/// <summary>
		/// Type of symbol identified by the pattern
		/// </summary>
		SymbolTypes symbolType;

		CapturePattern();
		CapturePattern(TokenTypes ident, SymbolTypes symbol = SymbolTypes::Unknown);

		CapturePattern(const std::initializer_list<TokenQualifier>& predicates, TokenTypes type);
		CapturePattern(const std::initializer_list<TokenQualifier>& predicates, SymbolTypes type);
		CapturePattern(const std::initializer_list<TokenQualifier>& predicates, TokenTypes ident, SymbolTypes symbol);

		bool GetHasFlags(SymbolTypes flags) const { return (symbolType & flags) == flags; }

		bool GetHasFlags(TokenTypes flags) const { return (tokenType & flags) == flags; }

		bool GetIsValid() const;
	};

	class MatchPattern : public DynamicArray<BlockQualifier>
	{
	public:
		MatchPattern();

		MatchPattern(
			const std::initializer_list<BlockQualifier>& blocks,
			const std::initializer_list<CapturePattern>& capPatterns,
			MatchQualifiers qualifers = MatchQualifiers::None
		);

		MatchPattern(
			const std::initializer_list<BlockQualifier>& blocks, 
			const CapturePattern& capPattern = {},
			MatchQualifiers qualifers = MatchQualifiers::None
		);

		MatchPattern(
			BlockQualifier block, 
			const CapturePattern& capPattern = {},
			MatchQualifiers qualifers = MatchQualifiers::None
		);

		MatchPattern(
			BlockQualifier block,
			const std::initializer_list<CapturePattern>& capPatterns,
			MatchQualifiers qualifers = MatchQualifiers::None
		);

		MatchPattern(
			const std::initializer_list<BlockQualifier>& blocks, 
			CapturePattern&& capPattern,
			MatchQualifiers qualifers = MatchQualifiers::None
		) noexcept;

		MatchPattern(
			BlockQualifier block, 
			CapturePattern&& capPattern,
			MatchQualifiers qualifers = MatchQualifiers::None
		) noexcept;

		~MatchPattern();

		MatchPattern(const MatchPattern& other);
		MatchPattern(MatchPattern&& other) noexcept;
		MatchPattern& operator=(const MatchPattern& other);
		MatchPattern& operator=(MatchPattern&& other) noexcept;

		bool GetHasFlags(MatchQualifiers flags) const;

		bool GetHasCapPattern() const;

		const IDynamicArray<CapturePattern>& GetCapPatterns() const;

	private:
		DynamicArray<CapturePattern>* pCapPatterns;
		MatchQualifiers qualifiers;
	};

	/// <summary>
	/// A LexBlock paired a set of CapturePatterns needed to capture tokens and symbols it contains
	/// </summary>
	struct CaptureBlock
	{
		int blockID;
		int mainCap;
		const IDynamicArray<CapturePattern>* pPatterns;

		CaptureBlock();
		CaptureBlock(const int blockIndex, const IDynamicArray<CapturePattern>& capPatterns, const int mainCap = -1);
	};

	/// <summary>
	/// Node for creating a tree of CapturePatterns
	/// </summary>
	struct CaptureGroup
	{
		/// <summary>
		/// Index of the first child group
		/// </summary>
		int subGroupStart;

		/// <summary>
		/// Number of direct child capture nodes
		/// </summary>
		int subGroupCount;

		/// <summary>
		/// Index of the first CaptureBlock directly owned by the node
		/// </summary>
		int capStart;

		/// <summary>
		/// Number of CaptureBlock directly owned by the node
		/// </summary>
		int capCount;

		/// <summary>
		/// Beginning of LexBlock range captured
		/// </summary>
		int srcStart;

		/// <summary>
		/// Length of LexBlock range captured
		/// </summary>
		int srcCount;

		/// <summary>
		/// Index of the CaptureBlock that owns the group
		/// </summary>
		int parentCap;

		CaptureGroup() :
			subGroupStart(0),
			subGroupCount(0),
			capStart(0),
			capCount(0),
			srcStart(0),
			srcCount(0),
			parentCap(-1)
		{ }
	};

	/// <summary>
	/// A group of BlocksPattern sequences, with CapturePattern in parallel, used for arranging
	/// matching patterns into trees
	/// </summary>
	class MatchNode
	{
	public:
		MatchNode();

		~MatchNode();

		MatchNode(const MatchNode& other);

		MatchNode(MatchNode&& other) noexcept;

		MatchNode(
			const std::initializer_list<MatchPattern>& blockPatterns,
			TokenTypes parentToken,
			MatchQualifiers qualifier = MatchQualifiers::None,
			bool isEntrypoint = false
		);

		MatchNode(
			const std::initializer_list<MatchPattern>& blockPatterns,
			MatchQualifiers qualifier = MatchQualifiers::None,
			bool isEntrypoint = false
		);

		MatchNode(
			MatchPattern&& blockPattern,
			MatchQualifiers qualifier = MatchQualifiers::None,
			bool isEntrypoint = false
		)  noexcept;

		MatchNode(
			MatchPattern&& blockPattern,
			TokenTypes parentToken,
			MatchQualifiers qualifier = MatchQualifiers::None,
			bool isEntrypoint = false
		)  noexcept;

		MatchNode(
			const std::initializer_list<MatchNode>& subpatterns,
			TokenTypes parentToken,
			MatchQualifiers qualifier = MatchQualifiers::None,
			bool isEntrypoint = false
		);

		MatchNode(
			const std::initializer_list<MatchNode>& subpatterns,
			MatchQualifiers qualifier = MatchQualifiers::None,
			bool isEntrypoint = false
		);

		MatchNode& operator=(const MatchNode& other);

		MatchNode& operator=(MatchNode&& other) noexcept;

		/// <summary>
		/// Returns token type associated with child captures, usually an identifier
		/// </summary>
		TokenTypes GetParentToken() const;

		/// <summary>
		/// Returns true if the node is used for forward pattern matching. False if backward.
		/// </summary>
		bool GetIsForward() const;

		/// <summary>
		/// Returns true if the node has the given match qualifiers
		/// </summary>
		bool GetHasFlags(MatchQualifiers flags) const;

		bool GetHasMatchPatterns() const;

		const IDynamicArray<MatchPattern>& GetMatchPatterns() const;

		bool GetHasMatchNodes() const;

		const IDynamicArray<MatchNode>& GetMatchNodes() const;

	private:
		/// <summary>
		/// Blocks pattern sequences used for matching a range of source to be selectively
		/// parsed using CapturePattern
		/// </summary>
		DynamicArray<MatchPattern>* pMatchPatterns;

		/// <summary>
		/// Nested MatchNodes searched in depth-first order, after the parent node
		/// </summary>
		DynamicArray<MatchNode>* pNodes;

		/// <summary>
		/// Token type associated with child captures, usually an identifier
		/// </summary>
		TokenTypes parentToken;

		/// <summary>
		/// Indicates whether a pattern is repeatable, optional, etc.
		/// </summary>
		MatchQualifiers matchQualifiers;

		bool isEntrypoint;

		/// <summary>
		/// True if the pattern is used for forward matching
		/// </summary>
		bool isForward;

		void PresortPattern();

		void ReversePattern();
	};

	/// <summary>
	/// Groups a set of match nodes based on the type of keyword in the first block
	/// </summary>
	struct MatchNodeGroup
	{
		static const UniqueArray<MatchNodeGroup> MatchNodeGroups;

		/// <summary>
		/// Types of tokens that match roots in this group can start with
		/// </summary>
		DynamicArray<TokenTypes> symbolTypes;

		/// <summary>
		/// Root match nodes for all matches starting with one of the keywords in
		/// the symbolTypes array for this group
		/// </summary>
		DynamicArray<MatchNode> rootNodes;

		MatchNodeGroup() = default;

		MatchNodeGroup(
			const std::initializer_list<TokenTypes>& startingTypes,
			const std::initializer_list<MatchNode>& patterns
		) noexcept;

		bool GetHasFlags(TokenTypes flags) const;
	};
}