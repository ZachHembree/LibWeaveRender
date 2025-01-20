#include "ParseExcept.hpp"
#include "ShaderLibGen/ShaderParser/BlockAnalyzer.hpp"
#include "ShaderLibGen/ShaderParser/ShaderTypeInfo.hpp"
#include "ShaderLibGen/ShaderParser/ScopeBuilder.hpp"
#include "ShaderLibGen/ShaderParser/SymbolParser.hpp"

namespace Replica::Effects
{
    const string g_WordBreakFilter = "=,:;[]";

    struct MatchState
    {
        // Match src bounds
        int matchStart;
        int matchEnd;

        // Ends of buffers before attempting match
        int capBufStart;
        int capStart;
        int groupStart;
        int subGroupStart;
    };

    template <typename T>
    static void AddVecRange(Vector<T>& dst, IDynamicArray<T>& src, ptrdiff_t start, ptrdiff_t count, bool isForward)
    {
        if (isForward)
        {
            dst.AddRange(src, start, count);
        }
        else
        {
            for (int i = 1; i <= count; i++)
                dst.Add(src[count - i]);
        }
    }

    SymbolParser::SymbolParser() :
        pSB(nullptr),
        pSrcBlocks(nullptr)
    { }

    SymbolParser::~SymbolParser() = default;

    void SymbolParser::GetSymbols(const IDynamicArray<LexBlock>& src, ScopeBuilder& dst)
    {
        Reset();

        pSrcBlocks = &src;
        pSB = &dst;
        REP_ASSERT_MSG(pSB != nullptr, "Parse destination not set")

        ParseSource();
    }

    void SymbolParser::Reset()
    {
        ClearMatchBuffers();

        tokenBuf.clear();
        textBuf.clear();
        textBuf.str({});
        
        pSrcBlocks = nullptr;
        pSB = nullptr;
    }

    void SymbolParser::ParseSource()
    {
        for (int i = 0; i < pSrcBlocks->GetLength(); i++)
        {
            const LexBlock& block = (*pSrcBlocks)[i];

            if (block.GetHasFlags(LexBlockTypes::Directive))
                continue;

            if (block.GetHasFlags(LexBlockTypes::StartScope))
            {
                pSB->PushScope(i, block.depth);
            }
            else if (block.GetHasFlags(LexBlockTypes::EndScope))
            {
                pSB->PopScope(i);
            }
            else if (!block.GetHasFlags(LexBlockTypes::Container))
            {
                TokenDef startToken(block.src.GetWord(block.src.GetFirst(), g_WordBreakFilter));

                if (pSB->TryGetTokenFlags(startToken))
                {
                    const int length = TryMatchPatternType(i, startToken.tokenFlags);

                    if (length > 0)
                    {
                        CaptureSymbols();
                        i += length - 1;

                        if ((*pSrcBlocks)[i].GetHasFlags(LexBlockTypes::StartScope))
                            i--;
                    }
                }
            }
        }
    }

    int SymbolParser::TryMatchPatternType(const int start, const TokenTypes startFlags)
    {
        for (const MatchNodeGroup& group : MatchNodeGroup::MatchNodeGroups)
        {
            if (group.GetHasFlags(startFlags))
            {
                for (const MatchNode& rootPattern : group.rootNodes)
                {
                    ClearMatchBuffers();
                    int nextMatch = TryMatchPatternNode(rootPattern, start);

                    if (nextMatch != -1)
                    {
                        // Create default root node if the pattern didn't define one
                        if (captureBuf.GetLength() > 0)
                        {
                            const int subGroupStart = (int)capSubGroups.GetLength();
                            const int subGroupCount = (int)capGroups.GetLength();

                            capSubGroups.AddRange(capGroups);
                            capGroups.clear();

                            // Create new parent group
                            CaptureGroup& group = capGroups.emplace_back();
                            group.subGroupStart = subGroupStart;
                            group.subGroupCount = subGroupCount;
                            group.capStart = (int)captures.GetLength();
                            group.capCount = (int)captureBuf.GetLength();

                            captures.AddRange(captureBuf);
                            captureBuf.clear();
                        }

                        return nextMatch - start;
                    }
                }
            }
        }

        return -1;
    }

    void SymbolParser::ClearMatchBuffers()
    {
        capSubGroups.clear();
        capGroups.clear();
        captures.clear();
        captureBuf.clear();
    }

    int SymbolParser::TryMatchPatternNode(const MatchNode& pattern, int matchStart)
    {
        const bool isOptional = pattern.GetHasFlags(MatchQualifiers::Optional),
            isUnbounded = pattern.GetHasFlags(MatchQualifiers::OneOrMore),
            isAlternation = pattern.GetHasFlags(MatchQualifiers::Alternation);
        int lastStart = matchStart;
        
        do
        { 
            MatchState state;
            state.matchStart = matchStart;
            state.matchEnd = matchStart;
            state.capBufStart = (int)captureBuf.GetLength();
            state.capStart = (int)captures.GetLength();
            state.groupStart = (int)capGroups.GetLength();
            state.subGroupStart = (int)capSubGroups.GetLength();

            // Match main pattern
            if (pattern.GetHasMatchPatterns() && lastStart != -1)
            {
                lastStart = TryMatchPattern(pattern, matchStart, isAlternation);

                if (lastStart >= state.matchEnd)
                    state.matchEnd = lastStart - 1;
                else
                    state.matchStart = lastStart + 1;
            }

            // Match patterns in child nodes
            if (pattern.GetHasMatchNodes() && lastStart != -1)
            {
                lastStart = pattern.GetIsForward() ? std::max(0, matchStart - 1) : matchStart;
                bool wasReversed = pattern.GetIsForward();
                const IDynamicArray<MatchNode>& subpatterns = pattern.GetMatchNodes();
                const int lastNode = (int)subpatterns.GetLength() - 1;

                for (int i = 0; i < subpatterns.GetLength(); i++)
                {
                    if (wasReversed && subpatterns[i].GetIsForward())
                    {
                        wasReversed = false;
                        lastStart = matchStart;
                    }

                    const int nextStart = TryMatchPatternNode(subpatterns[i], lastStart);
                    const bool isEnd = (isAlternation && (i != lastNode)) ^ (nextStart == -1);

                    if (isEnd || nextStart != -1)
                    { 
                        lastStart = nextStart;

                        if (lastStart >= state.matchEnd)
                            state.matchEnd = lastStart - 1;
                        else
                            state.matchStart = lastStart + 1;
                    }

                    if (isEnd)
                        break;
                }
            }

            // If the matches succeed, then lastStart always equals matchStart
            // If any fail, then lastStart == -1
            if (lastStart != -1)
                matchStart = lastStart;

            TryFinalizeCaptures(pattern, state);

        } while (isUnbounded && lastStart != -1);

        return (lastStart == -1 && isOptional) ? matchStart : lastStart;
    }

    int SymbolParser::TryMatchPattern(const MatchNode& node, int matchStart, const bool isAlternation)
    {
        const IDynamicArray<MatchPattern>& matchPatterns = node.GetMatchPatterns();
        const int last = (int)matchPatterns.GetLength() - 1;
        const int dir = node.GetIsForward() ? 1 : -1;

        for (int i = 0; i < (int)matchPatterns.GetLength(); i++)
        {
            const MatchPattern& matchPattern = matchPatterns[i];
            const bool isOptional = matchPattern.GetHasFlags(MatchQualifiers::Optional) || (isAlternation && i < last),
                isUnbounded = matchPattern.GetHasFlags(MatchQualifiers::OneOrMore);
            const int patternStart = matchStart;
            int nextStart;

            do
            { 
                matchStart = GetDirectiveEnd(matchStart, dir);
                nextStart = TryMatchBlockPattern(matchPattern, matchStart, dir);

                if (nextStart == -1)
                {
                    if (!isOptional && (matchStart == patternStart))
                        return -1;
                }
                else if (nextStart != matchStart)
                { 
                    if (matchPattern.GetHasCapPattern())
                        captureBuf.emplace_back(matchStart, matchPattern.GetCapPatterns());

                    matchStart = nextStart;
                }
            } 
            while (isUnbounded && nextStart != -1);
        }

        return matchStart;
    }

    int SymbolParser::TryMatchBlockPattern(const MatchPattern& matchPattern, int matchStart, const int dir)
    {
        REP_ASSERT_MSG(matchPattern.GetLength() > 0, "Empty matching patterns are not allowed");

        const bool isAlternation = matchPattern.GetHasFlags(MatchQualifiers::Alternation);
        const int last = (int)matchPattern.GetLength() - 1;

        for (int i = 0; i < (int)matchPattern.GetLength(); i++)
        {
            const BlockQualifier& pattern = matchPattern[i];
            const bool isWild = pattern.type == LexBlockTypes::Unknown,
                isOptional = pattern.GetHasFlags(MatchQualifiers::Optional) || (isAlternation && i < last),
                isUnbounded = pattern.GetHasFlags(MatchQualifiers::OneOrMore);
            int matchEnd = std::min(matchStart + dir, (int)pSrcBlocks->GetLength());

            if (isUnbounded) // Bound greedy match
            {
                const BlockQualifier& gMatchNext = matchPattern[i + 1];
                int newEnd = -1;

                for (int j = matchStart; (j >= 0 && j < pSrcBlocks->GetLength()); j += dir)
                {
                    if ((*pSrcBlocks)[j].GetHasFlags(gMatchNext.type))
                    {
                        newEnd = j;
                        break;
                    }
                }

                if (newEnd == -1)
                    return -1;

                matchEnd = newEnd;
            }

            // Attempt to match bounded block range
            int nextStart = matchStart;

            while ((nextStart != matchEnd))
            {
                const LexBlock& block = (*pSrcBlocks)[nextStart];
                const bool canSkip = isWild || (nextStart != matchStart && block.GetHasFlags(LexBlockTypes::Directive));

                if (pattern.GetHasFlags(block.type) || canSkip)
                    nextStart += dir;
                else
                    break;
            }

            if ((nextStart == matchStart) && !isOptional) // Zero matches and not optional
                return -1;
            else if (nextStart >= 0 && (nextStart != matchStart))
            {
                matchStart = nextStart;

                if (isAlternation)
                    break;
            }
        }

        return matchStart;
    }

    void SymbolParser::TryFinalizeCaptures(const MatchNode& pattern, const MatchState& state)
    {
        if (state.matchStart < 0) // Clear failed captures
        {
            captureBuf.RemoveRange(state.capBufStart, captureBuf.GetLength() - state.capBufStart);
            captures.RemoveRange(state.capStart, captures.GetLength() - state.capStart);
            capSubGroups.RemoveRange(state.subGroupStart, capSubGroups.GetLength() - state.subGroupStart);
            capGroups.RemoveRange(state.groupStart, capGroups.GetLength() - state.groupStart);
        }
        else if (pattern.GetParentToken() != TokenTypes::Unknown) // Create new parent group and demote children
        {
            const int subGroupCount = (int)capGroups.GetLength() - state.groupStart;
            const int capCount = (int)captureBuf.GetLength() - state.capBufStart;
            const int subGroupStart = (int)capSubGroups.GetLength();

            if (capCount > 0 || subGroupCount > 0)
            { 
                // Move children to sub group buffer
                AddVecRange(capSubGroups, capGroups, state.groupStart, subGroupCount, pattern.GetIsForward());
                capGroups.RemoveRange(state.groupStart, subGroupCount);

                // Create new parent group
                CaptureGroup& group = capGroups.emplace_back();
                group.subGroupStart = subGroupStart;
                group.subGroupCount = subGroupCount;
                group.capStart = (int)captures.GetLength();
                group.capCount = capCount;
                group.srcStart = state.matchStart;
                group.srcCount = state.matchEnd - state.matchStart + 1;

                AddVecRange(captures, captureBuf, state.capBufStart, capCount, pattern.GetIsForward());
                captureBuf.RemoveRange(state.capBufStart, capCount);

                for (int i = group.capStart; i < captures.GetLength(); i++)
                {
                    const auto& patterns = *captures[i].pPatterns;

                    for (int j = 0; j < patterns.GetLength(); j++)
                    {
                        if (patterns[j].GetHasFlags(pattern.GetParentToken()))
                        {
                            group.parentCap = i;
                            captures[i].mainCap = j;
                            break;
                        }
                    }
                }
            }
        }
    }

    int SymbolParser::GetDirectiveEnd(int start, const int dir)
    {
        while (start > 0 && start < pSrcBlocks->GetLength() && (*pSrcBlocks)[start].GetHasFlags(LexBlockTypes::Directive))
            start += dir;

        return start;
    }

    void SymbolParser::CaptureSymbols()
    {
        for (CaptureGroup& root : capGroups)
        {          
            // Get token definitions
            const int tokenStart = (int)pSB->GetTokenCount();
            CaptureChildNodes(root);
            tokenBuf.AddRange(tokenParentBuf);

            // Create tokens from definitions
            for (const TokenNodeDef& node : tokenBuf)
                GetNewToken(node);

            // Add symbols to symbol table
            for (int tokenID = tokenStart; tokenID < pSB->GetTokenCount(); tokenID++)
            {
                const TokenNode& token = pSB->GetTokenNode(tokenID);

                if (token.symbolID != -1)
                    PushSymbol(token.symbolID);
            }

            tokenBuf.clear();
            tokenParentBuf.clear();
        }
    }

    void SymbolParser::CaptureChildNodes(const CaptureGroup& node)
    {
        const int nodeStart = (int)tokenParentBuf.GetLength();

        if (node.subGroupCount > 0) // Recusively capture children in depth-first order
        {
            const Span<CaptureGroup> subGroups(&capSubGroups[node.subGroupStart], node.subGroupCount);

            for (const CaptureGroup& subGroup : subGroups)
                CaptureChildNodes(subGroup);
        }

        const int childStart = (int)tokenBuf.GetLength();
        const CaptureBlock* pStart = (node.parentCap != -1) ? &captures[node.parentCap] : nullptr;

        if (node.capCount > 0) // Get non-owning captures
        {
            const Span<CaptureBlock> caps(&captures[node.capStart], node.capCount);

            for (const CaptureBlock& cap : caps)
            {
                if (&cap != pStart)
                {
                    REP_ASSERT_MSG(cap.mainCap == -1, "Only one owner per token group is allowed");
                    CaptureTokens(cap);
                }
            }
        }

        if (pStart != nullptr) // if owner exists, add to parent queue
        {
            // Move completed parents into main buffer
            tokenBuf.AddRange(tokenParentBuf, nodeStart, tokenParentBuf.GetLength() - nodeStart);
            tokenParentBuf.RemoveRange(nodeStart, tokenParentBuf.GetLength() - nodeStart);

            // Get new parent
            REP_ASSERT_MSG(pStart->mainCap != -1, "Owner specified but not captured");
            const int capStart = (int)tokenBuf.GetLength();
            CaptureTokens(*pStart);

            const int mainIndex = capStart + pStart->mainCap;
            TokenNodeDef& owner = tokenParentBuf.emplace_back(tokenBuf[mainIndex]);
            tokenBuf.RemoveAt(mainIndex);

            owner.blockStart = node.srcStart;
            owner.blockCount = node.srcCount;
            owner.childStart = childStart + (int)pSB->GetTokenCount();
            owner.childCount = (int)tokenBuf.GetLength() - childStart;
        }
    }

    void SymbolParser::CaptureTokens(const CaptureBlock& cap)
    {
        const int tokenStart = (int)tokenBuf.GetLength();
        const IDynamicArray<CapturePattern>& patterns = *cap.pPatterns;
        const LexBlock& block = (*pSrcBlocks)[cap.blockID];
        const char* pStart = block.src.GetFirst();
        const char* pLast = nullptr;

        for (int i = 0; i < patterns.GetLength(); i++)
        {
            const CapturePattern& pattern = patterns[i];
            TokenDef ident;

            for (int j = 0; j < pattern.GetLength(); j++)
            {
                const TokenQualifier& match = pattern[j];
                const bool isOptional = match.GetHasFlags(MatchQualifiers::Optional),
                    isUnbounded = match.GetHasFlags(MatchQualifiers::OneOrMore);

                do // Predicates
                {
                    pLast = block.src.FindWordEnd(pStart, g_WordBreakFilter);
                    TokenDef token(TextBlock(pStart, pLast));

                    if (pSB->TryGetTokenFlags(token) && token.GetHasFlags(match.type))
                    {
                        ident.tokenFlags |= token.tokenFlags;
                    }
                    else if (isOptional)
                    {
                        break;
                    }
                    else
                    {
                        PARSE_ERR_FMT("Unexpected expression: {}", string_view(token.name))
                    }

                    pStart = block.src.FindWord(pLast + 1, g_WordBreakFilter);

                } while (isUnbounded && pStart > pLast);
            }

            // Identifier
            ident.name = TextBlock(pStart, block.src.FindWordEnd(pStart, g_WordBreakFilter));

            if (!pSB->TryGetTokenFlags(ident) || pattern.symbolType == SymbolTypes::Unknown 
                || pattern.GetHasFlags(SymbolTypes::FuncDefinition))
            {
                ident.tokenFlags |= pattern.tokenType;
                tokenBuf.emplace_back(ident, pattern.symbolType, cap.blockID);
                pStart = block.src.FindWord(ident.name.GetLast() + 1, g_WordBreakFilter);
            }
            else
            {
                PARSE_ERR_FMT("Unexpected expression: {}", string_view(ident.name))
            }
        }

        PARSE_ASSERT_MSG((tokenBuf.GetLength() - tokenStart) == patterns.GetLength(), "Expected an identifier");
    }

    static void GetSemanticIndex(TokenNode& ident, AttributeData& attrib)
    {
        if (ident.value.back() >= '0' && ident.value.back() <= '9')
        {
            TextBlock idText = ident.value;
            idText = idText.GetLastWord(&ident.value.back(), "", '0', '9');
            ident.value = TextBlock(ident.value.data(), idText.GetFirst() - 1);

            const int len = std::min((int)idText.GetLength(), 9);
            char buf[10];

            for (int i = 0; i < len; i++)
                buf[i] = idText[i];

            buf[len] = '\0';
            attrib.semanticIndex = std::atoi((char*)&buf);
        }
    }

    int SymbolParser::GetNewToken(const TokenNodeDef& nodeDef)
    {
        const LexBlock& block = (*pSrcBlocks)[nodeDef.blockStart];
        const int tokenID = pSB->GetNewToken(nodeDef.identifier.name, nodeDef.identifier.tokenFlags,
            block.depth, nodeDef.blockStart);
        TokenNode& ident = pSB->GetTokenNode(tokenID);
        ident.blockCount = nodeDef.blockCount;
        ident.childStart = nodeDef.childStart;
        ident.childCount = nodeDef.childCount;

        if (nodeDef.GetHasFlags(SymbolTypes::Function) && !nodeDef.GetHasFlags(SymbolTypes::Ambiguous))
        {
            pSB->GetNewFunc(tokenID, nodeDef.symbolFlags);
        }
        else if (nodeDef.GetHasFlags(SymbolTypes::TypeAlias))
        {
            pSB->GetNewTypeAlias(tokenID, nodeDef.symbolFlags);
        }
        else if (nodeDef.GetHasFlags(SymbolTypes::Struct) || nodeDef.GetHasFlags(SymbolTypes::ConstBuf))
        {
            pSB->GetNewUserType(tokenID, nodeDef.symbolFlags);
        }
        else if (nodeDef.symbolFlags != SymbolTypes::Unknown)
        {
            pSB->GetNewSymbol(tokenID, nodeDef.symbolFlags);
        }
        else if (nodeDef.GetHasFlags(TokenTypes::Type))
        {
            pSB->GetNewTypeSpecifier(tokenID);
        }
        else if (nodeDef.GetHasFlags(TokenTypes::Attribute) || nodeDef.GetHasFlags(TokenTypes::Semantic))
        {
            const int subtypeID = pSB->GetNewAttribute(tokenID);
            AttributeData& attrib = pSB->GetAttribData(subtypeID);
            TryGetShaderKeyword(ident.value, ident.type); // add keyword flags to identifier
            GetSemanticIndex(ident, attrib);
        }

        return tokenID;
    }

    void SymbolParser::PushSymbol(const int symbolID)
    {
        const SymbolData& symbol = pSB->GetSymbolData(symbolID);
        const bool isDeferred = symbol.GetHasFlags(SymbolTypes::Parameter);

        if (symbol.GetHasFlags(SymbolTypes::FuncDefinition))
        {
            GetFuncSignature(pSB->GetTokenNode(symbol.identID));
        }

        pSB->PushSymbol(symbolID, isDeferred);
    }

    void SymbolParser::GetFuncSignature(const TokenNode& ident)
    {
        int paramCount = 0;

        textBuf.clear();
        textBuf.str({});
        textBuf << ident.value << "(";

        // Find param symbols and get type data
        for (int paramID = ident.childStart; paramID < (ident.childStart + ident.childCount); paramID++)
        {
            const TokenNode& child = pSB->GetTokenNode(paramID);

            if (child.symbolID != -1 && child.GetHasFlags(TokenTypes::Parameter))
            {
                for (const TokenNode& paramChild : pSB->GetTokenChildren(paramID))
                {
                    if (paramChild.GetHasFlags(TokenTypes::Type))
                    {
                        const ShaderTypeInfo& subtype = pSB->GetTypeData(paramChild.subtypeID);

                        if (paramCount > 0)
                            textBuf << ",";

                        textBuf << subtype.name;
                        paramCount++;
                        break;
                    }
                }
            }
        }

        if (paramCount == 0)
            textBuf << "void";

        textBuf << ")";

        FunctionData& func = pSB->GetFuncData(ident.subtypeID);
        func.signature = pSB->AddGeneratedText(textBuf.str());
    }
}