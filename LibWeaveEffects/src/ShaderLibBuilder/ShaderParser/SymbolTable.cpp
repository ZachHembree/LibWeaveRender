#include "pch.hpp"
#include "WeaveEffects/ShaderLibBuilder/ShaderParser/ScopeBuilder.hpp"
#include "WeaveEffects/ShaderLibBuilder/ShaderParser/SymbolParser.hpp"
#include "WeaveEffects/ShaderLibBuilder/SymbolTable.hpp"

namespace Weave::Effects
{
    SymbolTable::SymbolTable() :
        pSB(new ScopeBuilder()),
        pParse(new SymbolParser())
    { }

    SymbolTable::~SymbolTable() = default;

    void SymbolTable::ParseBlocks(const BlockAnalyzer& src)
    {
        pSB->Clear();
        pParse->GetSymbols(src, *pSB);
    }

    void SymbolTable::Clear()
    {
        pSB->Clear();
        pParse->Reset();
    }

    TokenNodeHandle SymbolTable::GetToken(const int id) const { return TokenNodeHandle(*pSB, id); }

    int SymbolTable::GetTokenCount() const { return (int)pSB->GetTokenCount(); }

    SymbolHandle SymbolTable::GetSymbol(const int id) const { return SymbolHandle(*pSB, id); }

    int SymbolTable::GetSymbolCount() const { return (int)pSB->GetSymbolCount(); }

    ScopeHandle SymbolTable::GetScope(const int id) const { return ScopeHandle(*pSB, id); }

    int SymbolTable::GetScopeCount() const { return (int)pSB->GetScopeCount(); }

}