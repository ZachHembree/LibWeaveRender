#pragma once
#include "ReplicaMath.hpp"
#include "SymbolEnums.hpp"
#include "ShaderTypeInfo.hpp"
#include "ParseExcept.hpp"

namespace Replica::Effects
{
	/// <summary>
	/// Defines a shader constant's name, size and position within a cbuf
	/// </summary>
	struct ConstDef
	{
		string name;
		size_t offset;
		size_t size;

		ConstDef() :
			offset(0),
			size(0)
		{ }

		ConstDef(string_view name, size_t offset, size_t size) :
			name(name),
			offset(offset),
			size(size)
		{ }
	};

	/// <summary>
	/// Array of constant buffer member definitions used in conjunction with corresponding resource
	/// definition to define a constant buffer
	/// </summary>
	typedef UniqueArray<ConstDef> ConstBufLayout;

	/// <summary>
	/// Element describing types used for input and output on a given stage
	/// </summary>
	struct IOElementDef
	{
		/// <summary>
		/// Semantic associated with the element
		/// </summary>
		string semanticName;

		/// <summary>
		/// Optional index modifying the name. Ex: SV_Target2, Color1...
		/// </summary>
		uint semanticIndex;

		/// <summary>
		/// Type size of the element
		/// </summary>
		size_t size;

		IOElementDef() :
			semanticIndex(0),
			size(0)
		{ }

		IOElementDef(string_view semantic, uint semanticIndex, size_t size) :
			semanticName(semantic),
			semanticIndex(semanticIndex),
			size(size)
		{ }
	};

	/// <summary>
	/// Defines the layout of data used for input/output to shader stages
	/// </summary>
	typedef UniqueArray<IOElementDef> IOLayoutDef;

	/// <summary>
	/// Defines a shader resource, like a cbuf, texture or sampler
	/// </summary>
	struct ResourceDef
	{
		/// <summary>
		/// Name of the resource declared in the source
		/// </summary>
		string name;

		/// <summary>
		/// Type of resource
		/// </summary>
		ShaderTypes type;

		int slot;

		ResourceDef() :
			type(ShaderTypes::Void),
			slot(-1)
		{ }

		ResourceDef(string_view name, ShaderTypes type, int slot = -1) :
			name(name),
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
		MAKE_MOVE_ONLY(ShaderDef)

		/// <summary>
		/// File path of original source
		/// </summary>
		string fileName;

		/// <summary>
		/// Precompiled source binary
		/// </summary>
		UniqueArray<byte> binSrc;

		/// <summary>
		/// Main function name
		/// </summary>
		string name;

		/// <summary>
		/// Shading stage
		/// </summary>
		ShadeStages stage;

		/// <summary>
		/// Number of threads per group in a compute shader, if applicable
		/// </summary>
		tvec3<uint> threadGroupSize;

		/// <summary>
		/// Stage input
		/// </summary>
		IOLayoutDef inLayout;

		/// <summary>
		/// Stage output (return)
		/// </summary>
		IOLayoutDef outLayout;

		/// <summary>
		/// Stage resources
		/// </summary>
		UniqueArray<ResourceDef> res;

		/// <summary>
		/// Constant buffer resource subtype data
		/// </summary>
		UniqueArray<ConstBufLayout> constBufs;

		/// <summary>
		/// Identifier corresponding unique variant configuration
		/// </summary>
		uint variantID;

		ShaderDef() :
			stage(ShadeStages::Unknown),
			threadGroupSize(0, 0, 0),
			variantID(0)
		{ }
	};

	struct EffectPass
	{
		/// <summary>
		/// Shaders used in the pass, in the order they are executed. ShaderStage determined
		/// by ShaderDef referenced by ID.
		/// </summary>
		UniqueArray<int> shaderIDs;
	};

	struct EffectDef
	{
		/// <summary>
		/// Name of the effect
		/// </summary>
		string name;
		
		/// <summary>
		/// Effect passes in execution order
		/// </summary>
		UniqueArray<EffectPass> passes;

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
		/// Stores all effects and effect variants in the library
		/// </summary>
		DynamicArray<EffectDef> effects;

		/// <summary>
		/// Stores all shaders and shader variants in the library
		/// </summary>
		DynamicArray<ShaderDef> shaders;
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