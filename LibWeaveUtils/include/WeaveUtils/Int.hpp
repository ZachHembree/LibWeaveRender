#pragma once
#include <cstdint>
#include <limits>

namespace Weave
{
	typedef uint8_t byte;
	typedef uint16_t ushort;
	typedef uint32_t uint;
	typedef uint64_t ulong;
	typedef int8_t sbyte;
	typedef int16_t sshort;
	typedef int32_t sint;
	typedef int64_t slong;

	/// <summary>
	/// Maximum value of a 16bit unisgned integer
	/// </summary>
	static constexpr ushort g_UInt16Max = std::numeric_limits<ushort>().max();

	/// <summary>
	/// An unsigned 16bit integer with only the most significant bit set
	/// </summary>
	static constexpr ushort g_Bit16 = 1u << 15u;

	/// <summary>
	/// An unsigned 16bit integer commonly used to indicate an invalid unique identifier
	/// </summary>
	static constexpr ushort g_InvalidID16 = g_UInt16Max;

	/// <summary>
	/// Maximum value of a 32bit unisgned integer
	/// </summary>
	static constexpr uint g_UInt32Max = std::numeric_limits<uint>().max();

	/// <summary>
	/// An unsigned 32bit integer with only the most significant bit set
	/// </summary>
	static constexpr uint g_Bit32 = 1u << 31u;

	/// <summary>
	/// An unsigned 32bit integer commonly used to indicate an invalid unique identifier
	/// </summary>
	static constexpr uint g_InvalidID32 = g_UInt32Max;

	/// <summary>
	/// Maximum value of a 64bit unisgned integer
	/// </summary>
	static constexpr ulong g_UInt64Max = std::numeric_limits<ulong>().max();

	/// <summary>
	/// An unsigned 64bit integer with only the most significant bit set
	/// </summary>
	static constexpr ulong g_Bit64 = 1ull << 63ull;

	/// <summary>
	/// An unsigned 64bit integer commonly used to indicate an invalid unique identifier
	/// </summary>
	static constexpr ulong g_InvalidID64 = g_UInt64Max;
}