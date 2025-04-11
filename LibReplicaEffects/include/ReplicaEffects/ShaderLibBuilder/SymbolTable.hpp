#pragma once
#include <memory>
#include "SymbolHandles.hpp"

namespace Replica::Effects
{
    struct LexBlock;
    class ScopeBuilder;
    class SymbolParser;

    /// <summary>
    /// A reusable table of token, symbol and scope data generated from preprocessed shader LexBlocks
    /// </summary>
    class SymbolTable
    {
    public:
        MAKE_MOVE_ONLY(SymbolTable)

        SymbolTable();

        ~SymbolTable();

        /// <summary>
        /// Resets the parser, sets the source to the given value and parses symbols
        /// from it.
        /// </summary>
        void ParseBlocks(const BlockAnalyzer& src);

        /// <summary>
        /// Resets the parser to its initial state
        /// </summary>
        void Clear();

        /// <summary>
        /// Returns an interface to token with the given ID
        /// </summary>
        TokenNodeHandle GetToken(const int id) const;

        /// <summary>
        /// Returns the number of tokens stored in the table
        /// </summary>
        int GetTokenCount() const;

        /// <summary>
        /// Returns an interface to the symbol with the given ID
        /// </summary>
        SymbolHandle GetSymbol(const int id) const;

        /// <summary>
        /// Returns the number of symbols in the table
        /// </summary>
        int GetSymbolCount() const;

        /// <summary>
        /// Returns an interface to the scope with the given ID
        /// </summary>
        ScopeHandle GetScope(const int id) const;

        /// <summary>
        /// Returns the number of scopes in the table
        /// </summary>
        int GetScopeCount() const;

    private:
        std::unique_ptr<ScopeBuilder> pSB;
        std::unique_ptr<SymbolParser> pParse;
    };
}