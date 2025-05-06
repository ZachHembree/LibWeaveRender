#pragma once
#include "WeaveUtils/GlobalUtils.hpp"

enum D3D11_USAGE;
enum D3D11_BIND_FLAG;
enum D3D11_CPU_ACCESS_FLAG;
enum D3D11_CLEAR_FLAG;
enum D3D11_INPUT_CLASSIFICATION;
enum DXGI_FORMAT;
enum D3D11_FILTER;
enum D3D11_TEXTURE_ADDRESS_MODE;
enum D3D11_COMPARISON_FUNC;
enum D3D_PRIMITIVE_TOPOLOGY;

namespace Weave::D3D11
{
	template <std::integral IntT>
	struct PseudoEnum
	{
		IntT value;

		PseudoEnum(const PseudoEnum&) = default;
		PseudoEnum& operator=(const PseudoEnum&) = default;
		PseudoEnum(PseudoEnum&&) noexcept = default;
		PseudoEnum& operator=(PseudoEnum&&) noexcept = default;
		bool operator==(const PseudoEnum& rhs) const = default;
		bool operator!=(const PseudoEnum& rhs) const = default;

		constexpr PseudoEnum(IntT data = 0) : value(data) {}

		constexpr explicit operator IntT() const { return value; };

		// Bitwise AND
		friend constexpr IntT operator&(const PseudoEnum& a, const PseudoEnum& b) { return (a.value & b.value); }

		friend constexpr PseudoEnum& operator&=(PseudoEnum& a, const PseudoEnum& b)
		{
			a.value = (a.value & b.value);
			return a;
		}

		// Bitwise OR
		friend constexpr IntT operator|(const PseudoEnum& a, const PseudoEnum& b) { return (a.value | b.value); }

		friend constexpr PseudoEnum& operator|=(PseudoEnum& a, const PseudoEnum& b)
		{
			a.value = (a.value | b.value);
			return a;
		}

		// Bitwise Negation
		friend constexpr IntT operator~(const PseudoEnum& a) { return ~(a.value); }

		// Left Shift
		template <std::integral U>
		friend constexpr IntT operator<<(const PseudoEnum& a, U b) { return (a.value << b); }

		template <std::integral U>
		friend constexpr PseudoEnum& operator<<=(PseudoEnum& a, U b)
		{
			a.value = (a.value << b);
			return a;
		}

		// Right Shift
		template <std::integral U>
		friend constexpr IntT operator>>(const PseudoEnum& a, U b) { return (a.value >> b); }

		template <std::integral U>
		friend constexpr PseudoEnum& operator>>=(PseudoEnum& a, U b)
		{
			a.value = (a.value >> b);
			return a;
		}
	};

	template <EnumType EnumT>
	struct EnumAlias : public PseudoEnum<std::underlying_type_t<EnumT>>
	{
		using IntT = std::underlying_type_t<EnumT>;
		using PseudoEnum<IntT>::PseudoEnum;

		constexpr EnumAlias(EnumT value) : PseudoEnum<IntT>(static_cast<IntT>(value)) {}

		constexpr explicit operator EnumT() const { return static_cast<EnumT>(this->value); };
	};

	/// <summary>
	/// Generic enum name lookup helper template
	/// </summary>
	template<EnumType EnumT>
	inline static string_view GetEnumName(const EnumT& value, const std::unordered_map<EnumT, string_view>& nameMap)
	{
		const auto& it = nameMap.find(value);

		if (it != nameMap.end())
			return it->second;
		else
			return "Unknown Enum";
	}

	/// <summary>
	/// Generic enum name lookup helper template
	/// </summary>
	template<std::integral IntT>
	inline static string_view GetEnumName(const PseudoEnum<IntT>& value, const std::unordered_map<PseudoEnum<IntT>, string_view>& nameMap)
	{
		const auto& it = nameMap.find(value);

		if (it != nameMap.end())
			return it->second;
		else
			return "Unknown Enum";
	}

	/// <summary>
	/// Generic enum name lookup helper template
	/// </summary>
	template <EnumType EnumT>
	inline static string_view GetEnumName(const EnumAlias<EnumT>& value, const std::unordered_map<EnumAlias<EnumT>, string_view>& nameMap)
	{
		const auto& it = nameMap.find(value);

		if (it != nameMap.end())
			return it->second;
		else
			return "Unknown Enum";
	}
}

namespace std
{
	using namespace Weave::D3D11;

	template<std::integral IntT>
	struct hash<PseudoEnum<IntT>>
	{
		size_t operator()(const PseudoEnum<IntT>& enm) const noexcept { return std::hash<IntT>{}(enm.value); }
	};

	template <EnumType EnumT>
	struct hash<EnumAlias<EnumT>>
	{
		size_t operator()(const EnumAlias<EnumT>& enm) const noexcept { return std::hash<EnumAlias<EnumT>::IntT>{}(enm.value); }
	};
}

namespace Weave::D3D11
{
	/// <summary>
	/// Determines how vertex topology is interpreted by the input assembler
	/// </summary>
	struct PrimTopology : EnumAlias<D3D_PRIMITIVE_TOPOLOGY>
	{
		using EnumAlias::EnumAlias;

		static const PrimTopology UNDEFINED;
		static const PrimTopology POINTLIST;
		static const PrimTopology LINELIST;
		static const PrimTopology LINESTRIP;
		static const PrimTopology TRIANGLELIST;
		static const PrimTopology TRIANGLESTRIP;
		static const PrimTopology LINELIST_ADJ;
		static const PrimTopology LINESTRIP_ADJ;
		static const PrimTopology TRIANGLELIST_ADJ;
		static const PrimTopology TRIANGLESTRIP_ADJ;
		static const PrimTopology CP1_PATCHLIST;
		static const PrimTopology CP2_PATCHLIST;
		static const PrimTopology CP3_PATCHLIST;
		static const PrimTopology CP4_PATCHLIST;
		static const PrimTopology CP5_PATCHLIST;
		static const PrimTopology CP6_PATCHLIST;
		static const PrimTopology CP7_PATCHLIST;
		static const PrimTopology CP8_PATCHLIST;
		static const PrimTopology CP9_PATCHLIST;
		static const PrimTopology CP10_PATCHLIST;
		static const PrimTopology CP11_PATCHLIST;
		static const PrimTopology CP12_PATCHLIST;
		static const PrimTopology CP13_PATCHLIST;
		static const PrimTopology CP14_PATCHLIST;
		static const PrimTopology CP15_PATCHLIST;
		static const PrimTopology CP16_PATCHLIST;
		static const PrimTopology CP17_PATCHLIST;
		static const PrimTopology CP18_PATCHLIST;
		static const PrimTopology CP19_PATCHLIST;
		static const PrimTopology CP20_PATCHLIST;
		static const PrimTopology CP21_PATCHLIST;
		static const PrimTopology CP22_PATCHLIST;
		static const PrimTopology CP23_PATCHLIST;
		static const PrimTopology CP24_PATCHLIST;
		static const PrimTopology CP25_PATCHLIST;
		static const PrimTopology CP26_PATCHLIST;
		static const PrimTopology CP27_PATCHLIST;
		static const PrimTopology CP28_PATCHLIST;
		static const PrimTopology CP29_PATCHLIST;
		static const PrimTopology CP30_PATCHLIST;
		static const PrimTopology CP31_PATCHLIST;
		static const PrimTopology CP32_PATCHLIST;
	};

	/// <summary>
	/// Specifies how a buffer will be used
	/// </summary>
	struct ResourceUsages : public EnumAlias<D3D11_USAGE>
	{
		using EnumAlias::EnumAlias;

		/// <summary>
		/// R/W Access required for GPU
		/// </summary>
		static const ResourceUsages Default;

		/// <summary>
		/// Read-only GPU resource. Cannot be accessed by CPU.
		/// </summary>
		static const ResourceUsages Immutable;

		/// <summary>
		/// Read-only GPU access; write-only CPU access.
		/// </summary>
		static const ResourceUsages Dynamic;

		/// <summary>
		/// Resource supports transfer from GPU to CPU
		/// </summary>
		static const ResourceUsages Staging;
	};

	/// <summary>
	/// Supported buffer types
	/// </summary>
	struct ResourceBindFlags : public EnumAlias<D3D11_BIND_FLAG>
	{
		using EnumAlias::EnumAlias;

		static const ResourceBindFlags None;
		static const ResourceBindFlags Vertex;
		static const ResourceBindFlags Index;
		static const ResourceBindFlags Constant;
		static const ResourceBindFlags ShaderResource;
		static const ResourceBindFlags StreamOutput;
		static const ResourceBindFlags RenderTarget;
		static const ResourceBindFlags DepthStencil;
		static const ResourceBindFlags UnorderedAccess;
		static const ResourceBindFlags Decoder;
		static const ResourceBindFlags VideoEncoder;
		static const ResourceBindFlags RWTexture;
	};

	/// <summary>
	/// Specifies types of CPU access allowed for mappable resources
	/// </summary>
	struct ResourceAccessFlags : public EnumAlias<D3D11_CPU_ACCESS_FLAG>
	{
		using EnumAlias::EnumAlias;

		static const ResourceAccessFlags None;
		static const ResourceAccessFlags Write;
		static const ResourceAccessFlags Read;
		static const ResourceAccessFlags ReadWrite;
	};

	struct DSClearFlags : public EnumAlias<D3D11_CLEAR_FLAG>
	{
		using EnumAlias::EnumAlias;

		static const DSClearFlags None;
		static const DSClearFlags Depth;
		static const DSClearFlags Stencil;
		static const DSClearFlags DepthStencil;
	};

	/// <summary>
	/// Indicates the type of data in an input slot. Per-instance or per-vertex.
	/// </summary>
	struct InputClasses : public EnumAlias<D3D11_INPUT_CLASSIFICATION>
	{
		using EnumAlias::EnumAlias;

		static const InputClasses PerVertex;
		static const InputClasses PerInstance;
	};

	string_view GetInputClassName(InputClasses inputClass);

	/// <summary>
	/// Resource data formats
	/// </summary>
	struct Formats : public EnumAlias<DXGI_FORMAT>
	{
		using EnumAlias::EnumAlias;

		static const Formats UNKNOWN;
		static const Formats R32G32B32A32_TYPELESS;
		static const Formats R32G32B32A32_FLOAT;
		static const Formats R32G32B32A32_UINT;
		static const Formats R32G32B32A32_SINT;
		static const Formats R32G32B32_TYPELESS;
		static const Formats R32G32B32_FLOAT;
		static const Formats R32G32B32_UINT;
		static const Formats R32G32B32_SINT;
		static const Formats R16G16B16A16_TYPELESS;

		/// <summary>
		/// Default HDR
		/// </summary>
		static const Formats R16G16B16A16_FLOAT;
		static const Formats R16G16B16A16_UNORM;
		static const Formats R16G16B16A16_UINT;
		static const Formats R16G16B16A16_SNORM;
		static const Formats R16G16B16A16_SINT;
		static const Formats R32G32_TYPELESS;
		static const Formats R32G32_FLOAT;
		static const Formats R32G32_UINT;
		static const Formats R32G32_SINT;
		static const Formats R32G8X24_TYPELESS;
		static const Formats D32_FLOAT_S8X24_UINT;
		static const Formats R32_FLOAT_X8X24_TYPELESS;
		static const Formats X32_TYPELESS_G8X24_UINT;
		static const Formats R10G10B10A2_TYPELESS;
		static const Formats R10G10B10A2_UNORM;
		static const Formats R10G10B10A2_UINT;
		static const Formats R11G11B10_FLOAT;
		static const Formats R8G8B8A8_TYPELESS;

		/// <summary>
		/// Default SDR
		/// </summary>
		static const Formats R8G8B8A8_UNORM;

		static const Formats R8G8B8A8_UNORM_SRGB;
		static const Formats R8G8B8A8_UINT;
		static const Formats R8G8B8A8_SNORM;
		static const Formats R8G8B8A8_SINT;
		static const Formats R16G16_TYPELESS;
		static const Formats R16G16_FLOAT;
		static const Formats R16G16_UNORM;
		static const Formats R16G16_UINT;
		static const Formats R16G16_SNORM;
		static const Formats R16G16_SINT;
		static const Formats R32_TYPELESS;
		static const Formats D32_FLOAT;
		static const Formats R32_FLOAT;
		static const Formats R32_UINT;
		static const Formats R32_SINT;
		static const Formats R24G8_TYPELESS;
		static const Formats D24_UNORM_S8_UINT;
		static const Formats R24_UNORM_X8_TYPELESS;
		static const Formats X24_TYPELESS_G8_UINT;
		static const Formats R8G8_TYPELESS;
		static const Formats R8G8_UNORM;
		static const Formats R8G8_UINT;
		static const Formats R8G8_SNORM;
		static const Formats R8G8_SINT;
		static const Formats R16_TYPELESS;
		static const Formats R16_FLOAT;
		static const Formats D16_UNORM;
		static const Formats R16_UNORM;
		static const Formats R16_UINT;
		static const Formats R16_SNORM;
		static const Formats R16_SINT;
		static const Formats R8_TYPELESS;
		static const Formats R8_UNORM;
		static const Formats R8_UINT;
		static const Formats R8_SNORM;
		static const Formats R8_SINT;
		static const Formats A8_UNORM;
		static const Formats R1_UNORM;
		static const Formats R9G9B9E5_SHAREDEXP;
		static const Formats R8G8_B8G8_UNORM;
		static const Formats G8R8_G8B8_UNORM;
		static const Formats BC1_TYPELESS;
		static const Formats BC1_UNORM;
		static const Formats BC1_UNORM_SRGB;
		static const Formats BC2_TYPELESS;
		static const Formats BC2_UNORM;
		static const Formats BC2_UNORM_SRGB;
		static const Formats BC3_TYPELESS;
		static const Formats BC3_UNORM;
		static const Formats BC3_UNORM_SRGB;
		static const Formats BC4_TYPELESS;
		static const Formats BC4_UNORM;
		static const Formats BC4_SNORM;
		static const Formats BC5_TYPELESS;
		static const Formats BC5_UNORM;
		static const Formats BC5_SNORM;
		static const Formats B5G6R5_UNORM;
		static const Formats B5G5R5A1_UNORM;
		static const Formats B8G8R8A8_UNORM;
		static const Formats B8G8R8X8_UNORM;
		static const Formats R10G10B10_XR_BIAS_A2_UNORM;
		static const Formats B8G8R8A8_TYPELESS;
		static const Formats B8G8R8A8_UNORM_SRGB;
		static const Formats B8G8R8X8_TYPELESS;
		static const Formats B8G8R8X8_UNORM_SRGB;
		static const Formats BC6H_TYPELESS;
		static const Formats BC6H_UF16;
		static const Formats BC6H_SF16;
		static const Formats BC7_TYPELESS;
		static const Formats BC7_UNORM;
		static const Formats BC7_UNORM_SRGB;
		static const Formats AYUV;
		static const Formats Y410;
		static const Formats Y416;
		static const Formats NV12;
		static const Formats P010;
		static const Formats P016;
		static const Formats OPAQUE_420;
		static const Formats YUY2;
		static const Formats Y210;
		static const Formats Y216;
		static const Formats NV11;
		static const Formats AI44;
		static const Formats IA44;
		static const Formats P8;
		static const Formats A8P8;
		static const Formats B4G4R4A4_UNORM;
		static const Formats P208;
		static const Formats V208;
		static const Formats V408;
	};

	string_view GetFormatName(Formats format);

	/// <summary>
	/// Determines filter mode for minification/magnification on all three potential axes of a texture (U/V/W)
	/// </summary>
	struct TexFilterModes : public EnumAlias<D3D11_FILTER>
	{
		using EnumAlias::EnumAlias;

		static const TexFilterModes POINT;
		static const TexFilterModes MIN_MAG_POINT_MIP_LINEAR;
		static const TexFilterModes MIN_POINT_MAG_LINEAR_MIP_POINT;
		static const TexFilterModes MIN_POINT_MAG_MIP_LINEAR;
		static const TexFilterModes MIN_LINEAR_MAG_MIP_POINT;
		static const TexFilterModes MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		static const TexFilterModes MIN_MAG_LINEAR_MIP_POINT;
		static const TexFilterModes LINEAR;
		static const TexFilterModes ANISOTROPIC;
		static const TexFilterModes COMPARISON_POINT;
		static const TexFilterModes COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
		static const TexFilterModes COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
		static const TexFilterModes COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
		static const TexFilterModes COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
		static const TexFilterModes COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		static const TexFilterModes COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		static const TexFilterModes COMPARISON_LINEAR;
		static const TexFilterModes COMPARISON_ANISOTROPIC;
		static const TexFilterModes MINIMUM_POINT;
		static const TexFilterModes MINIMUM_MIN_MAG_POINT_MIP_LINEAR;
		static const TexFilterModes MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
		static const TexFilterModes MINIMUM_MIN_POINT_MAG_MIP_LINEAR;
		static const TexFilterModes MINIMUM_MIN_LINEAR_MAG_MIP_POINT;
		static const TexFilterModes MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		static const TexFilterModes MINIMUM_MIN_MAG_LINEAR_MIP_POINT;
		static const TexFilterModes MINIMUM_LINEAR;
		static const TexFilterModes MINIMUM_ANISOTROPIC;
		static const TexFilterModes MAXIMUM_POINT;
		static const TexFilterModes MAXIMUM_MIN_MAG_POINT_MIP_LINEAR;
		static const TexFilterModes MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
		static const TexFilterModes MAXIMUM_MIN_POINT_MAG_MIP_LINEAR;
		static const TexFilterModes MAXIMUM_MIN_LINEAR_MAG_MIP_POINT;
		static const TexFilterModes MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		static const TexFilterModes MAXIMUM_MIN_MAG_LINEAR_MIP_POINT;
		static const TexFilterModes MAXIMUM_LINEAR;
		static const TexFilterModes MAXIMUM_ANISOTROPIC;
	};

	string_view GetFilterModeName(TexFilterModes mode);

	/// <summary>
	/// Determines how texture sampling is/isn't bounded
	/// </summary>
	struct TexClampModes : public EnumAlias<D3D11_TEXTURE_ADDRESS_MODE>
	{
		using EnumAlias::EnumAlias;

		static const TexClampModes WRAP;
		static const TexClampModes MIRROR;
		static const TexClampModes CLAMP;
		static const TexClampModes BORDER;
		static const TexClampModes MIRROR_ONCE;
	};

	string_view GetTexClampModeName(TexClampModes mode);

	/// <summary>
	/// Comparison functions used to determine when new (source) data is to be
	/// stored in a given destination. Typically used in depth-stencil buffers.
	/// </summary>
	struct TexCmpFuncs : public EnumAlias<D3D11_COMPARISON_FUNC>
	{
		using EnumAlias::EnumAlias;

		static const TexCmpFuncs NEVER;
		static const TexCmpFuncs LESS;
		static const TexCmpFuncs EQUAL;
		static const TexCmpFuncs LESS_EQUAL;
		static const TexCmpFuncs GREATER;
		static const TexCmpFuncs NOT_EQUAL;
		static const TexCmpFuncs GREATER_EQUAL;
		static const TexCmpFuncs ALWAYS;
	};

	string_view GetTexCmpFuncName(TexCmpFuncs func);
}