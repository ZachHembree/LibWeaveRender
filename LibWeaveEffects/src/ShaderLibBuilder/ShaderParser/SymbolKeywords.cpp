#include "pch.hpp"
#include <array>
#include <unordered_map>
#include "WeaveEffects/ShaderLibBuilder/ShaderParser/SymbolEnums.hpp"

namespace Weave::Effects
{
    static const std::unordered_map<std::string_view, TokenTypes> s_KeywordToTypeMap
    {
        { "technique", TokenTypes::TechniqueDecl },
        { "effect", TokenTypes::TechniqueDecl },
        { "pass", TokenTypes::PassDecl },
        { "cbuffer", TokenTypes::ConstBufDecl, },

        { "vertex", TokenTypes::VertexShaderDecl },
        { "hull", TokenTypes::HullShaderDecl },
        { "domain", TokenTypes::DomainShaderDecl },
        { "geometry", TokenTypes::GeometryShaderDecl },
        { "pixel", TokenTypes::PixelShaderDecl },
        { "fragment", TokenTypes::PixelShaderDecl },
        { "frag", TokenTypes::PixelShaderDecl },
        { "compute", TokenTypes::ComputeShaderDecl },
        { "kernel", TokenTypes::ComputeShaderDecl },

        { "typedef", TokenTypes::TypedefDecl },
        { "struct", TokenTypes::StructDecl },
        { "static", TokenTypes::Static },
        { "const", TokenTypes::Const },
        { "in", TokenTypes::TypeModifier },
        { "inout", TokenTypes::TypeModifier },
        { "out", TokenTypes::TypeModifier },
        { "uniform", TokenTypes::TypeModifier },
        { "groupshared", TokenTypes::GroupShared }
    };

    bool TryGetShaderKeyword(std::string_view name, TokenTypes& type)
    {
        if (name.length() > 20)
            return false;

        char cpyBuf[20];

        for (int i = 0; i < name.length(); i++)
            cpyBuf[i] = std::tolower(name[i]);

        string_view nameCpy((char*)&cpyBuf, name.length());
        auto keywordTypePair = s_KeywordToTypeMap.find(nameCpy);

        if (keywordTypePair != s_KeywordToTypeMap.end())
        {
            type |= keywordTypePair->second;
            return true;
        }
        else
            return false;
    }

    const std::unordered_map<string_view, ShadeStages> s_StageNameMap
    {
        { "vertex", ShadeStages::Vertex },
        { "hull", ShadeStages::Hull },
        { "domain", ShadeStages::Domain },
        { "geometry", ShadeStages::Geometry },
        { "pixel", ShadeStages::Pixel },
        { "fragment", ShadeStages::Pixel },
        { "frag", ShadeStages::Pixel },
        { "compute", ShadeStages::Compute },
        { "kernel", ShadeStages::Compute }
    };

    bool TryGetShadeStage(string_view name, ShadeStages& stage)
    {
        if (name.length() > 20)
            return false;

        char cpyBuf[20];
        
        for (int i = 0; i < name.length(); i++)
            cpyBuf[i] = std::tolower(name[i]);

        string_view nameCpy((char*)&cpyBuf, name.length());
        auto keywordTypePair = s_StageNameMap.find(nameCpy);

        if (keywordTypePair != s_StageNameMap.end())
        {
            stage = keywordTypePair->second;
            return true;
        }
        else
            return false;
    }

    constexpr std::array<string_view, (uint)ShadeStages::ShadeStageCount> s_StageNameList
    {
        "Vertex",
        "Hull",
        "Domain",
        "Geometry",
        "Pixel",
        "Compute"
    };

    string_view GetStageName(ShadeStages stage)
    {
        const uint index = (uint)stage;

        if (index >= 0 && index < s_StageNameList.size())
            return s_StageNameList[index];
        else
            return "Unknown";
    }

    constexpr static int GetFirstSetPos(ulong value)
    {
        for (int i = 0; i < 64; i++)
        {
            if ((value & (1ull << (ulong)i)) > 0)
                return (int)i;
        }

        return -1;
    }

    constexpr static int GetLastSetPos(ulong value)
    {
        for (int i = 63; i >= 0; i--)
        {
            if ((value & (1ull << (ulong)i)) > 0)
                return (int)i;
        }

        return -1;
    }

    constexpr int s_TokenShaderStart = GetFirstSetPos((ulong)TokenTypes::ShaderMask);
    constexpr int s_TokenShaderEnd = GetLastSetPos((ulong)TokenTypes::ShaderMask);
    constexpr int s_SymbolShaderStart = GetFirstSetPos((ulong)SymbolTypes::ShaderMask);
    constexpr int s_SymbolShaderEnd = GetLastSetPos((ulong)SymbolTypes::ShaderMask);

    ShadeStages GetStageFromFlags(TokenTypes flags)
    {
        for (int i = s_TokenShaderStart; i <= s_TokenShaderEnd; i++)
        {
            if (((ulong)flags & (1ull << i)) > 0)
                return (ShadeStages)(i - s_TokenShaderStart);
        }

        return ShadeStages::Unknown;
    }

    ShadeStages GetStageFromFlags(SymbolTypes flags)
    {
        for (int i = s_SymbolShaderStart; i <= s_SymbolShaderEnd; i++)
        {
            if (((ulong)flags & (1ull << i)) > 0)
                return (ShadeStages)(i - s_SymbolShaderStart);
        }

        return ShadeStages::Unknown;
    }
}