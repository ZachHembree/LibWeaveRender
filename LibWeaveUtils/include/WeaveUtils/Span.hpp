#pragma once
#include "Math.hpp"
#include "DynamicCollections.hpp"
#include <array>
#include <span>

/*
	Allocates temporary stack allocated array with alloca and assigns it to an exiting span.

	Alloca arrays are allocated on the stack frame of the calling function. When that function
	returns, the array is deallocated.
*/
#define ALLOCA_SPAN_SET(SPAN, COUNT, TYPE) \
	do { TYPE* SPAN##_TMP_PTR; ALLOCA_ARR_SET(SPAN##_TMP_PTR, COUNT, TYPE); \
	SPAN = Span<TYPE>(SPAN##_TMP_PTR, COUNT); } \
while (0)

/*
	Allocates temporary stack allocated array with alloca, fils it with nulls, and assigns 
	it to an exiting span.

	Alloca arrays are allocated on the stack frame of the calling function. When that function
	returns, the array is deallocated.
*/
#define ALLOCA_SPAN_SET_NULL(SPAN, COUNT, TYPE) \
	do { TYPE* SPAN##_TMP_PTR; ALLOCA_ARR_SET_NULL(SPAN##_TMP_PTR, COUNT, TYPE); \
	SPAN = Span<TYPE>(SPAN##_TMP_PTR, COUNT); } \
while (0)

namespace Weave
{ 
	/// <summary>
	/// A non-owning view of a contiguous section of an array
	/// </summary>
	template<typename T>
	class Span : public IDynamicArray<T>, protected std::span<T>
	{
	public:
		DEF_DYN_ARR_TRAITS(IDynamicArray<T>)
		MAKE_DEF_MOVE_COPY(Span);

		Span() = default;

		explicit Span(T* pStart) :
			std::span<T>(pStart, 1)
		{ }

		Span(T* pStart, size_t length) : 
			std::span<T>(pStart, length)
		{ }

		Span(T* pStart, const T* pEnd) :
			std::span<T>(pStart, UnsignedDelta(pEnd + 1, pStart) / sizeof(T))
		{ }

		Span(T* pStart, size_t offset, size_t length) :
			std::span<T>(pStart + offset, length)
		{ }

		Span(IDynamicArray<T>& arr) :
			std::span<T>(arr.GetData(), arr.GetLength())
		{ }

		Span(std::span<T>& other) :
			std::span<T>(other)
		{ }

		template<size_t N>
		Span(std::array<T, N>& other) :
			std::span<T>(other.data(), other.size())
		{ }

		~Span() = default;

		/// <summary>
		/// Returns the length of the span
		/// </summary>
		size_t GetLength() const override { return this->size(); }

		/// <summary>
		/// Returns a copy of the pointer to the backing the array.
		/// </summary>
		T* GetData() override { return this->data(); }

		/// <summary>
		/// Returns a const copy of the pointer to the backing the array.
		/// </summary>
		const T* GetData() const override { return this->data(); }

		/// <summary>
		/// Returns iterator pointing to the start of the collection
		/// </summary>
		iterator begin() override { return iterator(this->data()); }

		/// <summary>
		/// Returns iterator pointing to the end of the collection
		/// </summary>
		iterator end() override { return iterator(this->data() + this->size()); }

		/// <summary>
		/// Returns iterator pointing to the start of the collection
		/// </summary>
		const_iterator begin() const override { return const_iterator(this->data()); }

		/// <summary>
		/// Returns iterator pointing to the end of the collection
		/// </summary>
		const_iterator end() const override { return const_iterator(this->data() + this->size()); }

		/// <summary>
		/// Provides indexed access to array member references.
		/// </summary>
		T& operator[](size_t index) override
		{
			return GetArrayAtIndex(this->data(), index, this->size());
		}

		/// <summary>
		/// Provides indexed access to array members using constant references.
		/// </summary>
		const T& operator[](size_t index) const override
		{
			return GetArrayAtIndex(this->data(), index, this->size());
		}
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