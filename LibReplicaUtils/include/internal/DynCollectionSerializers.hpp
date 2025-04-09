#pragma once
#include <concepts>
#include <cereal/cereal.hpp>
#include "ReplicaUtils/Utils.hpp"

namespace Replica
{
	// Constraints
	// Non-boolean arithmetic values that can be written as raw binary to the archive
	template <class Archive, typename T>
	concept IsBinarySerializableArrNB = requires
	{
		requires std::is_arithmetic<T>::value && !std::is_same<T, bool>::value;
		requires cereal::traits::is_output_serializable<cereal::BinaryData<T>, Archive>::value;
	};

	// Non-boolean types that either aren't writable as raw-binary or aren't arithmetic values
	template <class Archive, typename T>
	concept IsSerializableArrNB = requires
	{
		requires (!std::is_same<T, bool>::value);
		requires (!cereal::traits::is_output_serializable<cereal::BinaryData<T>, Archive>::value || !std::is_arithmetic<T>::value);
	};

	// Non-boolean arithmetic value that can be read as raw binary
	template <class Archive, typename T>
	concept IsBinaryDeserializableArrNB = requires
	{
		requires std::is_arithmetic<T>::value && !std::is_same<T, bool>::value;
		requires cereal::traits::is_input_serializable<cereal::BinaryData<T>, Archive>::value;
	};

	// Non-boolean value that either can't be read as binary or is non-arithmetic
	template <class Archive, typename T>
	concept IsDeserializableArrNB = requires
	{
		requires !std::is_same<T, bool>::value;
		requires !cereal::traits::is_input_serializable<cereal::BinaryData<T>, Archive>::value || !std::is_arithmetic<T>::value;
	};

	// Serializers
	// Non-boolean, binary write
	template <class Archive, typename T>
	requires IsBinarySerializableArrNB<Archive, T>
	inline void save(Archive& ar, const IDynamicArray<T>& src)
	{
		// Write array size tag
		ar(cereal::make_size_tag( static_cast<cereal::size_type>(src.GetLength()) ));
		// Output data
		ar(cereal::binary_data(src.GetPtr(), GetArrSize(src)));
	}

	// Non-boolean, non-binary write
	template <class Archive, typename T>
	requires IsSerializableArrNB<Archive, T>
	inline void save(Archive& ar, const IDynamicArray<T>& src)
	{
		// Write array size tag
		ar(cereal::make_size_tag( static_cast<cereal::size_type>(src.GetLength()) ));

		// Output data
		for (const auto& value : src)
			ar(value);
	}

	// Boolean write
	template <class Archive>
	inline void save(Archive& ar, const IDynamicArray<bool>& src)
	{
		// Write array size tag
		ar(cereal::make_size_tag(static_cast<cereal::size_type>(src.GetLength())));

		// Output data
		for (const auto v : src)
			ar(static_cast<bool>(v));
	}

	// Non-boolean raw binary read
	template <class Archive, typename T, typename ArrT>
	requires IsBinaryDeserializableArrNB<Archive, T> && std::derived_from<ArrT, DynamicArray<T>>
	inline void load(Archive& ar, ArrT& dst)
	{ 
		// Get array size
		cereal::size_type length;
		ar(cereal::make_size_tag(length));

		// Allocate destination
		dst = ArrT(length); 
		// Write deserialized data
		ar(cereal::binary_data( dst.GetPtr(), static_cast<cereal::size_type>(length) * sizeof(T) ));
	}

	// Non-boolean, non-binary read
	template <class Archive, typename T, typename ArrT>
	requires IsDeserializableArrNB<Archive, T> && std::derived_from<ArrT, DynamicArray<T>>
	inline void load(Archive& ar, ArrT& dst)
	{ 
		// Get array size
		cereal::size_type length;
		ar(cereal::make_size_tag(length));

		// Allocate destination
		dst = ArrT(length); 

		// Write deserialized data
		for (auto&& value : dst)
			ar(value);
	}

	// Boolean read
	template <class Archive, typename ArrT>
	requires std::derived_from<ArrT, DynamicArray<bool>>
	inline void load(Archive& ar, ArrT& dst)
	{
		// Get array size
		cereal::size_type length;
		ar(cereal::make_size_tag(length));

		// Allocate destination
		dst = ArrT(length);

		// Write deserialized data
		for (auto&& v : dst)
		{
			bool b;
			ar(b);
			v = b;
		}
	}

	// Non-boolean raw binary read
	template <class Archive, typename T, typename ArrT>
	requires IsBinaryDeserializableArrNB<Archive, T>&& std::derived_from<ArrT, Vector<T>>
	inline void load(Archive& ar, ArrT& dst)
	{
		// Get array size
		cereal::size_type length;
		ar(cereal::make_size_tag(length));

		// Allocate destination
		dst = ArrT();
		dst.reserve(length);

		// Write deserialized data
		for (size_t i = 0; i < length; i++)
			ar(dst.emplace_back());
	}

	// Interface templates used by Cereal
	template <class Archive, typename T>
	inline void load(Archive& ar, DynamicArray<T>& dst) { load<Archive, T, DynamicArray<T>>(ar, dst); }

	template <class Archive, typename T>
	inline void load(Archive& ar, UniqueArray<T>& dst) { load<Archive, T, UniqueArray<T>>(ar, dst); }

	template <class Archive, typename T>
	inline void load(Archive& ar, Vector<T>& dst) { load<Archive, T, Vector<T>>(ar, dst); }

	template <class Archive, typename T>
	inline void load(Archive& ar, UniqueVector<T>& dst) { load<Archive, T, UniqueVector<T>>(ar, dst); }
}
