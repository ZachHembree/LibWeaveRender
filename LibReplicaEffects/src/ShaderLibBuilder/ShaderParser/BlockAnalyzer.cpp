#include "pch.hpp"
#include <charconv>
#include "ReplicaEffects/ParseExcept.hpp"
#include "ReplicaEffects/ShaderLibBuilder/ShaderParser/BlockAnalyzer.hpp"

namespace Replica::Effects
{
    static LexBlockTypes GetDelimiterType(const char ch)
    {
        switch (ch)
        {
        case '{':
            return LexBlockTypes::StartScope;
        case '(':
            return LexBlockTypes::OpenParentheses;
        case '[':
            return LexBlockTypes::OpenSquareBrackets;
        case '<':
            return LexBlockTypes::OpenAngleBrackets;
        case '}':
            return LexBlockTypes::EndScope;
        case ')':
            return LexBlockTypes::CloseParentheses;
        case ']':
            return LexBlockTypes::CloseSquareBrackets;
        case '>':
            return LexBlockTypes::CloseAngleBrackets;
        default:
            return LexBlockTypes::Unknown;
        }
    }

    const string_view s_BreakFilters[]
    {
        "=,:;{}()[]#", // 0 - No templates
        "=,:;{}()[]<#", // 1 - Can start templates
        "=,:;{}()[]<>#", // 2 - Can start or close templates
    };

    /// <summary>
    /// Returns appropriate filter based on current template instantiation or backtracking state.
    /// </summary>
    string_view BlockAnalyzer::GetBreakFilter() const 
    {
        const int index = (int)GetCanCloseTemplate() + (int)GetCanOpenTemplate();
        return s_BreakFilters[index];
    }

    /// <summary>
    /// Returns true if the analyzer can add a new template in its current state. If the analyzer 
    /// has not progressed past the last backtrack pass, then it cannot handle new potential 
    // template instances.
    /// </summary>
    bool BlockAnalyzer::GetCanOpenTemplate() const { return pPos > pPosOld; }

    /// <summary>
    /// Returns true if a template instantiation is pending completion and '>' should be considered.
    /// </summary>
    bool BlockAnalyzer::GetCanCloseTemplate() const { return !containers.IsEmpty() && blocks[containers.GetBack()].GetHasFlags(LexBlockTypes::OpenAngleBrackets); }

    static bool GetHasFlags(const LexBlockTypes value, const LexBlockTypes flags) { return (value & flags) == flags; }

    BlockAnalyzer::BlockAnalyzer() :
        containers(20),
        pPos(nullptr),
        depth(0),
        line(1),
        pPosOld(nullptr)
    { }

    void BlockAnalyzer::Clear()
    {
        files.Clear();
        containers.Clear();
        blocks.Clear();
        depth = 0;
        line = 1;
        pPos = nullptr;
        pPosOld = nullptr;
    }

    void BlockAnalyzer::AnalyzeSource(string_view path, TextBlock src)
    {
        Clear();
        this->src = src;
        pPos = src.GetData();
        files.EmplaceBack(path, line);

        while (true)
        {            
            // Parse
            if (pPos <= &src.GetBack())
            { 
                if (*pPos <= ' ') // Skip whitespace
                {
                    if (*pPos == '\n') // Count lines
                        line++;
                }
                else // Get blocks
                {
                    switch (*pPos)
                    {
                    case '#':
                        AddDirective();
                        break;
                    case '{':
                    case '[':
                    case '(':
                        StartContainer();
                        break;
                    case '<':
                        if (GetCanCloseTemplate() || GetCanOpenTemplate())
                        {
                            StartContainer();
                            break;
                        }
                        [[fallthrough]];
                    case '}':
                    case ']':
                    case ')':
                        EndContainer();
                        break;
                    case '>':
                        if (GetCanCloseTemplate())
                        {
                            EndContainer();
                            break;
                        }
                        [[fallthrough]];
                    default:
                        AddBlock({ pPos, &src.GetBack() });
                        break;
                    }
                }

                pPos++;
            }
            // Try exit
            else if (TryFinalizeParse())
                break;
        }
    }

    const IDynamicArray<LexFile>& BlockAnalyzer::GetSourceFiles() const { return files; }

    const IDynamicArray<LexBlock>& BlockAnalyzer::GetBlocks() const { return blocks; }

    void BlockAnalyzer::AddBlock(const TextBlock& start)
    {
        const string_view filter = GetBreakFilter();
        const char* pNext = start.GetData() - 1;

        do
        {
            pNext = start.FindEnd(pNext + 1, filter);
        } 
        while (pNext < &start.GetBack() && TextBlock::GetIsRangeChar(*pNext, filter));

        // Create new non-container block
        LexBlock& block = blocks.EmplaceBack();
        
        switch (*pNext)
        {
        case '=':
            block.type = LexBlockTypes::AssignmentSeparator;
            break;
        case ';':
            block.type = LexBlockTypes::SemicolonSeparator;
            break;
        case ':':
            block.type = LexBlockTypes::ColonSeparator;
            break;
        case ',':
            block.type = LexBlockTypes::CommaSeparator;
            break;
        case '{':
            block.type = LexBlockTypes::ScopePreamble;
            break;
        case '(':
            block.type = LexBlockTypes::ParenthesesPreamble;
            break;
        case '[':
            block.type = LexBlockTypes::SquareBracketsPreamble;
            break;
        case '<':
            block.type = LexBlockTypes::AngleBracketsPreamble;
            break;
        default:
            block.type = LexBlockTypes::Unterminated;
            break;
        }

        // If unterminated, leave out last char
        if ((int)(block.type & LexBlockTypes::Separator) == 0)
            pNext--;

        block.depth = depth;
        block.src = TextBlock(start.GetData(), pNext);
        block.startLine = line;
        block.lineCount = block.src.FindCount('\n');
        block.file = GetFileIndex();

        line += block.lineCount;
        pPos = &block.src.GetBack();
    }

    LexBlock* BlockAnalyzer::GetTopContainer() { return !containers.IsEmpty() ? &blocks[containers.GetBack()] : nullptr; }

    void BlockAnalyzer::SetState(int blockIndex)
    {
        const char* pLast = pPos;

        // Restart from the beginning but without template parsing
        if (blockIndex < 0)
        {
            Clear();          
        }
        // Revert state to the point just after the given block was added and temporarily 
        // disable template parsing until this point is reached again
        else 
        { 
            PARSE_ASSERT_MSG(blockIndex >= 0 && blockIndex < (int)blocks.GetLength(), "Block index out of range")

            LexBlock& block = blocks[blockIndex];
            pPos = &block.src.GetBack();
            // Depth is normally incremented after a container is added
            depth = block.depth + (int)block.GetHasFlags(LexBlockTypes::StartContainer);
            line = block.startLine + block.lineCount;

            while (!containers.IsEmpty() && containers.GetBack() > blockIndex)
                containers.RemoveBack();

            while (block.file > GetFileIndex())
                files.RemoveBack();

            blocks.RemoveRange(blockIndex + 1, (int)blocks.GetLength() - blockIndex - 1);
        }

        pPosOld = pLast;
    }

    void BlockAnalyzer::RevertContainer(int blockIndex)
    {
        PARSE_ASSERT_MSG(blockIndex >= 0 && blockIndex < (int)blocks.GetLength(), "Block index out of range")
        const LexBlock& block = blocks[blockIndex];

        // Revert to block before container preamble
        if (blockIndex > 0 && blocks[blockIndex - 1].GetHasFlags(LexBlockTypes::Preamble))
            SetState(blockIndex - 2);
        // Revert to block before container
        else
            SetState(blockIndex - 1);
    }

    void BlockAnalyzer::StartContainer()
    {
        LexBlockTypes delimType = GetDelimiterType(*pPos);

        PARSE_ASSERT_FMT(GetHasFlags(delimType, LexBlockTypes::StartContainer),
            "Unexpected expression {} on line: {}. Expected new container.", *pPos, line)

        // If an unfinished angle bracket container is on the stack, revert it if a different container
        // type is started. More restrictive than a normal parser, but should be an acceptable simplification.
        if (!GetHasFlags(delimType, LexBlockTypes::OpenAngleBrackets))
        { 
            const LexBlock* top = GetTopContainer();

            if (top != nullptr && top->GetHasFlags(LexBlockTypes::OpenAngleBrackets))
            {
                RevertContainer(containers.GetBack());
                return;
            }
        }

        // Start new container to be later finalized in LIFO order as closing braces are encountered
        containers.Add((int)blocks.GetLength());
        blocks.EmplaceBack(depth, delimType, pPos, line, GetFileIndex());
        depth++;
    }

    void BlockAnalyzer::EndContainer()
    {
        LexBlock* pCont = GetTopContainer();
        PARSE_ASSERT_FMT(pCont != nullptr, "Unexpected closing '{}' on line: {}", *pPos, line);

        // An opening '<' was previously classified as a potential template, but another container closed 
        // before it was ready. It will need to be reverted and reclassified.
        if (pCont->GetHasFlags(LexBlockTypes::OpenAngleBrackets) && (*pPos != '>'))
        {
            RevertContainer(containers.GetBack());
        }
        // Normal bounding {}, [], () or <> containers, closed in last-in, first-out order.
        else
        { 
            LexBlockTypes delimType = GetDelimiterType(*pPos);

            if (pCont->GetHasFlags(LexBlockTypes::Scope))
            {
                PARSE_ASSERT_FMT(GetHasFlags(delimType, LexBlockTypes::EndScope), 
                    "Expected scope end '}}' on line: {}", line);
            }
            else if (pCont->GetHasFlags(LexBlockTypes::Parentheses))
            {
                PARSE_ASSERT_FMT(GetHasFlags(delimType, LexBlockTypes::CloseParentheses), 
                    "Expected closing parentheses ')' on line: {}", line);
            }
            else if (pCont->GetHasFlags(LexBlockTypes::SquareBrackets))
            {
                PARSE_ASSERT_FMT(GetHasFlags(delimType, LexBlockTypes::CloseSquareBrackets), 
                    "Expected closing square bracket ']' on line: {}", line);
            }

            depth--;
            PARSE_ASSERT_FMT(depth >= 0, "Unexpected closing '{}' on line: {}", *pPos, line)

            // Finalize source range and line count in container opening
            containers.RemoveBack();
            pCont->src = TextBlock(pCont->src.GetData(), pPos);
            pCont->lineCount = line - pCont->startLine;

            // Add duplicate ending marker with appropriate delim flags
            LexBlock& endContainer = blocks.EmplaceBack(*pCont);
            endContainer.type = delimType;
        }
    }

    /// <summary>
    /// Finds bounds of a single preprocessor directive starting at the given character
    /// </summary>
    void BlockAnalyzer::AddDirective()
    {
        LexBlock& name = blocks.EmplaceBack();
        bool hasMoreLines;

        name.depth = depth;
        name.type = LexBlockTypes::DirectiveName;
        name.src = TextBlock(pPos, src.FindWordEnd(pPos) + 1);
        name.startLine = line;
        name.file = GetFileIndex();

        pPos = src.FindWord(&name.src.GetBack() + 1);

        const char* pLast = pPos;
        int lineCount = 0;

        do
        {
            const char* lnStart = pLast + 1;
            pLast = src.Find('\n', lnStart);
            TextBlock ln(lnStart, pLast);

            hasMoreLines = ln.Contains("\\\n");
            lineCount++;

        } while (hasMoreLines);

        bool isLineDirective = false;

        if (name.src.StartsWith("#line"))
            isLineDirective = true;

        LexBlock& body = blocks.EmplaceBack();
        body.depth = depth;
        body.type = LexBlockTypes::DirectiveBody;
        body.src = TextBlock(pPos, pLast);
        body.startLine = line;
        body.lineCount = lineCount;
        body.file = GetFileIndex();

        line += lineCount;
        pPos = &body.src.GetBack();

        if (isLineDirective)
            ProcessLineDirective(body);
    }

    void BlockAnalyzer::ProcessLineDirective(const LexBlock& body) 
    {
        // New line number
        const char* pFirstDigit = body.src.FindStart(body.src.GetData(), "", '0', '9');
        const char* pLastDigit = body.src.FindEnd(pFirstDigit, "", '0', '9');
        TextBlock numString(pFirstDigit, pLastDigit);

        PARSE_ASSERT_FMT(*pFirstDigit >= '0' && *pFirstDigit <= '9',
            "Expected a line number after #line directive on line {}", line);

        int newLine = -1;
        string_view newPath;
        std::from_chars(&numString.GetFront(), &numString.GetBack() + 1, newLine);

        PARSE_ASSERT_FMT(newLine >= 0,
            "Expected a line number after #line directive on line {}", line);

        // New file path
        if (pLastDigit < &body.src.GetBack())
        {
            const char* pFileStart = body.src.Find('"', pLastDigit + 1);
            const char* pFileEnd = body.src.FindEnd(pFileStart + 1, """");
            newPath = TextBlock(pFileStart, pFileEnd);
        }
        else
            newPath = files.GetBack().filePath;

        files.EmplaceBack(newPath, newLine);
        line = newLine;
    }
    
    /// <summary>
    /// Returns true if parsing has completed successfully, rasies exceptions if there are unterminated 
    /// containers, and possibly backtracks to correct misidentifed trailing template instantiations.
    /// </summary>
    bool BlockAnalyzer::TryFinalizeParse()
    {
        if (!containers.IsEmpty())
        {
            LexBlock& top = *GetTopContainer();

            if (top.GetHasFlags(LexBlockTypes::StartScope))
            {
                PARSE_ERR_FMT("Unterminated scope '{{' starting on line {}", top.startLine);
            }
            else if (top.GetHasFlags(LexBlockTypes::OpenParentheses))
            {
                PARSE_ERR_FMT("Unterminated parentheses '(' starting on line {}", top.startLine);
            }
            else if (top.GetHasFlags(LexBlockTypes::OpenSquareBrackets))
            {
                PARSE_ERR_FMT("Unterminated square bracket '[' starting on line {}", top.startLine);
            }
            else if (top.GetHasFlags(LexBlockTypes::OpenAngleBrackets))
            {
                RevertContainer(containers.GetBack());
                return false;
            }
        }

        PARSE_ASSERT_MSG(depth == 0, "Internal container parsing error.")
        return true;
    }

    int BlockAnalyzer::GetFileIndex() const { return (int)files.GetLength() - 1; }
}