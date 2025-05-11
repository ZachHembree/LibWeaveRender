#pragma once
#include <mutex>
#include "Span.hpp"

namespace Weave
{
	/// <summary>
	/// Requires the given type to behave like a basic lockable type
	/// </summary>
	template <typename M>
	concept IsMutex = requires(M & m) 
	{
		{ m.lock() };
		{ m.unlock() };
	};

	/// <summary>
	/// A temporary non-owning view of an array guarded by a mutex for the duration of the 
	/// view's lifetime. Lock released on destruction.
	/// </summary>
	template<typename T, typename MutexT>
	requires IsMutex<MutexT>
	class MutexSpan : public Span<T>
	{
	public:
		MutexSpan() = default;

		MutexSpan(T* pStart, MutexT& mtx) :
			Span<T>(pStart),
			lock(mtx)
		{ }

		MutexSpan(T* pStart, size_t length, MutexT& mtx) :
			Span<T>(pStart, length),
			lock(mtx)
		{ }

		MutexSpan(T* pStart, const T* pEnd, MutexT& mtx) :
			Span<T>(pStart, pEnd),
			lock(mtx)
		{ }

		MutexSpan(T* pStart, size_t offset, size_t length, MutexT& mtx) :
			Span<T>(pStart, offset, length),
			lock(mtx)
		{ }

		MutexSpan(IDynamicArray<T>& arr, MutexT& mtx) :
			Span<T>(arr),
			lock(mtx)
		{ }

	protected:
		std::unique_lock<MutexT> lock;
	};
}