#pragma once
#include "Utils.hpp"
#include "DynamicCollections.hpp"

namespace Replica
{ 
	/// <summary>
	/// Represents a contiguous section of an array
	/// </summary>
	template<typename T>
	class Span : public IDynamicArray<T>
	{
	public:
		using Iterator = DynIterator<T>;

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
		/// Returns pointer to the first member in the span
		/// </summary>
		T* GetFirst() { return pStart; }

		/// <summary>
		/// Returns pointer to the last member in the span
		/// </summary>
		T* GetLast() { return pStart + length - 1; }

		/// <summary>
		/// Returns pointer to the first member in the span
		/// </summary>
		const T* GetFirst() const { return pStart; }

		/// <summary>
		/// Returns pointer to the last member in the span
		/// </summary>
		const T* GetLast() const { return pStart + length - 1; }

		/// <summary>
		/// Returns a copy of the pointer to the backing the array.
		/// </summary>
		T* GetPtr() override { return pStart; }

		/// <summary>
		/// Returns a const copy of the pointer to the backing the array.
		/// </summary>
		const T* GetPtr() const override { return pStart; }

		/// <summary>
		/// Returns iterator pointing to the start of the collection
		/// </summary>
		Iterator begin() override { return Iterator(pStart); }

		/// <summary>
		/// Returns iterator pointing to the end of the collection
		/// </summary>
		Iterator end() override { return Iterator(pStart + length); }

		/// <summary>
		/// Returns iterator pointing to the start of the collection
		/// </summary>
		const Iterator begin() const override { return Iterator(pStart); }

		/// <summary>
		/// Returns iterator pointing to the end of the collection
		/// </summary>
		const Iterator end() const override { return Iterator(pStart + length); }

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