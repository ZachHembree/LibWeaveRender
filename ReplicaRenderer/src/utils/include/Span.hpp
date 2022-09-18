#pragma once
#include "Utils.hpp"
#include "DynamicCollections.hpp"

namespace Replica
{ 
	/// <summary>
	/// Represents a contiguous section of an array
	/// </summary>
	template<typename T>
	class Span
	{
	public:

		Span() : 
			pStart(nullptr), length(0)
		{ }

		Span(T* pStart) :
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

		Span(const Span& other) :
			pStart(other.pStart),
			length(other.length)
		{ }

		size_t GetLength() const { return length; }

		T* GetFirst() { return pStart; }

		T* GetLast() { return pStart + length - 1; }

		const T* GetFirst() const { return pStart; }

		const T* GetLast() const { return pStart + length - 1; }

		Span& operator=(const Span& other)
		{
			memcpy(this, &other, sizeof(Span));
			return *this;
		}

		/// <summary>
		/// Provides indexed access to array member references.
		/// </summary>
		T& operator[](size_t index)
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
		const T& operator[](size_t index) const
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