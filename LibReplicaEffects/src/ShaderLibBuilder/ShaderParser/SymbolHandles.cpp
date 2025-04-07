#include "pch.hpp"
#include "ReplicaEffects/ShaderLibBuilder/SymbolHandles.hpp"
#include "ReplicaEffects/ShaderLibBuilder/ShaderParser/ScopeBuilder.hpp"
#include "ReplicaEffects/ShaderLibBuilder/ShaderParser/SymbolData.hpp"

using namespace Replica::Effects;

TokenNodeHandle::TokenNodeHandle(const ScopeBuilder& sb, int tokenID) :
	tokenID(tokenID),
	pSB(&sb)
{ }

string_view TokenNodeHandle::GetValue() const { return pSB->GetTokenNode(tokenID).value; }

int TokenNodeHandle::GetID() const { return tokenID; }

int TokenNodeHandle::GetDepth() const { return pSB->GetTokenNode(tokenID).depth; }

int TokenNodeHandle::GetBlockStart() const { return pSB->GetTokenNode(tokenID).blockStart; }

int TokenNodeHandle::GetBlockCount() const { return pSB->GetTokenNode(tokenID).blockCount; }

TokenTypes TokenNodeHandle::GetFlags() const { return pSB->GetTokenNode(tokenID).type; }

TokenNodeHandle TokenNodeHandle::GetChild(int index) const
{
	const int childID = pSB->GetTokenNode(tokenID).childStart + index;
	return TokenNodeHandle(*pSB, childID);
}

TokenNodeHandle TokenNodeHandle::operator[](int index) const
{
	return GetChild(index);
}

int TokenNodeHandle::GetChildCount() const { return pSB->GetTokenNode(tokenID).childCount; }

bool TokenNodeHandle::GetHasFlags(TokenTypes flags) const { return pSB->GetTokenNode(tokenID).GetHasFlags(flags); }

bool TokenNodeHandle::GetHasSymbol() const { return pSB->GetTokenNode(tokenID).symbolID != -1; }

optional<SymbolHandle> TokenNodeHandle::GetSymbol() const
{
	const int symbolID = pSB->GetTokenNode(tokenID).symbolID;

	if (symbolID != -1)
		return SymbolHandle(*pSB, symbolID);
	return 
		std::nullopt;
}

optional<AttribHandle> TokenNodeHandle::GetAsAttribute() const
{
	return AttribHandle(*pSB, tokenID);
}

bool TokenNodeHandle::operator==(const TokenNodeHandle& other) const
{
	return this->tokenID == other.tokenID;
}

bool TokenNodeHandle::operator!=(const TokenNodeHandle& other) const
{
	return !(*this == other);
}

SymbolHandle::SymbolHandle(const ScopeBuilder& sb, int id) :
	symbolID(id),
	pSB(&sb)
{ }

bool SymbolHandle::GetHasIdent() const
{
	return pSB->GetSymbolData(symbolID).identID != -1;
}

TokenNodeHandle SymbolHandle::GetIdent() const
{
	const int tokenID = pSB->GetSymbolData(symbolID).identID;
	return TokenNodeHandle(*pSB, tokenID);
}

string_view SymbolHandle::GetName() const
{
	const int tokenID = pSB->GetSymbolData(symbolID).identID;
	return tokenID != -1 ? pSB->GetTokenNode(tokenID).value : string_view();
}

int SymbolHandle::GetID() const { return symbolID; }

bool SymbolHandle::GetIsScope() const
{
	return pSB->GetSymbolData(symbolID).scopeID != -1;
}

SymbolTypes SymbolHandle::GetFlags() const { return pSB->GetSymbolData(symbolID).type; }

bool SymbolHandle::GetHasFlags(SymbolTypes flags) const { return pSB->GetSymbolData(symbolID).GetHasFlags(flags); }

optional<FuncHandle> SymbolHandle::GetAsFunc() const
{
	if (GetHasFlags(SymbolTypes::Function))
		return FuncHandle(*pSB, symbolID);
	else
		return std::nullopt;
}

optional<VarHandle> SymbolHandle::GetAsVar() const
{
	if (GetHasFlags(SymbolTypes::Variable) || GetHasFlags(SymbolTypes::Parameter))
		return VarHandle(*pSB, symbolID);
	else
		return std::nullopt;
}

bool SymbolHandle::operator==(const SymbolHandle& other) const
{
	return this->symbolID == other.symbolID;
}

bool SymbolHandle::operator!=(const SymbolHandle& other) const
{
	return !(*this == other);
}

optional<ScopeHandle> SymbolHandle::GetScope() const
{
	const int scopeID = pSB->GetSymbolData(symbolID).scopeID;

	if (scopeID != -1)
		return ScopeHandle(*pSB, scopeID);
	else
		return std::nullopt;
}

FuncHandle::FuncHandle(const ScopeBuilder& pSB, int id) :
	SymbolHandle(pSB, id)
{ }

string_view FuncHandle::GetSignature() const
{
	const int tokenID = pSB->GetSymbolData(symbolID).identID;
	const int subtypeID = pSB->GetTokenNode(tokenID).subtypeID;
	return pSB->GetFuncData(subtypeID).signature;
}

const ShaderTypeInfo& FuncHandle::GetReturnType() const
{
	const int tokenID = pSB->GetSymbolData(symbolID).identID;
	const TokenNode& node = pSB->GetTokenNode(tokenID);

	for (int i = 0; i < node.childCount; i++)
	{
		const TokenNode& child = pSB->GetTokenNode(i + node.childStart);

		if (child.GetHasFlags(TokenTypes::Type))
			return pSB->GetTypeData(child.subtypeID);
	}

	REP_THROW_MSG("Return type undefined");
}

void FuncHandle::GetReturnType(ShaderTypeInfo& typeInfo, TokenTypes& typeFlags) const
{
	const int tokenID = pSB->GetSymbolData(symbolID).identID;
	const TokenNode& node = pSB->GetTokenNode(tokenID);

	for (int i = 0; i < node.childCount; i++)
	{
		const TokenNode& child = pSB->GetTokenNode(i + node.childStart);

		if (child.GetHasFlags(TokenTypes::Type))
		{
			typeInfo = pSB->GetTypeData(child.subtypeID);
			typeFlags = child.type;
			return;
		}
	}

	REP_THROW_MSG("Return type undefined");
}

VarHandle::VarHandle(const ScopeBuilder& pSB, int id) :
	SymbolHandle(pSB, id)
{ }

size_t VarHandle::GetSize() const
{
	return GetType().size;
}

const ShaderTypeInfo& VarHandle::GetType() const
{
	const int tokenID = pSB->GetSymbolData(symbolID).identID;
	const TokenNode& node = pSB->GetTokenNode(tokenID);

	for (int i = 0; i < node.childCount; i++)
	{
		const TokenNode& child = pSB->GetTokenNode(i + node.childStart);

		if (child.GetHasFlags(TokenTypes::Type))
			return pSB->GetTypeData(child.subtypeID);
	}

	REP_THROW_MSG("Return type undefined");
}

void VarHandle::GetType(ShaderTypeInfo& typeInfo, TokenTypes& typeFlags) const
{
	const int tokenID = pSB->GetSymbolData(symbolID).identID;
	const TokenNode& node = pSB->GetTokenNode(tokenID);

	for (int i = 0; i < node.childCount; i++)
	{
		const TokenNode& child = pSB->GetTokenNode(i + node.childStart);

		if (child.GetHasFlags(TokenTypes::Type))
		{
			typeInfo = pSB->GetTypeData(child.subtypeID);
			typeFlags = child.type;
			return;
		}
	}

	REP_THROW_MSG("Return type undefined");
}

ScopeHandle::ScopeHandle(const ScopeBuilder& sb, int id) :
	scopeID(id),
	pSB(&sb)
{ }

int ScopeHandle::GetID() const { return scopeID; }

int ScopeHandle::GetBlockStart() const
{
	return pSB->GetScopeData(scopeID).blockStart;
}

int ScopeHandle::GetBlockCount() const
{
	return pSB->GetScopeData(scopeID).blockCount;
}

SymbolHandle ScopeHandle::GetSymbol() const
{
	const int symbolID = pSB->GetScopeData(scopeID).symbolID;
	return SymbolHandle(*pSB, symbolID);
}

SymbolHandle ScopeHandle::GetChild(int index) const
{
	int childID = pSB->GetScopeChild(scopeID, index);
	return SymbolHandle(*pSB, childID);
}

SymbolHandle ScopeHandle::operator[](int index) const
{
	return GetChild(index);
}

int ScopeHandle::GetChildCount() const { return (int)pSB->GetScopeChildCount(scopeID); }

optional<SymbolHandle> ScopeHandle::TryGetChild(string_view ident) const
{
	int symbolID = -1;
	
	if (pSB->TryGetSymbol(ident, symbolID, scopeID))
		return SymbolHandle(*pSB, symbolID);
	else
		return std::nullopt;
}

const IDList* ScopeHandle::TryGetFuncOverloads(string_view ident) const
{
	return pSB->TryGetFuncOverloads(ident, scopeID);
}

bool ScopeHandle::GetIsGlobal() const { return pSB->GetScopeData(scopeID).parentScope == -1; }

optional<ScopeHandle> ScopeHandle::GetParentScope() const
{
	const int parentID = pSB->GetScopeData(scopeID).parentScope;

	if (parentID != -1)
		return ScopeHandle(*pSB, parentID);
	else
		return std::nullopt;
}

bool ScopeHandle::operator==(const ScopeHandle& other) const
{
	return this->scopeID == other.scopeID;
}

bool ScopeHandle::operator!=(const ScopeHandle& other) const
{
	return !(*this == other);
}

AttribHandle::AttribHandle(const ScopeBuilder& pSB, int id) :
	TokenNodeHandle(pSB, id)
{ }

int AttribHandle::GetSemanticIndex() const
{
	const TokenNode& token = pSB->GetTokenNode(tokenID);
	const AttributeData& subtype = pSB->GetAttribData(token.subtypeID);
	return subtype.semanticIndex;
}
