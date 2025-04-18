#pragma once
#include "internal/DeltaUtils.hpp"
#include "DynamicCollections.hpp"
#include <array>

// Allocates temporary stack allocated array with alloca, and wraps it in a span
#define ALLOCA_SPAN(SPAN, COUNT, TYPE) \
	ALLOCA_ARR(SPAN##_TMP_PTR, COUNT, TYPE) \
	Span<TYPE> SPAN(SPAN##_TMP_PTR, COUNT);
// Allocates temporary stack allocated array with alloca, fills it with nulls, and wraps it in a span
#define ALLOCA_SPAN_NULL(SPAN, COUNT, TYPE) \
	ALLOCA_ARR_NULL(SPAN##_TMP_PTR, COUNT, TYPE) \
	Span<TYPE> SPAN(SPAN##_TMP_PTR, COUNT);

namespace Weave
{ 
	/// <summary>
	/// Represents a contiguous section of a fixed length array
	/// </summary>
	template<typename T>
	class Span : public IDynamicArray<T>
	{
	public:
		DEF_DYN_ARR_TRAITS(IDynamicArray<T>)

		Span() : 
			pStart(nullptr), length(0)
		{ }

		explicit Span(T* pStart) :
			pStart(pStart), length(1)
		{ }

		Span(T* pStart, size_t length) : 
			pStart(pStart), length(length)
		{ }

		Span(T* pStart, const T* pEnd) :
			pStart(pStart), 
			length(UnsignedDelta(pEnd + 1, pStart) / sizeof(T))
		{ }

		Span(T* pMain, size_t offset, size_t length) :
			pStart(pMain + offset),
			length(length)
		{ }

		template<size_t N>
		Span(std::array<T, N>& arr) :
			pStart(arr.data()), 
			length(arr.size())
		{ }

		Span(const Span& other) = default;

		Span(Span&& other) = default;

		~Span() = default;

		Span& operator=(const Span& other) = default;

		Span& operator=(Span&& other) = default;

		/// <summary>
		/// Returns the length of the span
		/// </summary>
		size_t GetLength() const override { return length; }

		/// <summary>
		/// Returns a copy of the pointer to the backing the array.
		/// </summary>
		T* GetData() override { return pStart; }

		/// <summary>
		/// Returns a const copy of the pointer to the backing the array.
		/// </summary>
		const T* GetData() const override { return pStart; }

		/// <summary>
		/// Returns iterator pointing to the start of the collection
		/// </summary>
		iterator begin() override { return iterator(pStart); }

		/// <summary>
		/// Returns iterator pointing to the end of the collection
		/// </summary>
		iterator end() override { return iterator(pStart + length); }

		/// <summary>
		/// Returns iterator pointing to the start of the collection
		/// </summary>
		const_iterator begin() const override { return const_iterator(pStart); }

		/// <summary>
		/// Returns iterator pointing to the end of the collection
		/// </summary>
		const_iterator end() const override { return const_iterator(pStart + length); }

		/// <summary>
		/// Provides indexed access to array member references.
		/// </summary>
		T& operator[](size_t index) override
		{
#if _CONTAINER_DEBUG_LEVEL > 0
			if (index >= length)
			{
				char buffer[100];
				sprintf_s(buffer, 100, "Array index out of range. Index: %tu, Length %tu", index, length);

				throw std::exception(buffer);
			}
#endif
			return pStart[index];
		}

		/// <summary>
		/// Provides indexed access to array members using constant references.
		/// </summary>
		const T& operator[](size_t index) const override
		{
#if _CONTAINER_DEBUG_LEVEL > 0
			if (index >= length)
			{
				char buffer[100];
				sprintf_s(buffer, 100, "Array index out of range. Index: %tu, Length %tu", index, length);

				throw std::exception(buffer);
			}
#endif
			return pStart[index];
		}

		protected:
			T* pStart;
			size_t length;
	};
}

namespace std
{
	template<typename T>
	struct hash<Weave::Span<T>>
	{
		size_t operator()(const Weave::Span<T>& arr) const noexcept { return arr.GetHash(); }
	};
}