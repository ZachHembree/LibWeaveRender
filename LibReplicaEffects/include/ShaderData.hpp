#pragma once
#include "ReplicaMath.hpp"
#include "ParseExcept.hpp"
#include "ShaderLibGen/ShaderParser/SymbolEnums.hpp"
#include "ShaderLibGen/ShaderParser/ShaderTypeInfo.hpp"
#include "StringIDMap.hpp"

namespace Replica::Effects
{
	/// <summary>
	/// Defines a shader constant's name, size and position within a cbuf
	/// </summary>
	struct ConstDef
	{
		USE_DEFAULT_CMP(ConstDef)

		uint stringID;
		uint offset;
		uint size;

		ConstDef() :
			offset(0),
			size(0),
			stringID(0)
		{ }

		ConstDef(uint stringID, uint offset, uint size) :
			stringID(stringID),
			offset(offset),
			size(size)
		{ }
	};

	/// <summary>
	/// Array of constant buffer member definitions used in conjunction with corresponding resource
	/// definition to define a constant buffer
	/// </summary>
	struct ConstBufDef
	{
		USE_DEFAULT_CMP(ConstBufDef);

		uint stringID;
		uint size;
		DynamicArray<uint> members;
	};

	/// <summary>
	/// Element describing types used for input and output on a given stage
	/// </summary>
	struct IOElementDef
	{
		USE_DEFAULT_CMP(IOElementDef);

		/// <summary>
		/// Semantic associated with the element
		/// </summary>
		uint semanticID;

		/// <summary>
		/// Optional index modifying the name. Ex: SV_Target2, Color1...
		/// </summary>
		uint semanticIndex;

		/// <summary>
		/// Scalar type
		/// </summary>
		uint dataType;

		/// <summary>
		/// Number of components
		/// </summary>
		uint componentCount;

		/// <summary>
		/// Type size of the element
		/// </summary>
		uint size;

		IOElementDef() :
			semanticID(0),
			semanticIndex(0),
			size(0),
			dataType(0),
			componentCount(0)
		{ }

		IOElementDef(uint semantic, uint semanticIndex, uint dataType, uint componentCount, uint size) :
			semanticID(semantic),
			semanticIndex(semanticIndex),
			dataType(dataType),
			componentCount(componentCount),
			size(size)
		{ }
	};

	/// <summary>
	/// Defines a shader resource, like a texture or sampler
	/// </summary>
	struct ResourceDef
	{
		USE_DEFAULT_CMP(ResourceDef)

		/// <summary>
		/// StringID of the resource declared in the source
		/// </summary>
		uint stringID;

		/// <summary>
		/// Type of resource
		/// </summary>
		ShaderTypes type;

		uint slot;

		ResourceDef() :
			type(ShaderTypes::Void),
			slot(-1),
			stringID(-1)
		{ }

		ResourceDef(uint stringID, ShaderTypes type, uint slot = -1) :
			stringID(stringID),
			type(type),
			slot(slot)
		{ }

		bool GetHasFlags(ShaderTypes flags) const { return (type & flags) == flags; }
	};

	/// <summary>
	/// Generalized shader stage definition
	/// </summary>
	struct ShaderDef
	{
		USE_DEFAULT_CMP(ShaderDef)

		/// <summary>
		/// StringID for the path of the original source
		/// </summary>
		uint fileStringID;

		/// <summary>
		/// Precompiled source binary
		/// </summary>
		DynamicArray<byte> binSrc;

		/// <summary>
		/// StringID corresponding to shader name
		/// </summary>
		uint nameID;

		/// <summary>
		/// Shading stage
		/// </summary>
		ShadeStages stage;

		/// <summary>
		/// Number of threads per group in a compute shader, if applicable
		/// </summary>
		tvec3<uint> threadGroupSize;

		/// <summary>
		/// Handle ID for stage input layout
		/// </summary>
		uint inLayoutID;

		/// <summary>
		/// Handle ID for stage output layout
		/// </summary>
		uint outLayoutID;

		/// <summary>
		/// Handle ID for resource group used
		/// </summary>
		uint resLayoutID;

		/// <summary>
		/// Handle ID for constant buffer group used
		/// </summary>
		uint cbufGroupID;

	};

	struct EffectPass
	{
		USE_DEFAULT_CMP(EffectPass)

		/// <summary>
		/// Shaders used in the pass, in the order they are executed. ShaderStage determined
		/// by ShaderDef referenced by ID.
		/// </summary>
		DynamicArray<uint> shaderIDs;
	};

	struct EffectDef
	{
		USE_DEFAULT_CMP(EffectDef)

		/// <summary>
		/// StringID corresponding to effect name
		/// </summary>
		uint nameID;

		/// <summary>
		/// Effect passes in execution order
		/// </summary>
		DynamicArray<EffectPass> passes;
	};

	struct ShaderVariantDef
	{
		/// <summary>
		/// Unique shader associated with the variant
		/// </summary>
		uint shaderID;

		/// <summary>
		/// Identifier corresponding unique variant configuration
		/// </summary>
		uint variantID;
	};

	struct EffectVariantDef
	{
		/// <summary>
		/// Unique effect associated with the variant
		/// </summary>
		uint effectID;

		/// <summary>
		/// Identifier corresponding unique variant configuration
		/// </summary>
		uint variantID;
	};

	/// <summary>
	/// Serializable library variant definition
	/// </summary>
	struct VariantDef
	{
		/// <summary>
		/// Stores all shaders and effects in the variant
		/// </summary>
		DynamicArray<EffectVariantDef> effects;

		/// <summary>
		/// Stores all shaders and effects in the variant
		/// </summary>
		DynamicArray<ShaderVariantDef> shaders;
	};

	/// <summary>
	/// Serializable definition for all unique data contained within a shader library
	/// </summary>
	struct ShaderRegistryDef
	{
		/// <summary>
		/// Unique strings
		/// </summary>
		StringIDMapDef stringIDs;

		/// <summary>
		/// Unique constant buffer members
		/// </summary>
		DynamicArray<ConstDef> constants;

		/// <summary>
		/// Unique constant buffers
		/// </summary>
		DynamicArray<ConstBufDef> cbufDefs;

		/// <summary>
		/// Unique individual input/output members for a stage
		/// </summary>
		DynamicArray<IOElementDef> ioElements;

		/// <summary>
		/// Unique textures, buffers and samplers
		/// </summary>
		DynamicArray<ResourceDef> resources;

		/// <summary>
		/// Unique groups of constant buffers used in a shader
		/// </summary>
		DynamicArray<DynamicArray<uint>> cbufGroups;

		/// <summary>
		/// Unique stage input/output signatures for a shader
		/// </summary>
		DynamicArray<DynamicArray<uint>> ioSignatures;

		/// <summary>
		/// Unique groups of textures, buffers and samplers used in a shader
		/// </summary>
		DynamicArray<DynamicArray<uint>> resGroups;

		/// <summary>
		/// Unique shaders and associated resources
		/// </summary>
		DynamicArray<ShaderDef> shaders;

		/// <summary>
		/// Unique effects and associated resources
		/// </summary>
		DynamicArray<EffectDef> effects;
	};

	/// <summary>
	/// Supported graphics APIs
	/// </summary>
	enum class PlatformTargets : byte
	{
		DirectX11 = 0,
		DirectX12 = 1,
		Vulkan = 2
	};

	/// <summary>
	/// Describes the platform targeted during compilation
	/// </summary>
	struct PlatformDef
	{
		/// <summary>
		/// Name and version string of the shader compiler used
		/// </summary>
		string compilerVersion;

		/// <summary>
		/// Shader model used for compilation
		/// </summary>
		string shaderModel;

		/// <summary>
		/// Graphics API compiled for
		/// </summary>
		PlatformTargets target;
	};

	/// <summary>
	/// Serializable collection of effect and shader variants compiled from the same
	/// effect file.
	/// </summary>
	struct ShaderLibDef
	{
		/// <summary>
		/// Describes the platform targeted during compilation
		/// </summary>
		PlatformDef platform;

		/// <summary>
		/// Flag names used for static shader variant generation
		/// </summary>
		DynamicArray<string> flagNames;

		/// <summary>
		/// Mutually exclusive shader modes/features used for static shader variant generation
		/// </summary>
		DynamicArray<string> modeNames;

		/// <summary>
		/// Array of shaders and effects for each variant
		/// </summary>
		DynamicArray<VariantDef> variants;

		/// <summary>
		/// Stores a serializable collection of all unique effects, shaders and requisite metadata
		/// required by the library definition.
		/// </summary>
		ShaderRegistryDef regData;
	};

	/// <summary>
	/// Serializable collection of shader libraries compiled for a given platform
	/// </summary>
	struct ShaderRepoDef
	{
		/// <summary>
		/// A collection of independent shader libraries from at least one effect file
		/// </summary>
		DynamicArray<ShaderLibDef> libraries;
	};
}

#include "ShaderLibGen/ShaderDataHashes.hpp"