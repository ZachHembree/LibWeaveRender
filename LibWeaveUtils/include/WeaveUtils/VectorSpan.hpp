#pragma once
#include <concepts>
#include "DynamicCollections.hpp"

namespace Weave
{
	template <typename ContainerT, typename ReturnT, typename IndexT = std::size_t>
	concept IsContiguousCollection = requires(const ContainerT ct, IndexT i)
	{
		requires std::is_integral_v<IndexT>;
		requires std::contiguous_iterator<std::ranges::iterator_t<const ContainerT>>;

		// Immutable const indexer
		{ ct[i] } -> std::same_as<const ReturnT&>;
	};

	/// <summary>
	/// Reallocation tolerant contiguous span. Non-owning. Previously obtained iterators and pointers 
	/// may become invalid after reallocation, but span accessors provide updated pointers.
	/// </summary>
	template<typename VecT>
		requires IsContiguousCollection<VecT, typename VecT::value_type>
	class VectorSpan : public IDynamicArray<typename VecT::value_type>
	{
	public:
		using T = typename VecT::value_type;
		MAKE_DEF_MOVE_COPY(VectorSpan);
		DEF_DYN_ARR_TRAITS(IDynamicArray<typename VecT::value_type>);

		VectorSpan() :
			pVec(nullptr), start(0), length(0)
		{ }

		VectorSpan(VecT& vec, size_t start, size_t length) :
			pVec(&vec), start(start), length(length)
		{
			assert(length > 0 && length != -1 && start != -1);
		}

		~VectorSpan() = default;

		/// <summary>
		/// Returns the index of the first element in the parent vector
		/// </summary>
		size_t GetIndex() const { return start; }

		/// <summary>
		/// Returns the length of the span
		/// </summary>
		size_t GetLength() const override { return length; }

		/// <summary>
		/// Returns pointer to the first member in the span
		/// </summary>
		T* GetFirst() { assert(pVec != nullptr); return &(*pVec)[start]; }

		/// <summary>
		/// Returns pointer to the last member in the span
		/// </summary>
		T* GetLast() { assert(length > 0); return &(*pVec)[start] + length - 1; }

		/// <summary>
		/// Returns pointer to the first member in the span
		/// </summary>
		const T* GetFirst() const { assert(pVec != nullptr); return &(*pVec)[start]; }

		/// <summary>
		/// Returns pointer to the last member in the span
		/// </summary>
		const T* GetLast() const { assert(length > 0); return &(*pVec)[start] + length - 1; }

		/// <summary>
		/// Returns a copy of the pointer to the backing the array.
		/// </summary>
		T* GetData() override { assert(pVec != nullptr); return &(*pVec)[start]; }

		/// <summary>
		/// Returns a const copy of the pointer to the backing the array.
		/// </summary>
		const T* GetData() const override { assert(pVec != nullptr); return &(*pVec)[start]; }

		/// <summary>
		/// Returns iterator pointing to the start of the collection
		/// </summary>
		iterator begin() override { assert(pVec != nullptr); return iterator(&(*pVec)[start]); }

		/// <summary>
		/// Returns iterator pointing to the end of the collection
		/// </summary>
		iterator end() override { assert(length > 0); return iterator(&(*pVec)[start] + length); }

		/// <summary>
		/// Returns iterator pointing to the start of the collection
		/// </summary>
		const_iterator begin() const override { assert(pVec != nullptr); return const_iterator(&(*pVec)[start]); }

		/// <summary>
		/// Returns iterator pointing to the end of the collection
		/// </summary>
		const_iterator end() const override { assert(length > 0); return const_iterator(&(*pVec)[start] + length); }

		/// <summary>
		/// Provides indexed access to array member references.
		/// </summary>
		T& operator[](size_t index) override { assert(pVec != nullptr); return GetArrayAtIndex(&(*pVec)[start], index, length); }

		/// <summary>
		/// Provides indexed access to array members using constant references.
		/// </summary>
		const T& operator[](size_t index) const override { assert(pVec != nullptr); return GetArrayAtIndex(&(*pVec)[start], index, length); }

	protected:
		VecT* pVec;
		size_t start;
		size_t length;
	};
}

namespace std
{
	template<typename VecT>
	struct hash<Weave::VectorSpan<VecT>>
	{
		size_t operator()(const Weave::VectorSpan<VecT>& arr) const noexcept { return arr.GetHash(); }
	};
}