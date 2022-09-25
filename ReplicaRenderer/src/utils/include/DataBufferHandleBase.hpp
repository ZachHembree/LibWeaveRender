#pragma once
#include "Span.hpp"

namespace Replica
{
	template<class HandleT, typename DataT>
	class DataBufferSpan;

	/// <summary>
	/// Generalized interface for getting and returning shared data buffers
	/// </summary>
	template<class SpanT, typename DataT>
	class DataBufferHandleBase
	{
	public:
		/// <summary>
		/// Returns the buffer as a Span
		/// </summary>
		virtual SpanT Get() = 0;

		/// <summary>
		/// Returns the next unused span from the buffer
		/// </summary>
		virtual SpanT GetNext(size_t length) = 0;

		/// <summary>
		/// Returns the given data span to the buffer
		/// </summary>
		virtual void Return(SpanT&& data) = 0;
	};

	/// <summary>
	/// Generalized interface for reading/writing to shared data buffers
	/// </summary>
	template<class HandleT, typename DataT>
	class DataBufferSpanBase : public Span<DataT>
	{
	public:
		DataBufferSpanBase() : pParent(nullptr)
		{ }

		DataBufferSpanBase(HandleT* pParent, DataT* pStart, size_t len) :
			Span<DataT>(pStart, len),
			pParent(pParent)
		{ }

	protected:
		HandleT* pParent;
	};
}