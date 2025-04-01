#pragma once
#include <vector>
#include <exception>
#include <string>
#include <iterator>
#include <memory>
#include <functional>
#include <concepts>
#include "ReplicaUtils/GlobalUtils.hpp"

namespace Replica
{
	template<typename T> class DynamicArray;
	template<typename T> class UniqueArray;
	template<typename T> class UniqueVector;

	/// <summary>
	/// Returns a hash for an arbitrary number of hashable values
	/// </summary>
	template<typename... Ts>
	inline size_t GetCombinedHash(const Ts&... values)
	{
		size_t seed = 0;
		auto combine_one = [&seed](const auto& value)
		{
			seed ^= std::hash<std::decay_t<decltype(value)>>{}(value)+0x9e3779b9 + (seed << 6) + (seed >> 2);
		};

		// Use fold expression to invoke lambda for each variadic argument
		(combine_one(values), ...);

		return seed;
	}

	/// <summary>
	/// Basic iterator template for contiguous collections
	/// </summary>
	template<typename ValueType>
	class DynIterator
	{
	public:
		using iterator_category = std::contiguous_iterator_tag;
		using value_type = ValueType;
		using difference_type = ptrdiff_t;
		using pointer = ValueType*;
		using reference = ValueType&;

		DynIterator() : pData(nullptr) { }

		template <typename T = ValueType, typename = std::enable_if_t<!std::is_const<T>::value>>
		DynIterator(T* pData) : pData(pData) { }

		DynIterator(const ValueType* pData) : pData(const_cast<ValueType*>(pData)) { }

		// Dereference operators
		reference operator*() const { return *pData; }
		pointer operator->() const { return pData; }

		// Random access element access
		reference operator[](difference_type offset) const { return *(pData + offset); }

		// Arithmetic operators
		DynIterator& operator++() { ++pData; return *this; }
		DynIterator operator++(int) { DynIterator temp = *this; ++(*this); return temp; }

		DynIterator& operator--() { --pData; return *this; }
		DynIterator operator--(int) { DynIterator temp = *this; --(*this); return temp; }

		DynIterator operator+(difference_type offset) const { return DynIterator(pData + offset); }
		DynIterator operator-(difference_type offset) const { return DynIterator(pData - offset); }

		DynIterator& operator+=(difference_type offset) { pData += offset; return *this; }
		DynIterator& operator-=(difference_type offset) { pData -= offset; return *this; }

		difference_type operator-(const DynIterator& other) const { return pData - other.pData; }

		// Comparison operators
		bool operator==(const DynIterator& other) const { return pData == other.pData; }
		bool operator!=(const DynIterator& other) const { return pData != other.pData; }
		bool operator<(const DynIterator& other) const { return pData < other.pData; }
		bool operator>(const DynIterator& other) const { return pData > other.pData; }
		bool operator<=(const DynIterator& other) const { return pData <= other.pData; }
		bool operator>=(const DynIterator& other) const { return pData >= other.pData; }

	protected:
		pointer pData;
	};

	/// <summary>
	/// Interface template for dynamic arrays/vectors
	/// </summary>
	template<typename T>
	class IDynamicArray
	{
	public:
		using Iterator = DynIterator<T>;
		using RevIterator = std::reverse_iterator<DynIterator<T>>;

		virtual ~IDynamicArray() = default;

		/// <summary>
		/// Returns the length of the array.
		/// </summary>
		virtual size_t GetLength() const = 0;

		/// <summary>
		/// Provides indexed access to array member references.
		/// </summary>
		virtual T& operator[](size_t index) = 0;

		/// <summary>
		/// Provides indexed access to array members using constant references.
		/// </summary>
		virtual const T& operator[](size_t index) const = 0;

		/// <summary>
		/// Returns a copy of the pointer to the backing the array.
		/// </summary>
		virtual T* GetPtr() = 0;

		/// <summary>
		/// Returns a const copy of the pointer to the backing the array.
		/// </summary>
		virtual const T* GetPtr() const = 0;

		/// <summary>
		/// Returns iterator pointing to the start of the collection
		/// </summary>
		virtual Iterator begin() = 0;

		/// <summary>
		/// Returns iterator pointing to the end of the collection
		/// </summary>
		virtual Iterator end() = 0;

		/// <summary>
		/// Returns iterator pointing to the start of the collection
		/// </summary>
		virtual const Iterator begin() const = 0;

		/// <summary>
		/// Returns iterator pointing to the end of the collection
		/// </summary>
		virtual const Iterator end() const = 0;

		/// <summary>
		/// Returns iterator pointing to the start of the collection
		/// </summary>
		virtual RevIterator rbegin() { return RevIterator(begin()); }

		/// <summary>
		/// Returns iterator pointing to the end of the collection
		/// </summary>
		virtual RevIterator rend() { return RevIterator(end()); }

		/// <summary>
		/// Returns iterator pointing to the start of the collection
		/// </summary>
		virtual const RevIterator rbegin() const { return RevIterator(begin()); }

		/// <summary>
		/// Returns iterator pointing to the end of the collection
		/// </summary>
		virtual const RevIterator rend() const { return RevIterator(end()); }

		virtual T& front() { return (*this)[0]; }

		virtual const T& front() const { return (*this)[0]; }

		virtual T& back() { return (*this)[GetLength() - 1]; }

		virtual const T& back() const { return (*this)[GetLength() - 1]; }

		virtual T& at(size_t index) { return (*this)[index]; }

		virtual const T& at(size_t index) const { return (*this)[index]; }

		/// <summary>
		/// Calculates a combined hash of the current contents of the array
		/// </summary>
		size_t GetHash() const
		{
			size_t seed = 0;

			for (const T& value : *this)
				seed ^= std::hash<T>{}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

			return seed;
		}
	};

	/// <summary>
	/// Returns the size of the array in bytes
	/// </summary>
	template<typename T>
	size_t GetArrSize(const IDynamicArray<T>& arr) { return arr.GetLength() * sizeof(T); }

	/// <summary>
	/// Returns true if the contents of the arrays are equal
	/// </summary>
	template<typename T>
	bool GetIsArrDataEqual(const IDynamicArray<T>& left, const IDynamicArray<T>& right)
	{
		if (&left == &right) return true;
		if (left.GetLength() != right.GetLength()) return false;

		if constexpr (std::is_trivially_copyable_v<T>)
			return memcmp(left.GetPtr(), right.GetPtr(), left.GetLength() * sizeof(T)) == 0;
		else
			return std::equal(left.begin(), left.end(), right.begin());
	}

	/// <summary>
	/// Returns true if the contents of the arrays are equal
	/// </summary>
	template<typename T>
	bool operator==(const IDynamicArray<T>& left, const IDynamicArray<T>& right) { return GetIsArrDataEqual(left, right); }

	/// <summary>
	/// Returns true if the contents of the arrays are NOT equal
	/// </summary>
	template<typename T>
	bool operator!=(const IDynamicArray<T>& left, const IDynamicArray<T>& right) { return !GetIsArrDataEqual(left, right); }

	/// <summary>
	/// Returns the size of the array in bytes
	/// </summary>
	template<typename T>
	size_t GetArrSize(const std::vector<T>& arr) { return arr.size() * sizeof(T); }

	/// <summary>
	/// Dynamically allocated array with members of type T and a fixed length.
	/// </summary>
	template<typename T>
	class DynamicArray : public IDynamicArray<T>
	{
	public:
		using Iterator = IDynamicArray<T>::Iterator;
		using RevIterator = IDynamicArray<T>::RevIterator;

		/// <summary>
		/// Initializes an empty array with a null pointer.
		/// </summary>
		DynamicArray() :
			length(0),
			data(nullptr)
		{ }

		/// <summary>
		/// Initializes a new dynamic array using an initializer list.
		/// </summary>
		DynamicArray(const std::initializer_list<T>& list) :
			length(list.size()),
			data(new T[length])
		{
			std::copy(list.begin(), list.end(), data);
		}

		/// <summary>
		/// Initializes a new dynamic array using an initializer list.
		/// </summary>
		explicit DynamicArray(const IDynamicArray<T>& arr) :
			length(arr.GetLength()),
			data(new T[arr.GetLength()])
		{
			std::copy(arr.begin(), arr.end(), data);
		}

		/// <summary>
		/// Initializes a dynamic array with the given length.
		/// </summary>
		template <std::integral CapT>
		explicit DynamicArray(CapT length) :
			length(static_cast<size_t>(length)),
			data((length > 0) ? new T[static_cast<size_t>(length)]() : nullptr)
		{ }

		/// <summary>
		/// Initializes a dynamic array with the given length.
		/// </summary>
		explicit DynamicArray(size_t length, const T& initValue) :
			length(length),
			data((length > 0) ? new T[length](initValue) : nullptr)
		{ }

		/// <summary>
		/// Initializes a new dynamic array object using the given pointer and length.
		/// Creates a copy.
		/// </summary>
		explicit DynamicArray(const T* data, const size_t length) :
			length(length),
			data(new T[length])
		{
			std::copy(data, data + length, this->data);
		}

		/// <summary>
		/// Initializes a new dynamic array object using the given pointer and length.
		/// Takes ownership of the pointer.
		/// </summary>
		explicit DynamicArray(std::unique_ptr<T[]>&& ptr, size_t length)
			: length(length), data(ptr.release())
		{ }

		/// <summary>
		/// Initializes a copy of the given dynamic array.
		/// </summary>
		DynamicArray(const DynamicArray& rhs) :
			length(rhs.length),
			data(new T[rhs.length])
		{
			std::copy(rhs.data, rhs.data + length, this->data);
		}

		// Disallow implicit copies of unique wrappers
		DynamicArray(const UniqueArray<T>& other) = delete;

		DynamicArray& operator=(const UniqueArray<T>& rhs) = delete;

		/// <summary>
		/// Initializes a new dynamic array by moving the contents of the given array to itself.
		/// </summary>
		DynamicArray(DynamicArray&& rhs) noexcept :
			length(rhs.length),
			data(rhs.data)
		{
			rhs.data = nullptr;
			rhs.length = 0;
		}

		/// <summary>
		/// Initializes a new dynamic array by moving the contents of the given unique array to itself.
		/// </summary>
		explicit DynamicArray(UniqueArray<T>&& rhs) noexcept : 
			DynamicArray<T>(std::move(reinterpret_cast<DynamicArray<T>&>(rhs)))
		{ }

		/// <summary>
		/// Deallocates the memory backing the array.
		/// </summary>
		~DynamicArray() { delete[] data; }

		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		DynamicArray& operator=(const DynamicArray& rhs)
		{
			if (this != &rhs)
			{
				if (length != rhs.length)
				{
					T* newData = new T[rhs.length];
					delete[] data;
					data = newData;
				}

				length = rhs.length;
				std::copy(rhs.data, rhs.data + length, data);
			}

			return *this;
		}

		/// <summary>
		/// Move assignment operator.
		/// </summary>
		DynamicArray& operator=(DynamicArray&& rhs) noexcept
		{
			if (data != nullptr)
				delete[] data;

			data = rhs.data;
			length = rhs.length;
			rhs.data = nullptr;
			rhs.length = 0;

			return *this;
		}

		/// <summary>
		/// Returns iterator pointing to the start of the collection
		/// </summary>
		Iterator begin() override { return Iterator(data); }

		/// <summary>
		/// Returns iterator pointing to the end of the collection
		/// </summary>
		Iterator end() override { return Iterator(data + length); }

		/// <summary>
		/// Returns iterator pointing to the start of the collection
		/// </summary>
		const Iterator begin() const override { return Iterator(data); }

		/// <summary>
		/// Returns iterator pointing to the end of the collection
		/// </summary>
		const Iterator end() const override { return Iterator(data + length); }

		/// <summary>
		/// Returns the first element in the vector
		/// </summary>
		T& front() override { return data[0]; }

		/// <summary>
		/// Returns the first element in the vector
		/// </summary>
		const T& front() const override { return data[0]; }

		/// <summary>
		/// Returns the last element in the vector
		/// </summary>
		T& back() override { return data[length - 1]; }

		/// <summary>
		/// Returns the last element in the vector
		/// </summary>
		const T& back() const override { return data[length - 1]; }

		/// <summary>
		/// Returns the length of the array.
		/// </summary>
		size_t GetLength() const override { return length; }

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

			return data[index];
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

			return data[index];
		}

		/// <summary>
		/// Returns a copy of the pointer to the backing the array.
		/// </summary>
		T* GetPtr() override { return data; }

		/// <summary>
		/// Returns a const copy of the pointer to the backing the array.
		/// </summary>
		const T* GetPtr() const override { return data; }

		void swap(DynamicArray& other) noexcept
		{
			std::swap(length, other.length);
			std::swap(data, other.data);
		}

		/// <summary>
		/// Returns a new copy of the array.
		/// </summary>
		DynamicArray GetCopy() const { return DynamicArray(*this); }

	protected:
		/// <summary>
		/// Array length
		/// </summary>
		size_t length;

		/// <summary>
		/// Array pointer
		/// </summary>
		T* data;
	};

	/// <summary>
	/// Unique dynamically allocated array. Implicit copy assignment disallowed.
	/// </summary>
	template<typename T>
	class UniqueArray : public DynamicArray<T>
	{
	private:
		MAKE_DEF_COPY(UniqueArray)

	public:
		using DynamicArray<T>::DynamicArray;
		MAKE_DEF_MOVE(UniqueArray)

		/// <summary>
		/// Initializes a copy of the given dynamic array.
		/// </summary>
		UniqueArray(DynamicArray<T>&& rhs) : 
			DynamicArray<T>(std::move(rhs))
		{ }
	};

	/// <summary>
	/// Vector{T} implementing IDynamicCollection{T}
	/// </summary>
	template<typename T>
	class Vector : public IDynamicArray<T>, private std::vector<T>
	{		
	public:
		using Iterator = IDynamicArray<T>::Iterator;
		using RevIterator = IDynamicArray<T>::RevIterator;

		using std::vector<T>::vector;
		MAKE_DEF_MOVE_COPY(Vector)

		using std::vector<T>::push_back;
		using std::vector<T>::emplace_back;
		using std::vector<T>::pop_back;
		using std::vector<T>::clear;
		using std::vector<T>::max_size;
		using std::vector<T>::resize;
		using std::vector<T>::capacity;
		using std::vector<T>::reserve;
		using std::vector<T>::empty;
		using std::vector<T>::shrink_to_fit;
		using std::vector<T>::swap;

		explicit Vector(const IDynamicArray<T>& other) :
			std::vector<T>(other.GetPtr(), other.GetPtr() + other.GetLength())
		{ }

		/// <summary>
		/// Initializes a new unique vector with the given capacity.
		/// </summary>
		template <std::integral CapT>
		explicit Vector(CapT capacity) { this->reserve(static_cast<size_t>(capacity)); }

		// Disallow implicit copies of unique wrappers
		Vector(const UniqueVector<T>& other) = delete;

		Vector& operator=(const UniqueVector<T>& other) = delete;

		/// <summary>
		/// Adds a copy of the given value to the end of the vector
		/// </summary>
		void Add(const T& value) { this->push_back(value); }

		/// <summary>
		/// Moves the given value into the end of the vector
		/// </summary>
		void Add(T&& value) noexcept { this->push_back(std::move(value)); }

		/// <summary>
		/// Inserts a copy of the given value at the given index
		/// </summary>
		void Insert(ptrdiff_t index, const T& value)
		{
			this->insert(std::vector<T>::begin() + index, value);
		}

		/// <summary>
		/// Moves the given value to the given position in the vector
		/// </summary>
		void Insert(ptrdiff_t index, T&& value) noexcept
		{
			this->insert(std::vector<T>::begin() + index, std::move(value));
		}

		/// <summary>
		/// Moves the given source range into the end of the vector
		/// </summary>
		void AddRange(IDynamicArray<T>&& src, ptrdiff_t srcStart = 0, ptrdiff_t count = -1) noexcept
		{
			InsertRange(this->size(), std::move(src), srcStart, count);
		}

		/// <summary>
		/// Moves the given source range into the vector at the given index
		/// </summary>
		void InsertRange(ptrdiff_t start, IDynamicArray<T>&& src, ptrdiff_t srcStart = 0, ptrdiff_t count = -1) noexcept
		{
			if (count == -1)
				count = (ptrdiff_t)src.GetLength() - count;

			if (count <= 0)
				return;
				
			this->insert((std::vector<T>::begin() + start), 
				std::move_iterator(src.begin() + srcStart), 
				std::move_iterator(src.begin() + srcStart + count)
			);
		}

		template <class InputIterator>
		void InsertRange(ptrdiff_t start, InputIterator first, InputIterator last)
		{
			this->insert((std::vector<T>::begin() + start), first, last);
		}

		/// <summary>
		/// Appends a copy of the given source range to the vector
		/// </summary>
		void AddRange(const IDynamicArray<T>& src, ptrdiff_t srcStart = 0, ptrdiff_t count = -1)
		{
			InsertRange(this->size(), src, srcStart, count);
		}

		/// <summary>
		/// Inserts a copy of the given source range starting at the given index in the vector
		/// </summary>
		void InsertRange(ptrdiff_t start, const IDynamicArray<T>& src, ptrdiff_t srcStart = 0, ptrdiff_t count = -1)
		{
			if (count == -1)
				count = (ptrdiff_t)src.GetLength() - srcStart;

			if (count == 0)
				return;
				
			this->insert((std::vector<T>::begin() + start),
				(src.begin() + srcStart),
				(src.begin() + srcStart + count)
			);
		}

		/// <summary>
		/// Removes the member at the given index
		/// </summary>
		void RemoveAt(size_t index)
		{
#if _CONTAINER_DEBUG_LEVEL > 0
			if ( index >= (this->size()) )
			{
				char buffer[100];
				sprintf_s( buffer, 100, "Array index out of range. Index: %tu, Length %tu", index, (this->size()) );

				throw std::exception(buffer);
			}
#endif

			this->erase(std::vector<T>::begin() + index);
		}

		/// <summary>
		/// Removes the specified contiguous range from the vector
		/// </summary>
		void RemoveRange(size_t index, size_t count)
		{
			if (count == 0)
				return;

#if _CONTAINER_DEBUG_LEVEL > 0
			if ( index >= (this->size()) )
			{
				char buffer[100];
				sprintf_s( buffer, 100, "Array index out of range. Index: %tu, Length %tu", index, (this->size()) );

				throw std::exception(buffer);
			}

			if ( (index + count) >= (this->size() + 1) ) 
			{
				char buffer[100];
				sprintf_s( buffer, 100, "Subrange count out of range. Index: %tu, Length %tu", (index + count), (this->size()) );

				throw std::exception(buffer);
			}
#endif

			this->erase(std::vector<T>::begin() + index, std::vector<T>::begin() + (index + count));
		}

		/// <summary>
		/// Returns the length of the vector.
		/// </summary>
		size_t GetLength() const override { return this->size(); }

		/// <summary>
		/// Returns a copy of the pointer to the backing the vector.
		/// </summary>
		T* GetPtr() override { return this->data(); }

		/// <summary>
		/// Returns a const copy of the pointer to the backing the vector.
		/// </summary>
		const T* GetPtr() const override { return this->data(); }

		/// <summary>
		/// Returns iterator pointing to the start of the collection
		/// </summary>
		Iterator begin() override { return Iterator(this->data()); }

		/// <summary>
		/// Returns iterator pointing to the end of the collection
		/// </summary>
		Iterator end() override { return Iterator(this->data() + this->size()); }

		/// <summary>
		/// Returns iterator pointing to the start of the collection
		/// </summary>
		const Iterator begin() const override { return Iterator(this->data()); }

		/// <summary>
		/// Returns iterator pointing to the end of the collection
		/// </summary>
		const Iterator end() const override { return Iterator(this->data() + this->size()); }

		/// <summary>
		/// Returns the first element in the vector
		/// </summary>
		T& front() override { return std::vector<T>::front(); }

		/// <summary>
		/// Returns the first element in the vector
		/// </summary>
		const T& front() const override { return std::vector<T>::front(); }

		/// <summary>
		/// Returns the last element in the vector
		/// </summary>
		T& back() override { return std::vector<T>::back(); }

		/// <summary>
		/// Returns the last element in the vector
		/// </summary>
		const T& back() const override { return std::vector<T>::back(); }

		/// <summary>
		/// Returns the element at the given index
		/// </summary>
		T& at(size_t index) override { return std::vector<T>::at(index); }

		/// <summary>
		/// Returns the element at the given index
		/// </summary>
		const T& at(size_t index) const override { return std::vector<T>::at(index); }

		/// <summary>
		/// Provides indexed access to vector member references.
		/// </summary>
		T& operator[](size_t index) override 
		{ 
#if _CONTAINER_DEBUG_LEVEL > 0
			if (index >= this->size())
			{
				char buffer[100];
				sprintf_s(buffer, 100, "Vector index out of range. Index: %tu, Length %tu", index, this->size());

				throw std::exception(buffer);
			}
#endif

			return this->at(index); 
		}

		/// <summary>
		/// Provides indexed access to vector members using constant references.
		/// </summary>
		const T& operator[](size_t index) const override 
		{ 
#if _CONTAINER_DEBUG_LEVEL > 0
			if (index >= this->size())
			{
				char buffer[100];
				sprintf_s(buffer, 100, "Vector index out of range. Index: %tu, Length %tu", index, this->size());

				throw std::exception(buffer);
			}
#endif

			return this->at(index); 
		}

		/// <summary>
		/// Returns a new copy of the unique vector.
		/// </summary>
		Vector GetCopy() const { return Vector(*this); }
	};

	/// <summary>
	/// Move-only wrapper for Replica::Vector(T)
	/// </summary>
	template<typename T>
	class UniqueVector : public Vector<T>
	{
	private:
		MAKE_DEF_COPY(UniqueVector)

	public:
		using Vector<T>::Vector;
		MAKE_DEF_MOVE(UniqueVector)

		/// <summary>
		/// Initializes a new unique vector by moving the contents the given Vector into itself.
		/// </summary>
		explicit UniqueVector(Vector<T>&& rhs) noexcept :
			Vector<T>(std::move(rhs))
		{ }
	};
}

/// <summary>
/// std::hash specializations
/// </summary>
namespace std
{
	template<typename T>
	struct hash<Replica::DynamicArray<T>>
	{
		size_t operator()(const Replica::DynamicArray<T>& arr) const noexcept { return arr.GetHash(); }
	};

	template<typename T>
	struct hash<Replica::UniqueArray<T>>
	{
		size_t operator()(const Replica::UniqueArray<T>& arr) const noexcept { return arr.GetHash(); }
	};

	template<typename T>
	struct hash<Replica::Vector<T>> {
		size_t operator()(const Replica::Vector<T>& vec) const noexcept { return vec.GetHash(); }
	};

	template<typename T>
	struct hash<Replica::UniqueVector<T>>
	{
		size_t operator()(const Replica::UniqueVector<T>& vec) const noexcept { return vec.GetHash(); }
	};
}