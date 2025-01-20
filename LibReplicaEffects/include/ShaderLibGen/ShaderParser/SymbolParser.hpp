#pragma once
#include <queue>
#include <tuple>
#include <memory>
#include "SymbolData.hpp"
#include "BlockAnalyzer.hpp"
#include "SymbolPatterns.hpp"

namespace Replica::Effects
{
    class ScopeBuilder;
    struct MatchState;

    using std::queue;
    using std::tuple;
    using std::unique_ptr;

    /// <summary>
    /// A reusable shader parser for identifying symbols and tokens from a set of preprocessed LexBlocks
    /// </summary>
    class SymbolParser
    {
    public:
        MAKE_MOVE_ONLY(SymbolParser)

        SymbolParser();

        ~SymbolParser();

        /// <summary>
        /// Resets the parser, sets the source to the given value and writes symbols
        /// parsed to the given destination.
        /// </summary>
        void GetSymbols(const IDynamicArray<LexBlock>& src, ScopeBuilder& dst);

        /// <summary>
        /// Resets the parser to its initial state
        /// </summary>
        void Reset();

    private:
        ScopeBuilder* pSB;
        const IDynamicArray<LexBlock>* pSrcBlocks;
        UniqueVector<TokenNodeDef> tokenBuf;
        UniqueVector<TokenNodeDef> tokenParentBuf;

        UniqueVector<CaptureGroup> capSubGroups;
        UniqueVector<CaptureGroup> capGroups;
        UniqueVector<CaptureBlock> captures;
        UniqueVector<CaptureBlock> captureBuf;

        std::stringstream textBuf;

        /// <summary>
        /// Parses relevant symbols and tokens from source
        /// </summary>
        void ParseSource();

        /// <summary>
        /// Recursively matches BlockPatterns depth-first starting at the given MatchNode, and buffers 
        /// Block-Pattern pairs to be captured on successful pattern matches.
        /// </summary>
        int TryMatchPatternNode(const MatchNode& pattern, int matchStart);

        /// <summary>
        /// Attempts to match source blocks beginning at the given index to the MatchPatterns contained
        /// in the node, and queues corresponding captures.
        /// </summary>
        int TryMatchPattern(const MatchNode& pattern, int matchStart, const bool isAlternation);

        /// <summary>
        /// Attempts to match the LexBlock at the given index to a block pattern.
        /// Returns -1 on fail.
        /// </summary>
        int TryMatchBlockPattern(const MatchPattern& blockPattern, int matchStart, const int dir);

        /// <summary>
        /// Creates capture groups for the last successful match or reverts match to last good state
        /// </summary>
        void TryFinalizeCaptures(const MatchNode& pattern, const MatchState& state);

        /// <summary>
        /// Attempts to match a LexBlock at the given index with a matching pattern that uses the given
        /// token flags
        /// </summary>
        int TryMatchPatternType(const int start, const TokenTypes startFlags);

        int GetDirectiveEnd(int start, const int dir);

        /// <summary>
        /// Clears previous matches
        /// </summary>
        void ClearMatchBuffers();

        /// <summary>
        /// Captures all symbols and tokens indicated in the capture groups
        /// </summary>
        void CaptureSymbols();

        /// <summary>
        /// Captures direct children of the given CaptureGroup and queues grandchildren for later capture.
        /// </summary>
        void CaptureChildNodes(const CaptureGroup& node);

        /// <summary>
        /// Creates a new token from the given definition and returns its ID in the ScopeBuilder
        /// </summary>
        int GetNewToken(const TokenNodeDef& nodeDef);

        /// <summary>
        /// Finalizes and pushes the given symbol to the builder's table
        /// </summary>
        void PushSymbol(const int symbolID);

        /// <summary>
        /// Captures token definitions within the CapturePatterns and LexBlock specified in the given
        /// CaptureBlock
        /// </summary>
        void CaptureTokens(const CaptureBlock& cap);

        /// <summary>
        /// Generates a function signature for the function with the given identifier
        /// </summary>
        void GetFuncSignature(const TokenNode& ident);
    };
}