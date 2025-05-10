#pragma once
#include <mutex>
#include "Span.hpp"

namespace Weave
{
	/// <summary>
	/// A temporary non-owning view of an array guarded by a mutex for the duration of the 
	/// view's lifetime. Lock released on destruction.
	/// </summary>
	template<typename T>
	class MutexSpan : public Span<T>
	{
	public:
		MutexSpan() = default;

		MutexSpan(T* pStart, std::mutex& mtx) :
			Span<T>(pStart),
			lock(mtx)
		{ }

		MutexSpan(T* pStart, size_t length, std::mutex& mtx) :
			Span<T>(pStart, length),
			lock(mtx)
		{ }

		MutexSpan(T* pStart, const T* pEnd, std::mutex& mtx) :
			Span<T>(pStart, pEnd),
			lock(mtx)
		{ }

		MutexSpan(T* pStart, size_t offset, size_t length, std::mutex& mtx) :
			Span<T>(pStart, offset, length),
			lock(mtx)
		{ }

		MutexSpan(IDynamicArray<T>& arr, std::mutex& mtx) :
			Span<T>(arr),
			lock(mtx)
		{ }

	protected:
		std::unique_lock<std::mutex> lock;
	};
}