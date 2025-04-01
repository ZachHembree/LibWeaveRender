#include "pch.hpp"
#include "ReplicaEffects/ParseExcept.hpp"
#include "ReplicaEffects/ShaderLibGen/ShaderParser/BlockAnalyzer.hpp"

using std::string;
using std::ios;
using std::stack;

namespace Replica::Effects
{
    const string_view g_FullBreakFilter = "=,:;{}()[]<>#";
    const string_view g_BasicBreakFilter = "=,:;{}()[]#";

    /// <summary>
    /// Exhaustively searches the given string for blocks of text enclosed by the given start and end strings
    /// and replaces them with spaces
    /// </summary>
    static size_t FilterTextBlocks(
        TextBlock text,
        const string_view& start,
        const string_view& end
    )
    {
        char* pFound = (char*)text.Find(start);
        char* pStart = nullptr;
        size_t blockCharCount = 0;

        while (pFound != nullptr)
        {
            if (pStart != nullptr) // Find end
            {
                const char* pLast = pFound + end.length();
                size_t blockLen = UnsignedDelta(pLast, pStart);
                blockCharCount += blockLen;

                for (int i = 0; i < blockLen; i++)
                {
                    if (pStart[i] != '\n')
                        pStart[i] = ' ';
                }

                pFound = (char*)text.Find(start, pFound + end.length());
                pStart = nullptr;
            }
            else // Find start
            {
                pStart = pFound;
                pFound = (char*)text.Find(end, pFound + start.length());
            }
        }

        return blockCharCount;
    }

    void GetSanitizedInput(Span<char> src)
    {
        // Replace any non-linebreak control chars with a space
        for (int i = 0; i < src.GetLength(); i++)
        {
            if (src[i] < '\n')
                src[i] = ' ';
        }

        // Filter block comments
        FilterTextBlocks(src, "/*", "*/");
        // Filter inline comments
        FilterTextBlocks(src, "//", "\n");
    }

    void AnalyzeBlocks(TextBlock src, UniqueVector<LexBlock>& blocks)
    {
        BlockAnalyzer bb;
        bb.AnalyzeSource(src);
    }

    BlockAnalyzer::BlockAnalyzer() :
        containers(20),
        pPos(nullptr),
        depth(0),
        line(1)
    { }

    void BlockAnalyzer::Clear()
    {
        containers.clear();
        blocks.clear();
        depth = 0;
        line = 1;
        pPos = nullptr;
    }

    void BlockAnalyzer::AnalyzeSource(TextBlock src)
    {
        Clear();
        this->src = src;
        pPos = src.GetFirst();

        while (pPos <= src.GetLast())
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
                case '<':
                    StartContainer();
                    break;
                case '}':
                case ']':
                case ')':
                case '>':
                    EndContainer();
                    break;
                default:
                    AddBlock({ pPos, src.GetLast() }, blocks.emplace_back(), g_FullBreakFilter);
                    break;
                }
            }

            pPos++;
        }

        CheckForDanglingContainers();
    }

    const IDynamicArray<LexBlock>& BlockAnalyzer::GetBlocks() const { return blocks; }

    void BlockAnalyzer::AddBlock(const TextBlock& start, LexBlock& block, string_view filter)
    {
        const char* pNext = start.GetFirst() - 1;

        do
        {
            pNext = start.FindEnd(pNext + 1, filter);
        } while (pNext < start.GetLast() && TextBlock::GetIsRangeChar(*pNext, filter));

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
            block.type |= LexBlockTypes::ScopePreamble;
            break;
        case '(':
            block.type |= LexBlockTypes::ParenthesesPreamble;
            break;
        case '[':
            block.type |= LexBlockTypes::SquareBracketsPreamble;
            break;
        case '<':
            block.type |= LexBlockTypes::AngleBracketsPreamble;
            break;
        default:
            block.type = LexBlockTypes::Unterminated;
            break;
        }

        // If unterminated, leave out last char
        if ((int)(block.type & LexBlockTypes::Separator) == 0)
            pNext--;

        block.depth = depth;
        block.src = TextBlock(start.GetFirst(), pNext);
        block.startLine = line;
        block.lineCount = block.src.FindCount('\n');

        line += block.lineCount;
        pPos = block.src.GetLast();
    }

    LexBlock& BlockAnalyzer::GetTopContainer()
    {
        PARSE_ASSERT_MSG(!containers.empty(), "Attempted to get non-existent container reference");
        const int i = containers[containers.GetLength() - 1];
        return blocks[i];
    }

    bool BlockAnalyzer::TryValidateAngleBlocks()
    {
        const int i = !containers.empty() ? containers[containers.GetLength() - 1] : -1;
        int last = -1;

        // If any other bracket type is encountered while an unterminated angle container is on the stack
        if (i != -1 && *pPos != '<' && *pPos != '>')
        {
            const LexBlock& cont = blocks[i];

            if (cont.GetHasFlags(LexBlockTypes::OpenAngleBrackets))
            {
                last = i;

                if (i > 0 && blocks[i - 1].GetHasFlags(LexBlockTypes::AngleBracketsPreamble))
                    last = i - 1;

                containers.pop_back();
            }
        }
        // Closing angle bracket without opening bracket
        else if (*pPos == '>' && (i == -1 || !GetTopContainer().GetHasFlags(LexBlockTypes::OpenAngleBrackets)))
        {
            if (!blocks.empty())
                last = (int)blocks.GetLength() - 1;
        }

        if (last != -1)
        {
            const char* pStart = pPos;
            LexBlock& block = blocks[last];
            depth = block.depth;
            line = block.startLine;
            blocks.RemoveRange(last + 1, (ptrdiff_t)blocks.GetLength() - last - 1);

            AddBlock({ block.src.GetFirst(), src.GetLast() }, block, g_BasicBreakFilter);
            pPos++;

            while (pPos < pStart)
            {
                if (*pPos > ' ')
                    AddBlock({ pPos, src.GetLast() }, blocks.emplace_back(), g_BasicBreakFilter);
                else if (*pPos == '\n')
                    line++;

                pPos++;
            }

            return false;
        }

        return true;
    }
    
    void BlockAnalyzer::StartContainer()
    {
        LexBlockTypes type = LexBlockTypes::Container;

        if (*pPos == '<')
        {
            type |= LexBlockTypes::OpenAngleBrackets;
        }
        else
        {
            TryValidateAngleBlocks();

            if (*pPos == '{')
                type |= LexBlockTypes::StartScope;
            else if (*pPos == '(')
                type |= LexBlockTypes::OpenParentheses;
            else if (*pPos == '[')
                type |= LexBlockTypes::OpenSquareBrackets;
        }

        containers.push_back((int)blocks.GetLength());
        blocks.emplace_back(depth, type, pPos, line);
        depth++;
    }
     
    void BlockAnalyzer::EndContainer()
    {
        TryValidateAngleBlocks();

        depth--;
        PARSE_ASSERT_FMT(depth >= 0, "Unexpected closing '{}' on line: {}", *pPos, line);

        LexBlock& cont = GetTopContainer();

        if (cont.GetHasFlags(LexBlockTypes::Scope))
        {
            PARSE_ASSERT_FMT(*pPos == '}', "Expected scope end '}}' on line: {}", line);
        }
        else if (cont.GetHasFlags(LexBlockTypes::Parentheses))
        {
            PARSE_ASSERT_FMT(*pPos == ')', "Expected closing parentheses ')' on line: {}", line);
        }
        else if (cont.GetHasFlags(LexBlockTypes::SquareBrackets))
        {
            PARSE_ASSERT_FMT(*pPos == ']', "Expected closing square bracket ']' on line: {}", line);
        }

        containers.pop_back();
        cont.src = TextBlock(cont.src.GetFirst(), pPos);
        cont.lineCount = line - cont.startLine;

        LexBlock& endContainer = blocks.emplace_back(cont);
        endContainer.type &= ~LexBlockTypes::StartContainer;
        endContainer.type |= LexBlockTypes::EndContainer;
    }

    /// <summary>
    /// Finds bounds of a single preprocessor directive starting at the given character
    /// </summary>
    void BlockAnalyzer::AddDirective()
    {
        LexBlock name, body;
        bool hasMoreLines;

        name.depth = depth;
        name.type = LexBlockTypes::DirectiveName;
        name.src = TextBlock(pPos, src.FindWordEnd(pPos) + 1);
        name.startLine = line;

        pPos = src.FindWord(name.src.GetLast() + 1);
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

        body.depth = depth;
        body.type = LexBlockTypes::DirectiveBody;
        body.src = TextBlock(pPos, pLast);
        body.startLine = line;
        body.lineCount = lineCount;

        line += lineCount;
        blocks.emplace_back(name);
        blocks.emplace_back(body);

        pPos = body.src.GetLast();
    }
    
    void BlockAnalyzer::CheckForDanglingContainers()
    {
        while (!containers.empty() && TryValidateAngleBlocks())
        {
            LexBlock& top = GetTopContainer();

            if (top.GetHasFlags(LexBlockTypes::Scope))
            {
                PARSE_ERR_FMT("Unterminated scope '{{' starting on line {}", top.startLine);
            }
            else if (top.GetHasFlags(LexBlockTypes::Parentheses))
            {
                PARSE_ERR_FMT("Unterminated parentheses '(' starting on line {}", top.startLine);
            }
            else if (top.GetHasFlags(LexBlockTypes::SquareBrackets))
            {
                PARSE_ERR_FMT("Unterminated square bracket '[' starting on line {}", top.startLine);
            }

            containers.pop_back();
        }
    }
}