#include "pch.hpp"
#include "ReplicaEffects/ShaderLibBuilder/ShaderParser/SymbolData.hpp"
#include "ReplicaEffects/ShaderLibBuilder/ShaderParser/SymbolPatterns.hpp"
#include "ReplicaEffects/ShaderLibBuilder/ShaderParser/ScopeBuilder.hpp"

using namespace Replica;
using namespace Replica::Effects;

ScopeBuilder::ScopeBuilder() :
    scopes(50),
    topScope(0)
{
    Init();
}

void ScopeBuilder::Init()
{
    topScope = -1;
    pendingScopeSymbol = -1;

    symbols.EmplaceBack(-1, 0, SymbolTypes::AnonScope);
    AddScope(0);
}

void ScopeBuilder::AddScope(const int symbolID, const int blockStart, const int blockCount)
{
    // Create new scope and parent it to the last unterminated scope
    const int parentID = topScope;
    topScope = (int)scopes.GetLength();

    symbols[symbolID].scopeID = topScope;
    ScopeData& scope = scopes.EmplaceBack();
    scope.symbolID = symbolID;
    scope.parentScope = parentID;
    scope.blockStart = blockStart;
    scope.blockCount = blockCount;

    scopeSymbolMaps.EmplaceBack();
    funcOverloadMaps.EmplaceBack();
    scopeSymbolLists.EmplaceBack();
}

void ScopeBuilder::AddFuncToOverloadTable(const string_view name, const int symbolID)
{
    FuncOverloadMap& map = funcOverloadMaps[topScope];

    if (!map.contains(name))
        map.emplace(name, IDList());

    IDList& funcList = map.find(name)->second;
    funcList.push_front(symbolID);
}

void ScopeBuilder::Clear()
{
    scopes.Clear();
    scopeSymbolMaps.Clear();
    funcOverloadMaps.Clear();
    scopeSymbolLists.Clear();

    tokens.Clear();
    symbols.Clear();

    types.Clear();
    userTypes.Clear();
    functions.Clear();
    attributes.Clear();

    deferredSymbolBuf.Clear();
    generatedText.clear();

    Init();
}

size_t ScopeBuilder::GetTokenCount() const { return tokens.GetLength(); }

size_t ScopeBuilder::GetScopeCount() const { return scopes.GetLength(); }

size_t ScopeBuilder::GetSymbolCount() const { return symbols.GetLength(); }

TokenNode& ScopeBuilder::GetTokenNode(int index) { return tokens[index]; }

Span<TokenNode> ScopeBuilder::GetTokenChildren(const int tokenID)
{
    TokenNode& ident = tokens[tokenID];
    return Span<TokenNode>(&tokens[ident.childStart], ident.childCount);
}

SymbolData& ScopeBuilder::GetSymbolData(int index) { return symbols[index]; }

ScopeData& ScopeBuilder::GetScopeData(int index) { return scopes[index]; }

FunctionData& ScopeBuilder::GetFuncData(int index) { return functions[index]; }

ShaderTypeInfo& ScopeBuilder::GetUserTypeData(int index) { return userTypes[index]; }

AttributeData& ScopeBuilder::GetAttribData(int index) { return attributes[index]; }

const TokenNode& ScopeBuilder::GetTokenNode(int index) const { return tokens[index]; }

const Span<const TokenNode> ScopeBuilder::GetTokenChildren(const int tokenID) const
{
    const TokenNode& ident = tokens[tokenID];
    return Span<const TokenNode>(&tokens[ident.childStart], ident.childCount);
}

const SymbolData& ScopeBuilder::GetSymbolData(int index) const { return symbols[index]; }

const ScopeData& ScopeBuilder::GetScopeData(int index) const { return scopes[index]; }

const FunctionData& ScopeBuilder::GetFuncData(int index) const { return functions[index]; }

const ShaderTypeInfo& ScopeBuilder::GetTypeData(int index) const { return *types[index]; }

const AttributeData& ScopeBuilder::GetAttribData(int index) const { return attributes[index]; }

int ScopeBuilder::GetScopeChild(const int scopeID, const int index) const
{
    return scopeSymbolLists[scopeID][index];
}

int ScopeBuilder::GetScopeChild(const int scopeID, string_view ident) const
{
    if (scopeSymbolMaps[scopeID].contains(ident))
        return scopeSymbolMaps[scopeID].find(ident)->second;
    else
        return -1;
}

size_t ScopeBuilder::GetScopeChildCount(const int scopeID) const { return scopeSymbolLists[scopeID].GetLength(); }

string_view ScopeBuilder::AddGeneratedText(string&& str) { return generatedText.emplace_front(std::move(str)); }

string_view ScopeBuilder::AddGeneratedText(string_view str) { return generatedText.emplace_front(str); }

bool ScopeBuilder::TryGetTokenFlags(TokenDef& token, int top) const
{
    if (token.name.GetLength() > 0)
    {
        if (TryGetShaderKeyword(token.name, token.tokenFlags))
        {
            return true;
        }
        else if (TryGetShaderType(token.name, token.typeFlags))
        {
            token.tokenFlags |= TokenTypes::IntrinsicType;
            return true;
        }
        else
        {
            int symbolID;

            if (TryGetSymbol(token.name, symbolID, top))
            {
                TokenTypes symbolFlags = tokens[symbols[symbolID].identID].type;
                token.tokenFlags |= symbolFlags & ~TokenTypes::Replica;
                return true;
            }
            else
                return false;
        }
    }
    else
        return false;
}

const ShaderTypeInfo* ScopeBuilder::TryGetType(string_view name, int top) const
{
    if (name.length() > 0)
    {
        ShaderTypes type = ShaderTypes::Void;

        if (TryGetShaderType(name, type))
        {
            return TryGetShaderTypeInfo(type);
        }
        else
        {
            int symbolID;

            if (TryGetSymbol(name, symbolID, top))
            {
                const TokenNode& ident = tokens[symbols[symbolID].identID];

                if (ident.GetHasFlags(TokenTypes::TypeAlias))
                    return types[ident.subtypeID];
                else if (ident.GetHasFlags(TokenTypes::UserType))
                    return &userTypes[ident.subtypeID];
            }
        }
    }
    
    return nullptr;
}

bool ScopeBuilder::TryGetSymbol(string_view name, int& symbolID, int top) const
{
    if (top == -1)
        top = topScope;

    const ScopeData* pScope;

    do
    {
        pScope = &scopes[top];
        auto nameSymbolPair = scopeSymbolMaps[top].find(name);

        if (nameSymbolPair != scopeSymbolMaps[top].end())
        {
            symbolID = nameSymbolPair->second;
            return true;
        }

        top = pScope->parentScope;

    } while (pScope->parentScope != -1);

    return false;
}

const IDList* ScopeBuilder::TryGetFuncOverloads(string_view ident, int top) const
{
    if (top == -1)
        top = topScope;

    const ScopeData* pScope;

    do
    {
        pScope = &scopes[top];     
        auto nameSymbolPair = funcOverloadMaps[top].find(ident);

        if (nameSymbolPair != funcOverloadMaps[top].end())
        {
            return &nameSymbolPair->second;
        }

        top = pScope->parentScope;

    } while (pScope->parentScope != -1);

    return nullptr;
}

bool ScopeBuilder::GetHasSymbol(string_view name, int top) const
{
    if (top == -1)
        top = topScope;

    const ScopeData* pScope;

    do
    {
        pScope = &scopes[top];

        if (scopeSymbolMaps[top].contains(name))
            return true;

        top = pScope->parentScope;

    } while (pScope->parentScope != -1);

    return false;
}

int ScopeBuilder::GetNewToken(string_view value, TokenTypes flags, const int depth, const int blockID)
{
    const int tokenID = (int)tokens.GetLength();
    TokenNode& token = tokens.EmplaceBack();
    token.value = value;
    token.type = flags;
    token.depth = depth;
    token.blockStart = blockID;
    token.blockCount = 1;
    token.childStart = 0;
    token.childCount = 0;
    token.subtypeID = -1;
    token.symbolID = -1;

    return tokenID;
}

int ScopeBuilder::GetNewTypeSpecifier(const int tokenID)
{
    TokenNode& token = tokens[tokenID];
    token.subtypeID = (int)types.GetLength();
    types.Add(TryGetType(token.value));

    return token.subtypeID;
}

int ScopeBuilder::GetNewAttribute(const int tokenID)
{
    TokenNode& token = tokens[tokenID];
    token.subtypeID = (int)attributes.GetLength();
    AttributeData& attribSpec = attributes.EmplaceBack();
    attribSpec.semanticIndex = -1;

    return token.subtypeID;
}

int ScopeBuilder::GetNewSymbol(const int tokenID, SymbolTypes flags)
{
    const int symbolID = (int)symbols.GetLength();
    SymbolData& symbol = symbols.EmplaceBack();
    symbol.identID = tokenID;
    symbol.scopeID = -1;
    symbol.type = flags;
    tokens[tokenID].symbolID = symbolID;

    return symbolID;
}

int ScopeBuilder::GetNewFunc(const int tokenID, SymbolTypes flags)
{
    const int funcID = GetNewSymbol(tokenID, flags);
    TokenNode& token = tokens[tokenID];
    token.subtypeID = (int)functions.GetLength();
    FunctionData& subtype = functions.EmplaceBack();

    return funcID;
}

int ScopeBuilder::GetNewTypeAlias(const int tokenID, SymbolTypes flags)
{
    const int symbolID = GetNewSymbol(tokenID, flags);
    TokenNode& ident = tokens[tokenID];

    for (const TokenNode& child : GetTokenChildren(tokenID))
    {
        if (child.GetHasFlags(TokenTypes::Type))
        {
            const ShaderTypeInfo* pType = TryGetType(child.value);

            ident.subtypeID = (int)types.GetLength();
            types.Add(pType);
            break;
        }
    }

    return symbolID;
}

int ScopeBuilder::GetNewUserType(const int tokenID, SymbolTypes flags)
{
    const int symbolID = GetNewSymbol(tokenID, flags);
    TokenNode& token = tokens[tokenID];
    token.type |= TokenTypes::UserType;
    token.subtypeID = (int)userTypes.GetLength();
    ShaderTypeInfo& type = userTypes.EmplaceBack();
    type.name = token.value;
    type.flags = ShaderTypes::UserType;
    type.size = 0;

    return symbolID;
}

void ScopeBuilder::PushScope(const int firstBlock, const int depth)
{
    // Add anonymous scope if no named scope is pending
    if (pendingScopeSymbol == -1)
    {
        const int symbolID = (int)symbols.GetLength();
        symbols.EmplaceBack(-1, -1, SymbolTypes::AnonScope);
        pendingScopeSymbol = symbolID;
    }

    AddScope(pendingScopeSymbol, firstBlock, 1);

    for (int index : deferredSymbolBuf)
        PushSymbol(index);

    deferredSymbolBuf.Clear();
    pendingScopeSymbol = -1;
}

void ScopeBuilder::PopScope(const int lastBlock)
{
    ScopeData& scope = scopes[topScope];

    if (scope.parentScope >= 0)
    {
        scope.blockCount = lastBlock - scope.blockStart + 1;
        topScope = scope.parentScope;
    }
    else
    {
        PARSE_ERR("Attempted to terminate global scope.")
    }
}

void ScopeBuilder::PushSymbol(const int symbolID, const bool isDeferred)
{
    const SymbolData& symbol = symbols[symbolID];

    if (isDeferred)
        deferredSymbolBuf.Add(symbolID);
    else if (symbol.GetHasFlags(SymbolTypes::Anonymous))
        scopeSymbolLists[topScope].EmplaceBack(symbolID);
    else
    {
        const TokenNode& token = tokens[symbol.identID];
        string_view name;

        if (token.GetHasFlags(TokenTypes::FuncIdent))
        {
            const FunctionData& func = functions[token.subtypeID];
            name = func.signature;
            AddFuncToOverloadTable(token.value, symbolID);
        }
        else
            name = token.value;

        PARSE_ASSERT_FMT(!GetHasSymbol(name), "Unexpected redefinition of symbol '{}'", name);

        scopeSymbolMaps[topScope].emplace(name, symbolID);
        scopeSymbolLists[topScope].EmplaceBack(symbolID);
    }

    if (symbol.GetHasFlags(SymbolTypes::Scope))
    {
        if (pendingScopeSymbol == -1)
            pendingScopeSymbol = symbolID;
        else
            PARSE_ERR("Only one pending scope can be queued")
    }
}
