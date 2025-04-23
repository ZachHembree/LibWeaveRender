#pragma once
#include "pch.hpp"
#include <unordered_map>
#include "D3D11/Resources/Formats.hpp"

namespace Weave::D3D11
{
	static const std::unordered_map<InputClass, string_view> s_InputClassNames
	{
		{ InputClass::PerVertex, "PerVertex" },
		{ InputClass::PerInstance, "PerInstance" }
	};

	static const std::unordered_map<Formats, string_view> s_FormatNames
	{
		{ Formats::UNKNOWN, "UNKNOWN" },
		{ Formats::R32G32B32A32_TYPELESS, "R32G32B32A32_TYPELESS" },
		{ Formats::R32G32B32A32_FLOAT, "R32G32B32A32_FLOAT" },
		{ Formats::R32G32B32A32_UINT, "R32G32B32A32_UINT" },
		{ Formats::R32G32B32A32_SINT, "R32G32B32A32_SINT" },
		{ Formats::R32G32B32_TYPELESS, "R32G32B32_TYPELESS" },
		{ Formats::R32G32B32_FLOAT, "R32G32B32_FLOAT" },
		{ Formats::R32G32B32_UINT, "R32G32B32_UINT" },
		{ Formats::R32G32B32_SINT, "R32G32B32_SINT" },
		{ Formats::R16G16B16A16_TYPELESS, "R16G16B16A16_TYPELESS" },
		{ Formats::R16G16B16A16_FLOAT, "R16G16B16A16_FLOAT" },
		{ Formats::R16G16B16A16_UNORM, "R16G16B16A16_UNORM" },
		{ Formats::R16G16B16A16_UINT, "R16G16B16A16_UINT" },
		{ Formats::R16G16B16A16_SNORM, "R16G16B16A16_SNORM" },
		{ Formats::R16G16B16A16_SINT, "R16G16B16A16_SINT" },
		{ Formats::R32G32_TYPELESS, "R32G32_TYPELESS" },
		{ Formats::R32G32_FLOAT, "R32G32_FLOAT" },
		{ Formats::R32G32_UINT, "R32G32_UINT" },
		{ Formats::R32G32_SINT, "R32G32_SINT" },
		{ Formats::R32G8X24_TYPELESS, "R32G8X24_TYPELESS" },
		{ Formats::D32_FLOAT_S8X24_UINT, "D32_FLOAT_S8X24_UINT" },
		{ Formats::R32_FLOAT_X8X24_TYPELESS, "R32_FLOAT_X8X24_TYPELESS" },
		{ Formats::X32_TYPELESS_G8X24_UINT, "X32_TYPELESS_G8X24_UINT" },
		{ Formats::R10G10B10A2_TYPELESS, "R10G10B10A2_TYPELESS" },
		{ Formats::R10G10B10A2_UNORM, "R10G10B10A2_UNORM" },
		{ Formats::R10G10B10A2_UINT, "R10G10B10A2_UINT" },
		{ Formats::R11G11B10_FLOAT, "R11G11B10_FLOAT" },
		{ Formats::R8G8B8A8_TYPELESS, "R8G8B8A8_TYPELESS" },
		{ Formats::R8G8B8A8_UNORM, "R8G8B8A8_UNORM" },
		{ Formats::R8G8B8A8_UNORM_SRGB, "R8G8B8A8_UNORM_SRGB" },
		{ Formats::R8G8B8A8_UINT, "R8G8B8A8_UINT" },
		{ Formats::R8G8B8A8_SNORM, "R8G8B8A8_SNORM" },
		{ Formats::R8G8B8A8_SINT, "R8G8B8A8_SINT" },
		{ Formats::R16G16_TYPELESS, "R16G16_TYPELESS" },
		{ Formats::R16G16_FLOAT, "R16G16_FLOAT" },
		{ Formats::R16G16_UNORM, "R16G16_UNORM" },
		{ Formats::R16G16_UINT, "R16G16_UINT" },
		{ Formats::R16G16_SNORM, "R16G16_SNORM" },
		{ Formats::R16G16_SINT, "R16G16_SINT" },
		{ Formats::R32_TYPELESS, "R32_TYPELESS" },
		{ Formats::D32_FLOAT, "D32_FLOAT" },
		{ Formats::R32_FLOAT, "R32_FLOAT" },
		{ Formats::R32_UINT, "R32_UINT" },
		{ Formats::R32_SINT, "R32_SINT" },
		{ Formats::R24G8_TYPELESS, "R24G8_TYPELESS" },
		{ Formats::D24_UNORM_S8_UINT, "D24_UNORM_S8_UINT" },
		{ Formats::R24_UNORM_X8_TYPELESS, "R24_UNORM_X8_TYPELESS" },
		{ Formats::X24_TYPELESS_G8_UINT, "X24_TYPELESS_G8_UINT" },
		{ Formats::R8G8_TYPELESS, "R8G8_TYPELESS" },
		{ Formats::R8G8_UNORM, "R8G8_UNORM" },
		{ Formats::R8G8_UINT, "R8G8_UINT" },
		{ Formats::R8G8_SNORM, "R8G8_SNORM" },
		{ Formats::R8G8_SINT, "R8G8_SINT" },
		{ Formats::R16_TYPELESS, "R16_TYPELESS" },
		{ Formats::R16_FLOAT, "R16_FLOAT" },
		{ Formats::D16_UNORM, "D16_UNORM" },
		{ Formats::R16_UNORM, "R16_UNORM" },
		{ Formats::R16_UINT, "R16_UINT" },
		{ Formats::R16_SNORM, "R16_SNORM" },
		{ Formats::R16_SINT, "R16_SINT" },
		{ Formats::R8_TYPELESS, "R8_TYPELESS" },
		{ Formats::R8_UNORM, "R8_UNORM" },
		{ Formats::R8_UINT, "R8_UINT" },
		{ Formats::R8_SNORM, "R8_SNORM" },
		{ Formats::R8_SINT, "R8_SINT" },
		{ Formats::A8_UNORM, "A8_UNORM" },
		{ Formats::R1_UNORM, "R1_UNORM" },
		{ Formats::R9G9B9E5_SHAREDEXP, "R9G9B9E5_SHAREDEXP" },
		{ Formats::R8G8_B8G8_UNORM, "R8G8_B8G8_UNORM" },
		{ Formats::G8R8_G8B8_UNORM, "G8R8_G8B8_UNORM" },
		{ Formats::BC1_TYPELESS, "BC1_TYPELESS" },
		{ Formats::BC1_UNORM, "BC1_UNORM" },
		{ Formats::BC1_UNORM_SRGB, "BC1_UNORM_SRGB" },
		{ Formats::BC2_TYPELESS, "BC2_TYPELESS" },
		{ Formats::BC2_UNORM, "BC2_UNORM" },
		{ Formats::BC2_UNORM_SRGB, "BC2_UNORM_SRGB" },
		{ Formats::BC3_TYPELESS, "BC3_TYPELESS" },
		{ Formats::BC3_UNORM, "BC3_UNORM" },
		{ Formats::BC3_UNORM_SRGB, "BC3_UNORM_SRGB" },
		{ Formats::BC4_TYPELESS, "BC4_TYPELESS" },
		{ Formats::BC4_UNORM, "BC4_UNORM" },
		{ Formats::BC4_SNORM, "BC4_SNORM" },
		{ Formats::BC5_TYPELESS, "BC5_TYPELESS" },
		{ Formats::BC5_UNORM, "BC5_UNORM" },
		{ Formats::BC5_SNORM, "BC5_SNORM" },
		{ Formats::B5G6R5_UNORM, "B5G6R5_UNORM" },
		{ Formats::B5G5R5A1_UNORM, "B5G5R5A1_UNORM" },
		{ Formats::B8G8R8A8_UNORM, "B8G8R8A8_UNORM" },
		{ Formats::B8G8R8X8_UNORM, "B8G8R8X8_UNORM" },
		{ Formats::R10G10B10_XR_BIAS_A2_UNORM, "R10G10B10_XR_BIAS_A2_UNORM" },
		{ Formats::B8G8R8A8_TYPELESS, "B8G8R8A8_TYPELESS" },
		{ Formats::B8G8R8A8_UNORM_SRGB, "B8G8R8A8_UNORM_SRGB" },
		{ Formats::B8G8R8X8_TYPELESS, "B8G8R8X8_TYPELESS" },
		{ Formats::B8G8R8X8_UNORM_SRGB, "B8G8R8X8_UNORM_SRGB" },
		{ Formats::BC6H_TYPELESS, "BC6H_TYPELESS" },
		{ Formats::BC6H_UF16, "BC6H_UF16" },
		{ Formats::BC6H_SF16, "BC6H_SF16" },
		{ Formats::BC7_TYPELESS, "BC7_TYPELESS" },
		{ Formats::BC7_UNORM, "BC7_UNORM" },
		{ Formats::BC7_UNORM_SRGB, "BC7_UNORM_SRGB" },
		{ Formats::AYUV, "AYUV" },
		{ Formats::Y410, "Y410" },
		{ Formats::Y416, "Y416" },
		{ Formats::NV12, "NV12" },
		{ Formats::P010, "P010" },
		{ Formats::P016, "P016" },
		{ Formats::OPAQUE_420, "OPAQUE_420" },
		{ Formats::YUY2, "YUY2" },
		{ Formats::Y210, "Y210" },
		{ Formats::Y216, "Y216" },
		{ Formats::NV11, "NV11" },
		{ Formats::AI44, "AI44" },
		{ Formats::IA44, "IA44" },
		{ Formats::P8, "P8" },
		{ Formats::A8P8, "A8P8" },
		{ Formats::B4G4R4A4_UNORM, "B4G4R4A4_UNORM" },
		{ Formats::P208, "P208" },
		{ Formats::V208, "V208" },
		{ Formats::V408, "V408" }
	};

	template<typename EnumT>
	static string_view GetEnumName(EnumT value, const std::unordered_map<EnumT, string_view>& nameMap)
	{
		const auto& it = nameMap.find(value);

		if (it != nameMap.end())
			return it->second;
		else
			return "Unknown Enum";
	}

	string_view GetInputClassName(InputClass inputClass) { return GetEnumName(inputClass, s_InputClassNames); }

	string_view GetFormatName(Formats format) { return GetEnumName(format, s_FormatNames); }
}