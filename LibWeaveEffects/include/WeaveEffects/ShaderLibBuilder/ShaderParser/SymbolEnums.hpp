#pragma once
#include "WeaveUtils/Utils.hpp"

namespace Weave::Effects
{ 
    enum class TokenTypes : uint
    {
        Unknown = 0,

        Intrinsic = 1u << 0u,
        UserDefined = 1u << 1u,

        Identifier = 1u << 2u,
        Keyword = 1u << 3u,
        Literal = 1u << 4u,

        Alias = 1u << 5u | UserDefined,

        Attribute = 1u << 6u,
        Semantic = 1u << 7u,
        Argument = 1u << 8u,
        Parameter = 1u << 9u,
        Variable = 1u << 10u,

        TypeModifier = 1u << 11 | Keyword,
        Type = 1u << 12u,

        IntrinsicType = Intrinsic | Type,
        UserType = 1u << 13 | UserDefined | Type,
        TypeAlias = Alias | UserType,

        Struct = 1u << 14 | Keyword,
        Typedef = 1u << 15 | Keyword,
        Function = 1u << 16u,
        Weave = 1u << 17u,

        Static = 1u << 18u | TypeModifier,
        Const = 1u << 19u | TypeModifier,
        GroupShared = 1u << 20u,

        Shader = 1u << 21u,
        Pass = 1u << 22u,
        Technique = 1u << 23u,
        ConstBuf = 1u << 24u,

        Vertex = 1u << 25u,
        Hull = 1u << 26u,
        Domain = 1u << 27u,
        Geometry = 1u << 28u,
        Pixel = 1u << 29u,
        Compute = 1u << 30u,

        Template = 1u << 31u,
        TemplatedType = Type | Template,

        WeaveDecl = Weave | Keyword,
        ShaderDecl = Shader | WeaveDecl,
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
        WeaveIdent = Weave | Identifier,
        
        ShaderIdent = Shader | WeaveIdent,
        VertexIdent = Vertex | ShaderIdent,
        HullIdent = Hull | ShaderIdent,
        DomainIdent = Domain | ShaderIdent,
        GeometryIdent = Geometry | ShaderIdent,
        PixelIdent = Pixel | ShaderIdent,
        ComputeIdent = Compute | ShaderIdent,
        ConstBufIdent = ConstBuf | Identifier,

        TechniqueIdent = Technique | WeaveIdent,
        TechniqueDecl = Technique | WeaveDecl,

        PassIdent = Pass | WeaveIdent,
        PassDecl = Pass | WeaveDecl,

        TechniqueShaderIdent = Technique | ShaderIdent,
        TechniqueShaderDecl = Technique | ShaderDecl,

        VertexShaderDecl = Vertex | ShaderDecl,
        HullShaderDecl = Hull | ShaderDecl,
        DomainShaderDecl = Domain | ShaderDecl,
        GeometryShaderDecl = Geometry | ShaderDecl,
        PixelShaderDecl = Pixel | ShaderDecl,
        ComputeShaderDecl = Compute | ShaderDecl,
        AttribShaderDecl = Attribute | ShaderDecl,
        ShaderMask = Vertex | Hull | Domain | Geometry | Pixel | Compute,

        LiteralArg = Literal | Argument,
        TypeModifierMask = Static | Const
    };

    BITWISE_ALL(TokenTypes, uint)

    enum class SymbolTypes : ulong
    {
        Unknown = 0x0,

        Scope = 1u << 0u,
        Weave = 1u << 1u,
        Shader = 1u << 2u,
        Pass = 1u << 3u,
        Technique = 1u << 4u,

        Vertex = 1u << 7u,
        Hull = 1u << 8u,
        Domain = 1u << 9u,
        Geometry = 1u << 10u,
        Pixel = 1u << 11u,
        Compute = 1u << 12u,

        Declaration = 1u << 16u,
        Definition = 1u << 17u,

        UserDefined = 1u << 18u,
        Type = 1u << 19u,

        Alias = 1u << 20u,
        Struct = 1u << 21u,
        Function = 1u << 22u,
        Parameter = 1u << 23u,
        Variable = 1u << 24u,
        ConstBuf = 1u << 25u,

        Ambiguous = 1u << 26u,
        Argument = 1u << 27u,
        Anonymous = 1u << 28u,

        AnonScope = Anonymous | Scope,
        AnonVariable = Anonymous | Variable,
        AmbigFuncVarDecl = Ambiguous | Function | Variable | Declaration,

        UserType = UserDefined | Type,
        TypeAlias = Alias | UserType,
        UserStruct = Struct | UserType,
        UserCBuf = ConstBuf | UserType,

        ScopedDefinition = Definition | Scope,
        WeaveDefinition = Weave | ScopedDefinition,
        ShaderDef = Shader | WeaveDefinition | Anonymous,

        TechniqueDef = Technique | WeaveDefinition,
        TechniquePassDecl = Pass | WeaveDefinition | Anonymous,
        TechniqueShaderDecl = Technique | Shader | Weave | Declaration | AnonVariable,

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

    enum class ShadeStages : sbyte
    {
        Unknown = -1,
        Vertex = 0,
        Hull = 1,
        Domain = 2,
        Geometry = 3,
        Pixel = 4,
        Compute = 5,

        ShadeStageCount
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