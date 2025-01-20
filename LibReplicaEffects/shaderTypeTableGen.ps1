<# Replica Effects Shader Type Map Generator

Generates a C++ table for mapping shader types, names and metadata

Example Pre-build Event:
cd $(ProjectDir)
powershell -ExecutionPolicy Bypass -File "shaderTypeTableGen.ps1"^
  -TablePath "$(ProjectDir)src\ShaderParser\ShaderTypeInfo.cpp"

#>

using namespace System.Text;

Param(
	[Parameter(mandatory=$true)]
	[string]$TablePath
)

[string]$glmPrecision = "defaultp";

# HLSL resource name - Enum pairs
[string[][]]$hlslResources = $(
	@( "void", "Void"),
	@( "matrix", "Matrix" ),
	@( "Texture", "Texture" ),
	@( "Texture1D", "Texture1D" ),
	@( "Texture2D", "Texture2D" ),
	@( "Texture3D", "Texture3D" ),
	@( "Texture1DArray", "Texture1DArray" ),
	@( "Texture2DArray", "Texture2DArray" ),
	@( "TextureCube", "TextureCube" ),
	@( "TextureCubeArray", "TextureCube" ),
	@( "RWTexture", "RwTexture" ),
	@( "RWTexture1D", "RWTexture1D" ),
	@( "RWTexture2D", "RWTexture2D" ),
	@( "RWTexture1DArray", "RwTexture1DArray" ),
	@( "RWTexture2DArray", "RwTexture2DArray" ),
	@( "Buffer", "Buffer" ),
	@( "StructuredBuffer", "StructuredBuffer" ),
	@( "RWBuffer", "RwBuffer" ),
	@( "RWStructuredBuffer", "RwStructuredBuffer" ),
	@( "SamplerState", "Sampler" )
);

# HLSL Type Name - Enum pairs
[string[][]]$hlslScalars = @(
	@("bool", "Bool"),
	@("uint", "UnsignedInt"),
	@("int", "Integer"),
	@("float", "Float"),
	@("double", "Double")
);

# Special HLSL low precision types - not compatible with normal IO types
[string[][]]$hlslLowP = @(
	@("half", "HalfFloat"),
	@("min10float", "Min10Float"),
	@("min16float", "Min16Float"),
	@("min10int", "Min10Int"),
	@("min16int", "Min16Int"),
	@("min10uint", "Min10UInt"),
	@("min16uint", "Min16UInt")
);

[string]$header = 
@"
#include "ShaderLibGen/ShaderParser/ShaderTypeInfo.hpp"
#include "ReplicaMath.hpp"
#include <unordered_map>

namespace Replica::Effects
{
	using MapEntry = std::unordered_map<ShaderTypes, ShaderTypeInfo>::value_type;

	template<typename T>
	static MapEntry GetTypeEntry(string_view name, ShaderTypes flags) 
	{ 
		return { flags, { name, flags, sizeof(T) } }; 
	}

"@;

[string]$footer = 
@"

const ShaderTypeInfo* TryGetShaderTypeInfo(ShaderTypes flags)
{
	const auto& pair = s_ShaderTypeMap.find(flags);

	if (pair != s_ShaderTypeMap.end())
	{
		return &pair->second;
	}
	else
		return nullptr;
}

bool TryGetShaderType(string_view name, ShaderTypes& type)
{
	const auto& pair = s_ShaderTypeNameMap.find(name);

	if (pair != s_ShaderTypeNameMap.end())
	{
		type |= pair->second;
		return true;
	}
	else
		return false;
}

const ShaderTypeInfo* TryGetShaderTypeInfo(string_view name)
{
	ShaderTypes flags = ShaderTypes::Void;

	if (TryGetShaderType(name, flags))
		return TryGetShaderTypeInfo(flags);
	else 
		return nullptr;
}
}
"@;

# Shader type to type data map
[string]$typeInfoMapDecl =
@"
static const std::unordered_map<ShaderTypes, ShaderTypeInfo> s_ShaderTypeMap =
{
"@;

# HLSL type name to shader type map
[string]$nameTypeMapDecl = 
@"
static const std::unordered_map<string_view, ShaderTypes> s_ShaderTypeNameMap
{
"@;

# Generates shader intrinsic enum to type data entries for a given base type (int/bool/float)
function AppendTypeInfoSubtypes {
	param (
		[string]$enumTypeName,
		[string]$baseTypeName
	)	

	# Scalar
	[string]$flags = "(ShaderTypes::Scalar|ShaderTypes::$($enumTypeName))";
	[void]$sb.AppendLine("GetTypeEntry<$($baseTypeName)>(`"$($baseTypeName)`",$($flags)),");

	for ([int]$i = 1; $i -lt 4; $i++)
	{
		# Vector
		[int]$dimX = $i + 1;
		$flags = "(ShaderTypes::Vector|ShaderTypes::$($enumTypeName)|ShaderTypes::Dim$($dimX))";
		$typeName = "$($baseTypeName)$($dimX)";
		$glmTypeName = "glm::vec<$($dimX),$($baseTypeName),glm::$($glmPrecision)>";
		[void]$sb.AppendLine("GetTypeEntry<$($glmTypeName)>(`"$($typeName)`",$($flags)),");

		# Matrices
		for ([int]$j = 1; $j -lt 4; $j++)
		{
			[int]$dimY = $j + 1;
			$flags = "(ShaderTypes::Matrix|ShaderTypes::$($enumTypeName)|ShaderTypes::Dim$($dimX)|ShaderTypes::DimAx$($dimY))";
			$typeName = "$($baseTypeName)$($dimX)x$($dimY)";
			$glmTypeName = "glm::mat<$($dimX),$($dimY),$($baseTypeName),glm::$($glmPrecision)>";
			[void]$sb.AppendLine("GetTypeEntry<$($glmTypeName)>(`"$($typeName)`",$($flags)),");
		}
	}
}

# Generates shader intrinsic enum to type data entries for a given low precision type
function AppendTypeInfoLowP {
	param (
		[string]$enumTypeName,
		[string]$baseTypeName
	)	

	# Scalar
	[string]$flags = "(ShaderTypes::Scalar|ShaderTypes::$($enumTypeName))";
	[void]$sb.AppendLine("{$($flags),{`"$($baseTypeName)`",$($flags),0u}},");

	for ([int]$i = 1; $i -lt 4; $i++)
	{
		# Vector
		[int]$dimX = $i + 1;
		$flags = "(ShaderTypes::Vector|ShaderTypes::$($enumTypeName)|ShaderTypes::Dim$($dimX))";
		$typeName = "$($baseTypeName)$($dimX)";
		[void]$sb.AppendLine("{$($flags),{`"$($typeName)`",$($flags),0u}},");

		# Matrices
		for ([int]$j = 1; $j -lt 4; $j++)
		{
			[int]$dimY = $j + 1;
			$flags = "(ShaderTypes::Matrix|ShaderTypes::$($enumTypeName)|ShaderTypes::Dim$($dimX)|ShaderTypes::DimAx$($dimY))";
			$typeName = "$($baseTypeName)$($dimX)x$($dimY)";
			[void]$sb.AppendLine("{$($flags),{`"$($typeName)`",$($flags),0u}},");
		}
	}
}

# Generates shader type name to enum entries for the given base type (int/half/float)
function AppendSubtypeNames {
	param (
		[string]$hlslBaseName,
		[string]$baseEnumName
	)

	# Scalar
	[string]$typeName = $hlslBaseName;
	[string]$flags = "(ShaderTypes::Scalar|ShaderTypes::$($baseEnumName))";
	[void]$sb.AppendLine("{`"$($typeName)`",$($flags)},");

	for ([int]$i = 1; $i -lt 4; $i++)
	{
		# Vector
		[int]$dimX = $i + 1;
		$typeName = "$($hlslBaseName)$($dimX)";
		$flags = "(ShaderTypes::Vector|ShaderTypes::$($baseEnumName)|ShaderTypes::Dim$($dimX))";
		[void]$sb.AppendLine("{`"$($typeName)`",$($flags)},");

		for ([int]$j = 1; $j -lt 4; $j++)
		{
			# Matrices
			[int]$dimY = $j + 1;
			$typeName = "$($hlslBaseName)$($dimX)x$($dimY)";
			$flags = "(ShaderTypes::Matrix|ShaderTypes::$($baseEnumName)|ShaderTypes::Dim$($dimX)|ShaderTypes::DimAx$($dimY))";
			[void]$sb.AppendLine("{`"$($typeName)`",$($flags)},");
		}
	}
}

function Write-Tables 
{
	Write-Host "Generating shader type lookup tables..."

	$sb = New-Object -TypeName StringBuilder;
	[void]$sb.EnsureCapacity(10000);
	[void]$sb.AppendLine($header);

	# Type info map
	[void]$sb.AppendLine($typeInfoMapDecl);

	foreach($pair in $hlslResources)
	{
		[void]$sb.AppendLine("{ShaderTypes::$($pair[1]),{`"$($pair[0])`",ShaderTypes::$($pair[1]),0u}},");
	}

	foreach ($pair in $hlslScalars)
	{
		AppendTypeInfoSubtypes -enumTypeName $pair[1] -baseTypeName $pair[0]
	}

	foreach ($pair in $hlslLowP)
	{
		AppendTypeInfoLowP -enumTypeName $pair[1] -baseTypeName $pair[0]
	}

	[void]$sb.AppendLine("};");

	# Name to type map
	[void]$sb.AppendLine($nameTypeMapDecl);

	foreach($pair in $hlslResources)
	{
		[void]$sb.AppendLine("{`"$($pair[0])`",ShaderTypes::$($pair[1])},");
	}

	foreach ($pair in $hlslScalars)
	{
		AppendSubtypeNames -hlslBaseName $pair[0] -baseEnumName $pair[1]
	}

	foreach ($pair in $hlslLowP)
	{
		AppendSubtypeNames -hlslBaseName $pair[0] -baseEnumName $pair[1]
	}

	[void]$sb.AppendLine("};");

	# Add footer and write to file
	[void]$sb.AppendLine($footer);
	Set-Content -Path $TablePath -Value $sb.ToString()

	Write-Host "Tables written to: $($TablePath)"
}

# Run main
Write-Tables