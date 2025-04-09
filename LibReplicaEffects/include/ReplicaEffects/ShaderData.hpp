#pragma once
#include "ReplicaUtils/Math.hpp"
#include "ReplicaEffects/ParseExcept.hpp"
#include "ReplicaEffects/ShaderLibBuilder/ShaderParser/SymbolEnums.hpp"
#include "ReplicaEffects/ShaderLibBuilder/ShaderParser/ShaderTypeInfo.hpp"
#include "ReplicaUtils/StringIDMap.hpp"

namespace Replica::Effects
{
	constexpr uint g_VariantMask = 0xFFFFu;
	constexpr uint g_VariantGroupOffset = 16u;

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
		uint layoutID;
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
		/// Precompiled source binary ID
		/// </summary>
		uint byteCodeID;

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
		DynamicArray<uint> passes;
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
		/// Unique constant buffer layout combinations
		/// </summary>
		DynamicArray<DynamicArray<uint>> cbufLayouts;

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
		/// Unique IDs representing shaders in an effect pass
		/// </summary>
		DynamicArray<DynamicArray<uint>> effectPasses;

		/// <summary>
		/// Shader bytecode buffers
		/// </summary>
		DynamicArray<DynamicArray<byte>> binaries;

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
		string featureLevel;

		/// <summary>
		/// Graphics API compiled for
		/// </summary>
		PlatformTargets target;
	};

	struct VariantRepoSrc
	{
		/// <summary>
		/// Name of the repo
		/// </summary>
		string name;

		/// <summary>
		/// File the library was compiled from
		/// </summary>
		string path;
	};

	/// <summary>
	/// Serializable collection of effect and shader variants compiled from the same
	/// effect file.
	/// </summary>
	struct VariantRepoDef
	{
		/// <summary>
		/// Source file and name used to create the repo
		/// </summary>
		VariantRepoSrc src;

		/// <summary>
		/// Flag names used for static shader variant generation
		/// </summary>
		DynamicArray<uint> flagIDs;

		/// <summary>
		/// Mutually exclusive shader modes/features used for static shader variant generation
		/// </summary>
		DynamicArray<uint> modeIDs;

		/// <summary>
		/// Array of shaders and effects for each variant
		/// </summary>
		DynamicArray<VariantDef> variants;
	};

	/// <summary>
	/// A collection of independent shader libraries from at least one effect file
	/// </summary>
	struct ShaderLibSrc
	{
		DynamicArray<VariantRepoSrc> srcFiles;
	};

	/// <summary>
	/// Serializable collection of shaders and their variants from one or more source files
	/// </summary>
	struct ShaderLibDef
	{
		/// <summary>
		/// Describes the platform targeted during compilation
		/// </summary>
		PlatformDef platform;

		/// <summary>
		/// A collection of independent shader libraries from at least one effect file
		/// </summary>
		DynamicArray<VariantRepoDef> repos;

		/// <summary>
		/// Stores a serializable collection of all unique effects, shaders and requisite metadata
		/// required by library definitions
		/// </summary>
		ShaderRegistryDef regData;
	};
}

#include "ReplicaEffects/ShaderLibBuilder/ShaderDataHashes.hpp"