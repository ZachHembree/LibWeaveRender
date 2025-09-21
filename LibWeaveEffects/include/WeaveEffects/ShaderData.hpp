#pragma once
#include "WeaveUtils/Math.hpp"
#include "WeaveEffects/EffectParseException.hpp"
#include "WeaveUtils/StringIDMap.hpp"
#include "WeaveUtils/VectorSpan.hpp"
#include "WeaveEffects/ShaderLibBuilder/ShaderParser/SymbolEnums.hpp"
#include "WeaveEffects/ShaderLibBuilder/ShaderParser/ShaderTypeInfo.hpp"

namespace Weave::Effects
{
	constexpr uint g_VariantMask = 0xFFFFu;
	constexpr uint g_VariantGroupOffset = 16u;

	/// <summary>
	/// Non-owning span of raw bytes
	/// </summary>
	using ByteSpan = const VectorSpan<Vector<byte>>;

	/// <summary>
	/// Non-owning span of uint IDs
	/// </summary>
	using IDSpan = const VectorSpan<Vector<uint>>;

	template <typename T>
	class SpanVector
	{
	public:
		using value_type = VectorSpan<Vector<T>>;
		using const_value_type = const VectorSpan<Vector<T>>;
		using reference = value_type;
		using const_reference = const_value_type;

		Vector<T> data;
		Vector<uint> spans;

		void SetData(const SpanVector& data)
		{
			Clear();
			this->data.AddRange(data.data);
			this->spans.AddRange(data.spans);
		}

		void SetData(const IDynamicArray<T>& data, const IDynamicArray<uint>& spans)
		{
			Clear();
			this->data.AddRange(data);
			this->spans.AddRange(spans);
		}

		/// <summary>
		/// Copies a new span into the vector
		/// </summary>
		void Add(const IDynamicArray<T>& arr)
		{
			uint index = (uint)data.GetLength();
			uint length = (uint)arr.GetLength();
			WV_ASSERT_MSG(length > 0, "Cannot add empty array");
			spans.Add(index);
			spans.Add(length);
			data.AddRange(arr);
		}

		void RemoveBack()
		{
			WV_ASSERT_MSG(spans.GetLength() >= 2, "No spans to remove");
			uint index = (uint)spans[spans.GetLength() - 2];
			uint length = (uint)spans.GetBack();
			data.RemoveRange(index, length);
			spans.RemoveBack();
			spans.RemoveBack();
		}

		bool IsEmpty() const { return spans.GetLength() == 0; }

		/// <summary>
		/// Clears the contents of the vector
		/// </summary>
		void Clear()
		{
			data.Clear();
			spans.Clear();
		}

		/// <summary>
		/// Returns the number of spans in the vector
		/// </summary>
		size_t GetLength() const { return spans.GetLength() / 2; }

		/// <summary>
		/// Returns the span at the given index
		/// </summary>
		value_type at(size_t index)
		{
			WV_ASSERT_MSG((index * 2 + 1) < spans.GetLength(), "Span index out of range");
			return value_type(data, spans[index * 2], spans[index * 2 + 1]);
		}

		/// <summary>
		/// Returns the span at the given index
		/// </summary>
		const_value_type at(size_t index) const
		{
			WV_ASSERT_MSG((index * 2 + 1) < spans.GetLength(), "Span index out of range");
			return const_value_type(*const_cast<Vector<T>*>(&data), spans[index * 2], spans[index * 2 + 1]);
		}

		/// <summary>
		/// Returns the span at the given index
		/// </summary>
		value_type operator[](size_t index)
		{
			WV_ASSERT_MSG((index * 2 + 1) < spans.GetLength(), "Span index out of range");
			return value_type(data, spans[index * 2], spans[index * 2 + 1]);
		}

		/// <summary>
		/// Returns the span at the given index
		/// </summary>
		const_value_type operator[](size_t index) const
		{
			WV_ASSERT_MSG((index * 2 + 1) < spans.GetLength(), "Span index out of range");
			return const_value_type(*const_cast<Vector<T>*>(&data), spans[index * 2], spans[index * 2 + 1]);
		}
	};

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
		{
		}

		ConstDef(uint stringID, uint offset, uint size) :
			stringID(stringID),
			offset(offset),
			size(size)
		{
		}
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
		{
		}

		IOElementDef(uint semantic, uint semanticIndex, uint dataType, uint componentCount, uint size) :
			semanticID(semantic),
			semanticIndex(semanticIndex),
			dataType(dataType),
			componentCount(componentCount),
			size(size)
		{
		}
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
		{
		}

		ResourceDef(uint stringID, ShaderTypes type, uint slot = -1) :
			stringID(stringID),
			type(type),
			slot(slot)
		{
		}

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
		uint passGroupID;
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
		/// Unique constant buffer members
		/// </summary>
		Vector<ConstDef> constants;

		/// <summary>
		/// Unique constant buffers
		/// </summary>
		Vector<ConstBufDef> cbufDefs;

		/// <summary>
		/// Unique individual input/output members for a stage
		/// </summary>
		Vector<IOElementDef> ioElements;

		/// <summary>
		/// Unique textures, buffers and samplers
		/// </summary>
		Vector<ResourceDef> resources;

		/// <summary>
		/// Unique shaders and associated resources
		/// </summary>
		Vector<ShaderDef> shaders;

		/// <summary>
		/// Unique effects and associated resources
		/// </summary>
		Vector<EffectDef> effects;

		/// <summary>
		/// Groups of unique uint resources identifiers
		/// </summary>
		SpanVector<uint> idGroups;

		/// <summary>
		/// Unique shader binaries
		/// </summary>
		SpanVector<byte> binSpans;

		/// <summary>
		/// Represents a serializable, non-owning view to all unique shader data in the registry
		/// </summary>
		struct Handle
		{
			const IDynamicArray<ConstDef>* pConstants;
			const IDynamicArray<ConstBufDef>* pCBufDefs;
			const IDynamicArray<IOElementDef>* pIOElements;
			const IDynamicArray<ResourceDef>* pResources;
			const IDynamicArray<ShaderDef>* pShaders;
			const IDynamicArray<EffectDef>* pEffects;
			const SpanVector<uint>* pIDGroups;
			const SpanVector<byte>* pBinSpans;

			/// <summary>
			/// Returns a deep copy of the definition data
			/// </summary>
			ShaderRegistryDef GetCopy() const
			{
				return 
				{
					.constants = Vector(*pConstants),
					.cbufDefs = Vector(*pCBufDefs),
					.ioElements = Vector(*pIOElements),
					.resources = Vector(*pResources),
					.shaders = Vector(*pShaders),
					.effects = Vector(*pEffects),
					.idGroups = *pIDGroups,
					.binSpans = *pBinSpans
				};
			}
		};

		/// <summary>
		/// Returns a serializable, non-owning view to all unique shader data in the registry
		/// </summary>
		Handle GetHandle() const
		{
			return
			{
				.pConstants = &constants,
				.pCBufDefs = &cbufDefs,
				.pIOElements = &ioElements,
				.pResources = &resources,
				.pShaders = &shaders,
				.pEffects = &effects,
				.pIDGroups = &idGroups,
				.pBinSpans = &binSpans
			};
		}

		void Clear()
		{
			constants.Clear();
			cbufDefs.Clear();
			ioElements.Clear();
			resources.Clear();
			shaders.Clear();
			effects.Clear();
			idGroups.Clear();
			binSpans.Clear();
		}
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

		/// <summary>
		/// Unique strings
		/// </summary>
		StringIDMapDef stringIDs;

		/// <summary>
		/// Represents a serializable, non-owning view of the definition data
		/// </summary>
		struct Handle
		{
			const PlatformDef* pPlatform;
			const IDynamicArray<VariantRepoDef>* pRepos;
			const ShaderRegistryDef::Handle regHandle;
			const StringIDMapDef::Handle strMapHandle;

			/// <summary>
			/// Returns a deep copy of the definition data
			/// </summary>
			ShaderLibDef GetCopy() const
			{
				return 
				{
					.platform = *pPlatform,
					.repos = DynamicArray(*pRepos),
					.regData = regHandle.GetCopy(),
					.stringIDs = strMapHandle.GetCopy()
				};
			}
		};

		/// <summary>
		/// Returns a serializable, non-owning view of the definition data
		/// </summary>
		Handle GetHandle() const
		{
			return 
			{
				.pPlatform = &platform,
				.pRepos = &repos,
				.regHandle = regData.GetHandle(),
				.strMapHandle = stringIDs.GetHandle()
			};
		}
	};

	/// <summary>
	/// Deserializes a byte array into a ShaderLibDef
	/// </summary>
	ShaderLibDef GetDeserializedLibDef(string_view libData);

	/// <summary>
	/// Deserializes a byte array into a ShaderLibDef
	/// </summary>
	template <std::size_t N>
	constexpr ShaderLibDef GetDeserializedLibDef(const byte(&arr)[N]) noexcept
	{
		return GetDeserializedLibDef(string_view(reinterpret_cast<const char*>(&arr[0]), N));
	}

	/// <summary>
	/// Deserializes a uint64_t / Weave::ulong array into a ShaderLibDef
	/// </summary>
	template <std::size_t N>
	constexpr ShaderLibDef GetDeserializedLibDef(const ulong(&arr)[N]) noexcept
	{
		return GetDeserializedLibDef(string_view(reinterpret_cast<const char*>(&arr[0]), 8 * N));
	}
}

#include "WeaveEffects/ShaderLibBuilder/ShaderDataHashes.hpp"
