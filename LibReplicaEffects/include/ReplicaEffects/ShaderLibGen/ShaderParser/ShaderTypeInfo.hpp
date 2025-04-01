#pragma once
#include <typeinfo>
#include "ReplicaUtils/Utils.hpp"

namespace Replica::Effects
{
    enum class ShaderTypes : ulong
    {
        Void = 0x0,

        Scalar = 1 << 1,
        Vector = 1 << 2,
        Matrix = 1 << 3,
        Resource = 1 << 4,

        Array = 1 << 5,
        Cube = 1 << 6,

        ConstBuf = 1 << 7 | Resource,
        Texture = 1 << 8 | Resource,
        Buffer = 1 << 9 | Resource,
        Sampler = 1 << 10 | Resource,

        Bool = 1 << 11,
        Integer = 1 << 12,
        UnsignedInt = 1 << 13,
        Half = 1 << 14,
        Float = 1 << 15,
        Double = 1 << 16,

        Min10 = 1 << 17,
        Min16 = 1 << 18,

        RandomWrite = 1 << 19,
        Structured = 1 << 20,

        Dim1 = 1 << 22,
        Dim2 = 1 << 23,
        Dim3 = 1 << 24,
        Dim4 = 1 << 25,

        DimAx1 = 1 << 26,
        DimAx2 = 1 << 27,
        DimAx3 = 1 << 28,
        DimAx4 = 1 << 29,

        UserType = 1ull << 30,
        Template = 1ull << 31,

        Texture1D = Texture | Dim1,
        Texture2D = Texture | Dim2,
        Texture3D = Texture | Dim3,

        RwTexture = RandomWrite | Texture,
        RWTexture1D = RandomWrite | Texture1D,
        RWTexture2D = RandomWrite | Texture2D,
        RWTexture3D = RandomWrite | Texture3D,

        Texture1DArray = Texture1D | Array,
        Texture2DArray = Texture2D | Array,

        RwTexture1DArray = RandomWrite | RWTexture1D,
        RwTexture2DArray = RandomWrite | RWTexture2D,

        TextureCube = Texture | Cube,
        TextureCubeArray = TextureCube | Array,

        RwBuffer = RandomWrite | Buffer,
        StructuredBuffer = Structured | Buffer,
        RwStructuredBuffer = RandomWrite | StructuredBuffer,

        HalfFloat = Half | Float,
        Min10Float = Min10 | Float,
        Min16Float = Min16 | Float,
        Min10Int = Min10 | Integer,
        Min16Int = Min16 | Integer,
        Min10UInt = Min10 | UnsignedInt,
        Min16UInt = Min16 | UnsignedInt,
    };

    BITWISE_ALL(ShaderTypes, ulong)

	struct ShaderTypeInfo
	{
        string_view name;
        ShaderTypes flags;
		size_t size;

        bool GetHasFlags(ShaderTypes flags) const { return (this->flags & flags) == flags; }
	};

    const ShaderTypeInfo* TryGetShaderTypeInfo(ShaderTypes flags);

    bool TryGetShaderType(string_view name, ShaderTypes& type);

    const ShaderTypeInfo* TryGetShaderTypeInfo(string_view name);
}

