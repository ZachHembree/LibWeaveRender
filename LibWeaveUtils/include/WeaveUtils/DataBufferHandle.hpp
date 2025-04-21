#pragma once
#include "WeaveUtils/Span.hpp"

namespace Weave
{
	/// <summary>
	/// Generalized interface for reading/writing to shared data buffers
	/// </summary>
	template<class HandleT, typename DataT>
	class DataBufferSpan : public Span<DataT>
	{
	public:
		MAKE_NO_COPY(DataBufferSpan);

		DataBufferSpan() : pParent(nullptr)
		{ }

		DataBufferSpan(HandleT* pParent, DataT* pStart, size_t len) :
			Span<DataT>(pStart, len),
			pParent(pParent)
		{ }

		DataBufferSpan(DataBufferSpan&&) noexcept = default;

		DataBufferSpan& operator=(DataBufferSpan&&) noexcept = default;

		/// <summary>
		/// Reference to the owner of the buffer
		/// </summary>
		HandleT& GetParent() { return *pParent; }

		/// <summary>
		/// Returns true of the handle is valid and initialized.
		/// </summary>
		bool GetIsValid() const { return pParent != nullptr; }

	protected:
		HandleT* pParent;
	};
}