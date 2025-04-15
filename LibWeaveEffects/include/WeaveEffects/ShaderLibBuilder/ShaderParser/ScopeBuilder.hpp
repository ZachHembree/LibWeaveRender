#pragma once
#include <forward_list>
#include <list>
#include <unordered_map>
#include "SymbolEnums.hpp"
#include "ShaderTypeInfo.hpp"

namespace Weave::Effects
{
    struct LexBlock;
    struct TokenDef;
    struct TokenNode;
    struct SymbolData;
    struct FunctionData;
    struct ScopeData;
    struct AttributeData;

    using IDList = std::list<int>;
    using NameIndexMap = std::unordered_map<string_view, int>;
    using FuncOverloadMap = std::unordered_map<string_view, IDList>;

    /// <summary>
    /// Stores a collection of symbols and tokens owned by scoping objects
    /// </summary>
    class ScopeBuilder
    {
    public:
        MAKE_MOVE_ONLY(ScopeBuilder)

        ScopeBuilder();

        /// <summary>
        /// Returns the number of tokens in the builder
        /// </summary>
        size_t GetTokenCount() const;

        /// <summary>
        /// Returns the number of scopes in the builder
        /// </summary>
        size_t GetScopeCount() const;

        /// <summary>
        /// Returns the number of symbols in the builder
        /// </summary>
        size_t GetSymbolCount() const;

        /// <summary>
        /// Returns the token at the given index
        /// </summary>
        const TokenNode& GetTokenNode(int tokenID) const;

        const Span<const TokenNode> GetTokenChildren(const int tokenID) const;

        /// <summary>
        /// Returns reference to raw symbol data at the given index
        /// </summary>
        const SymbolData& GetSymbolData(int symbolID) const;

        /// <summary>
        /// Returns reference to raw scope data at the given index
        /// </summary>
        const ScopeData& GetScopeData(int scopeID) const;

        /// <summary>
        /// Returns reference to raw function data at the given index
        /// </summary>
        const FunctionData& GetFuncData(int funcID) const;

        /// <summary>
        /// Returns reference to raw type data at the given index
        /// </summary>
        const ShaderTypeInfo& GetTypeData(int typeID) const;

        /// <summary>
        /// Returns reference to raw attribute data at the given index
        /// </summary>
        const AttributeData& GetAttribData(int attribID) const;

        int GetScopeChild(const int scopeID, const int index) const;

        int GetScopeChild(const int scopeID, string_view ident) const;

        size_t GetScopeChildCount(const int scopeID) const;

        /// <summary>
        /// Returns the token at the given index
        /// </summary>
        TokenNode& GetTokenNode(int tokenID);

        Span<TokenNode> GetTokenChildren(const int tokenID);

        /// <summary>
        /// Returns reference to raw symbol data at the given index
        /// </summary>
        SymbolData& GetSymbolData(int symbolID);

        /// <summary>
        /// Returns reference to raw scope data at the given index
        /// </summary>
        ScopeData& GetScopeData(int scopeID);

        /// <summary>
        /// Returns reference to raw function data at the given index
        /// </summary>
        FunctionData& GetFuncData(int funcID);

        /// <summary>
        /// Returns reference to raw type data at the given index
        /// </summary>
        ShaderTypeInfo& GetUserTypeData(int userTypeID);

        /// <summary>
        /// Returns reference to raw attribute data at the given index
        /// </summary>
        AttributeData& GetAttribData(int attribID);

        /// <summary>
        /// Moves a dynamically generated string into the builder and returns a view to it
        /// </summary>
        string_view AddGeneratedText(string&& str);

        /// <summary>
        /// Copies a dynamically generated string into the builder and returns a view to it
        /// </summary>
        string_view AddGeneratedText(string_view str);

        /// <summary>
        /// Attempts to retrive a symbol by the given name with the given top scope
        /// </summary>
        bool TryGetSymbol(string_view name, int& symbolID, int top = -1) const;

        const ShaderTypeInfo* TryGetType(string_view name, int top = -1) const;

        /// <summary>
        /// Attempts to retrieve a list of function overloads with the given identifier, starting
        /// with the given top scopeID
        /// </summary>
        const IDList* TryGetFuncOverloads(string_view ident, int top = -1) const;

        /// <summary>
        /// Attempts to retrieve flags for the given token, including symbol identifiers
        /// </summary>
        bool TryGetTokenFlags(TokenDef& token, int top = -1) const;

        /// <summary>
        /// Returns true if a symbol with the given name exists within the context defined
        /// by the given top scope.
        /// </summary>
        bool GetHasSymbol(string_view name, int top = -1) const;

        /// <summary>
        /// Constructs a new token in place with the given value and returns the ID for it
        /// </summary>
        int GetNewToken(string_view value, TokenTypes flags, const int depth, const int blockID);

        int GetNewTypeSpecifier(const int tokenID);

        /// <summary>
        /// Returns the index to a new token subtype initialized with the given definition, semantic or attribute
        /// </summary>
        int GetNewAttribute(const int tokenID);

        /// <summary>
        /// Returns the index to a new generic symbol initialized with the given definition
        /// </summary>
        int GetNewSymbol(const int tokenID, SymbolTypes flags);

        /// <summary>
        /// Returns the index to a new function symbol initialized with the given definition
        /// </summary>
        int GetNewFunc(const int tokenID, SymbolTypes flags);

        int GetNewTypeAlias(const int tokenID, SymbolTypes flags);

        int GetNewUserType(const int tokenID, SymbolTypes);

        /// <summary>
        /// Attempts to push a new symbol to the top-most scope. Must be unique.
        /// </summary>
        void PushSymbol(const int symbolID, const bool isDeferred = false);

        /// <summary>
        /// Pushes a new scope with the given depth, associated with the given LexBlock ID.
        /// Automatically associated with symbol declaring scope, if one exists.
        /// </summary>
        void PushScope(const int firstBlock, const int depth);

        /// <summary>
        /// Terminates the top most unterminated scope at the given block index
        /// </summary>
        void PopScope(const int lastBlock);

        void Clear();

    private:
        /// <summary>
        /// Lookup tables for each symbol in each scope, parallel with scopes vector
        /// </summary>
        UniqueVector<ScopeData> scopes;
        UniqueVector<NameIndexMap> scopeSymbolMaps;
        UniqueVector<FuncOverloadMap> funcOverloadMaps;
        UniqueVector<UniqueVector<int>> scopeSymbolLists;

        UniqueVector<TokenNode> tokens;
        UniqueVector<SymbolData> symbols;

        UniqueVector<const ShaderTypeInfo*> types;
        UniqueVector<ShaderTypeInfo> userTypes;
        UniqueVector<FunctionData> functions;
        UniqueVector<AttributeData> attributes;

        UniqueVector<int> deferredSymbolBuf;
        std::forward_list<string> generatedText;

        int topScope;
        int pendingScopeSymbol;

        void Init();

        void AddScope(const int symbolID, const int blockStart = 0, const int blockCount = 0);

        void AddFuncToOverloadTable(const string_view name, const int symbolID);
    };
}