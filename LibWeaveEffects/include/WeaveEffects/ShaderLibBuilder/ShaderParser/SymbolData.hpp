#pragma once
#include "SymbolEnums.hpp"
#include "ShaderTypeInfo.hpp"

namespace Weave::Effects
{
    using std::string_view;
    struct CaptureBlock;

    struct TokenDef
    {
        TextBlock name;
        ShaderTypes typeFlags;
        TokenTypes tokenFlags;

        TokenDef() :
            name(),
            tokenFlags(TokenTypes::Unknown),
            typeFlags(ShaderTypes::Void)
        { }

        TokenDef(const TextBlock& name,
            TokenTypes tokenFlags = TokenTypes::Unknown,
            ShaderTypes typeFlags = ShaderTypes::Void
        ) :
            name(name),
            tokenFlags(tokenFlags),
            typeFlags(typeFlags)
        { }

        bool GetHasFlags(TokenTypes other) const { return (tokenFlags & other) == other; }
    };

    struct TokenNodeDef
    {
        TokenDef identifier;
        SymbolTypes symbolFlags;
        int blockStart;
        int blockCount;
        int childStart;
        int childCount;

        TokenNodeDef() :
            symbolFlags(SymbolTypes::Unknown),
            blockStart(0),
            blockCount(0),
            childStart(0),
            childCount(0)
        { }

        TokenNodeDef(
            const TokenDef& identifier, 
            SymbolTypes symbolFlags = SymbolTypes::Unknown, 
            int blockID = 0,
            int childStart = 0, 
            int childCount = 0
        ) :
            identifier(identifier),
            symbolFlags(symbolFlags),
            blockStart(blockID),
            childStart(childStart),
            childCount(childCount),
            blockCount(0)
        { }

        bool GetHasFlags(TokenTypes other) const { return identifier.GetHasFlags(other); }

        bool GetHasFlags(SymbolTypes other) const { return (symbolFlags & other) == other; }
    };

    /// <summary>
    /// Node for storing semantically related tokens in a heirarchy and relating them to text in the original
    /// source
    /// </summary>
    struct TokenNode
    {
        /// <summary>
        /// Original text value of the token, identifier text, type name, operator, etc.
        /// </summary>
        string_view value;

        /// <summary>
        /// Flags indicating token type
        /// </summary>
        TokenTypes type;

        /// <summary>
        /// Scope depth of the token
        /// </summary>
        int depth;

        /// <summary>
        /// Index of the first block comprising the token
        /// </summary>
        int blockStart;

        /// <summary>
        /// Number of blocks owned by the token
        /// </summary>
        int blockCount;

        /// <summary>
        /// Index of the first child token
        /// </summary>
        int childStart;

        /// <summary>
        /// Number of child tokens
        /// </summary>
        int childCount;

        /// <summary>
        /// Index of the subtype accessor
        /// </summary>
        int subtypeID;

        /// <summary>
        /// Index of associated owning symbol, if one exists
        /// </summary>
        int symbolID;

        /// <summary>
        /// Returns true if the token has the given flags set
        /// </summary>
        bool GetHasFlags(TokenTypes flag) const { return ((type & flag) == flag); }
    };

    struct SymbolData
    {
        /// <summary>
        /// Token ID for the symbol's identifier
        /// </summary>
        int identID;

        /// <summary>
        /// Scope ID owned by the symbol, if applicable
        /// </summary>
        int scopeID;

        /// <summary>
        /// Flags indicating symbol type
        /// </summary>
        SymbolTypes type;

        /// <summary>
        /// Returns true if the symbol has the given flags set
        /// </summary>
        bool GetHasFlags(SymbolTypes flag) const { return ((type & flag) == flag); }
    };

    struct ScopeData
    {
        /// <summary>
        /// Index of the symbol declaring the given scope.
        /// </summary>
        int symbolID;

        /// <summary>
        /// Index of the scope containing this scope
        /// </summary>
        int parentScope;

        /// <summary>
        /// Index of the first block in the body of the scope, the opening '{'
        /// bracket.
        /// </summary>
        int blockStart;

        /// <summary>
        /// Length of the scope body, including closing braces
        /// </summary>
        int blockCount;

        /// <summary>
        /// Returns the index of the first block in the body, after '{'
        /// </summary>
        int GetBodyStart() const { return blockStart + 1; }

        /// <summary>
        /// Returns the index of the last block in the scope, the closing '}'
        /// bracket.
        /// </summary>
        int GetLast() const { return blockStart + blockCount - 1; }
    };

    struct FunctionData
    {
        string_view signature;
    };

    struct AttributeData
    {
        int semanticIndex;
    };
}