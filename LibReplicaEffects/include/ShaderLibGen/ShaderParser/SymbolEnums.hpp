#pragma once
#include "ReplicaUtils.hpp"

namespace Replica::Effects
{ 
    enum class TokenTypes : uint
    {
        Unknown = 0,

        Intrinsic = 1 << 0,
        UserDefined = 1 << 1,

        Identifier = 1 << 2,
        Keyword = 1 << 3,
        Literal = 1 << 4,

        Alias = 1 << 5 | UserDefined,

        Attribute = 1 << 6,
        Semantic = 1 << 7,
        Argument = 1 << 8,
        Parameter = 1 << 9,
        Variable = 1 << 10,

        TypeModifier = 1 << 11 | Keyword,
        Type = 1 << 12,

        IntrinsicType = Intrinsic | Type,
        UserType = 1 << 13 | UserDefined | Type,
        TypeAlias = Alias | UserType,

        Struct = 1 << 14 | Keyword,
        Typedef = 1 << 15 | Keyword,
        Function = 1 << 16,
        Replica = 1 << 17,

        Static = 1 << 18 | TypeModifier,
        Const = 1 << 19 | TypeModifier,
        GroupShared = 1 << 20,

        Shader = 1 << 21,
        Technique = 1 << 22,
        ConstBuf = 1 << 23,

        Vertex = 1 << 24,
        Hull = 1 << 25,
        Domain = 1 << 26,
        Geometry = 1 << 27,
        Pixel = 1 << 28,
        Compute = 1 << 29,

        Template = 1 << 30,
        TemplatedType = Type | Template,

        ReplicaDecl = Replica | Keyword,
        ShaderDecl = Shader | ReplicaDecl,
        StructDecl = Struct | Keyword,
        TypedefDecl = Typedef | Keyword,
        ConstBufDecl = ConstBuf | Keyword,

        AttribIdent = Attribute | Identifier,
        SemanticIdent = Semantic | Identifier,
        ParamIdent = Parameter | Identifier,
        VarIdent = Variable | Identifier,

        StructIdent = Struct | Identifier,
        TypeIdent = Typedef | Identifier,
        FuncIdent = Function | Identifier,
        RepIdent = Replica | Identifier,
        ShaderIdent = Shader | RepIdent,
        TechniqueIdent = Technique | RepIdent,
        ConstBufIdent = ConstBuf | Identifier,

        LiteralArg = Literal | Argument,

        TechniqueDecl = Technique | ReplicaDecl,
        VertexShaderDecl = Vertex | ShaderDecl,
        HullShaderDecl = Hull | ShaderDecl,
        DomainShaderDecl = Domain | ShaderDecl,
        GeometryShaderDecl = Geometry | ShaderDecl,
        PixelShaderDecl = Pixel | ShaderDecl,
        ComputeShaderDecl = Compute | ShaderDecl,

        AttribShaderDecl = Attribute | ShaderDecl,
        TypeModifierMask = Static | Const,
        ShaderMask = Vertex | Hull | Domain | Geometry | Pixel | Compute
    };

    BITWISE_ALL(TokenTypes, uint)

    enum class SymbolTypes : ulong
    {
        Unknown = 0x0,

        Scope = 1 << 0,
        ReplicaBlock = 1 << 1,
        Shader = 1 << 2,
        Technique = 1 << 3,

        Vertex = 1 << 4,
        Hull = 1 << 5,
        Domain = 1 << 6,
        Geometry = 1 << 7,
        Pixel = 1 << 8,
        Compute = 1 << 9,

        Declaration = 1 << 10,
        Definition = 1 << 11,

        UserDefined = 1 << 12,
        Type = 1 << 13,

        Alias = 1 << 16,
        Struct = 1 << 17,
        Function = 1 << 18,
        Parameter = 1 << 19,
        Variable = 1 << 20,
        ConstBuf = 1 << 21,

        Ambiguous = 1 << 22,
        Argument = 1 << 23,

        AmbigFuncVarDecl = Ambiguous | Function | Variable | Declaration,

        UserType = UserDefined | Type,
        TypeAlias = Alias | UserType,
        UserStruct = Struct | UserType,
        UserCBuf = ConstBuf | UserType,

        ScopedDefinition = Definition | Scope,
        ReplicaDefinition = ReplicaBlock | ScopedDefinition,
        ShaderDef = Shader | ReplicaDefinition,
        TechniqueDef = Technique | ReplicaDefinition,

        VertexShaderDef = Vertex | ShaderDef,
        HullShaderDef = Hull | ShaderDef,
        DomainShaderDef = Domain | ShaderDef,
        GeometryShaderDef = Geometry | ShaderDef,
        PixelShaderDef = Pixel | ShaderDef,
        ComputeShaderDef = Compute | ShaderDef,

        TypedefDecl = TypeAlias | Declaration,
        StructDecl = UserStruct | Declaration,
        TypedefStruct = TypeAlias | UserStruct,
        StructDef = UserStruct | ScopedDefinition,
        ConstBufDef = UserCBuf | ScopedDefinition,
        TypedefStructDef = TypedefStruct | ScopedDefinition,

        VariableDecl = Variable | Declaration,
        VariableAssignDef = VariableDecl | Definition,

        FuncDeclaration = Function | Declaration,
        FuncDefinition = Function | ScopedDefinition,

        ShaderMask = Vertex | Hull | Domain | Geometry | Pixel | Compute
    };

    BITWISE_ALL(SymbolTypes, ulong)

    enum class ShadeStages : byte
    {
        Unknown = 0,
        Vertex = 1,
        Hull = 2,
        Domain = 3,
        Geometry = 4,
        Pixel = 5,
        Compute = 6,

        ShadeStageCount = Compute
    };

    /// <summary>
    /// Extracts shade stage enum from the given token flags
    /// </summary>
    ShadeStages GetStageFromFlags(TokenTypes flags);

    /// <summary>
    /// Extracts shade stage enum from the given symbol flags
    /// </summary>
    ShadeStages GetStageFromFlags(SymbolTypes flags);

    /// <summary>
    /// Attempts to map the given keyword to its corresponding enum
    /// </summary>
    bool TryGetShaderKeyword(string_view name, TokenTypes& type);

    /// <summary>
    /// Attempts to map the given shader stage name to its corresponding enum
    /// </summary>
    bool TryGetShadeStage(string_view name, ShadeStages& stage);
}