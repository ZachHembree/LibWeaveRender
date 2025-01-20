#pragma once
#include "Span.hpp"

namespace Replica
{
	/// <summary>
	/// Generalized interface for reading/writing to shared data buffers
	/// </summary>
	template<class HandleT, typename DataT>
	class DataBufferSpan : public Span<DataT>
	{
	public:
		DataBufferSpan() : pParent(nullptr)
		{ }

		DataBufferSpan(HandleT* pParent, DataT* pStart, size_t len) :
			Span<DataT>(pStart, len),
			pParent(pParent)
		{ }

		HandleT& GetParent() { return *pParent; }

	protected:
		HandleT* pParent;
	};
}