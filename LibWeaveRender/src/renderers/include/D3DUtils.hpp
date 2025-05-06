#pragma once
#include "WeaveUtils/WinUtils.hpp"
#include "D3DException.hpp"

namespace Weave
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
    template<std::integral IntT>
    struct hash<Weave::PseudoEnum<IntT>>
    {
        size_t operator()(const Weave::PseudoEnum<IntT>& enm) const noexcept { return std::hash<IntT>{}(enm.value); }
    };

    template <EnumType EnumT>
    struct hash<Weave::EnumAlias<EnumT>>
    {
        size_t operator()(const Weave::EnumAlias<EnumT>& enm) const noexcept { return std::hash<Weave::EnumAlias<EnumT>::IntT>{}(enm.value); }
    };
}
