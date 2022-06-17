#ifndef UniqueCollections
#define UniqueCollections

#include<vector>
#include<exception>
#include<string>

template<typename T> class DynamicArrayBase;
template<typename T> class DynamicArray;
template<typename T> class UniqueArray;

/// <summary>
/// Dynamically allocated array with members of type T and a fixed length.
/// </summary>
template<typename T>
class DynamicArrayBase
{
protected:
	/// <summary>
	/// Array length
	/// </summary>
	size_t length;

	/// <summary>
	/// Array pointer
	/// </summary>
	T* data;

	/// <summary>
	/// Initializes an empty array with a null pointer.
	/// </summary>
	DynamicArrayBase() :
		length(0),
		data(nullptr)
	{ }

	/// <summary>
	/// Initializes a new dynamic array using an initializer list.
	/// </summary>
	DynamicArrayBase(const std::initializer_list<T>& list) noexcept :
		length(list.size()),
		data(new T[length])
	{
		std::copy(list.begin(), list.end(), data);
	}

	/// <summary>
	/// Initializes a dynamic array with the given length.
	/// </summary>
	explicit DynamicArrayBase(size_t length) :
		length(length),
		data(new T[length])
	{ }

	/// <summary>
	/// Initializes a new dynamic array object using the given pointer and length.
	/// </summary>
	explicit DynamicArrayBase(T* data, size_t length) :
		length(length),
		data(data)
	{ }

	/// <summary>
	/// Initializes a copy of the given dynamic array.
	/// </summary>
	DynamicArrayBase(const DynamicArrayBase& rhs) :
		length(rhs.length),
		data(new T[rhs.length])
	{
		memcpy(data, rhs.data, length * sizeof(T));
	}

	/// <summary>
	/// Initializes a new dynamic array by moving the contents of the given array to itself.
	/// </summary>
	DynamicArrayBase(DynamicArrayBase&& rhs) noexcept :
		length(rhs.length),
		data(rhs.data)
	{
		rhs.data = nullptr;
		rhs.length = -1;
	}

	/// <summary>
	/// Copy assignment operator.
	/// </summary>
	DynamicArrayBase& operator=(const DynamicArrayBase& rhs)
	{
		if (this != &rhs)
		{
			delete[] data;

			length = rhs.length;
			data = new T[length];
			memcpy(data, rhs.data, length * sizeof(T));
		}

		return *this;
	}

	/// <summary>
	/// Move assignment operator.
	/// </summary>
	DynamicArrayBase& operator=(DynamicArrayBase&& rhs) noexcept
	{
		delete[] data;

		data = rhs.data;
		length = rhs.length;
		rhs.data = nullptr;
		rhs.length = 0;

		return *this;
	}

public:
	/// <summary>
	/// Deallocates the memory backing the array.
	/// </summary>
	virtual ~DynamicArrayBase()
	{
		delete[] data;
	}

	/// <summary>
	/// Returns the length of the array.
	/// </summary>
	size_t GetLength() const { return length; }

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

		return data[index];
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

		return data[index];
	}

	/// <summary>
	/// Returns a copy of the pointer to the backing the array.
	/// </summary>
	T* GetPtr() { return data; }

	/// <summary>
	/// Returns a const copy of the pointer to the backing the array.
	/// </summary>
	const T* GetPtr() const { return data; }
};

/// <summary>
/// Dynamically allocated array with members of type T and a fixed length.
/// </summary>
template<typename T>
class DynamicArray : public DynamicArrayBase<T>
{
public:
	/// <summary>
	/// Initializes an empty array with a null pointer.
	/// </summary>
	DynamicArray() : DynamicArrayBase<T>() { }

	/// <summary>
	/// Initializes a new dynamic array from an initializer list.
	/// </summary>
	DynamicArray(const std::initializer_list<T>& initializerList) noexcept : 
		DynamicArrayBase<T>(initializerList)
	{ }

	/// <summary>
	/// Initializes a dynamic array with the given length.
	/// </summary>
	explicit DynamicArray(size_t length) : 
		DynamicArrayBase<T>(length) 
	{ }

	/// <summary>
	/// Initializes a new dynamic array object using the given pointer and length.
	/// </summary>
	explicit DynamicArray(T* data, size_t length) : 
		DynamicArrayBase<T>(data, length) 
	{ }

	/// <summary>
	/// Initializes a copy of the given dynamic array.
	/// </summary>
	DynamicArray(const DynamicArray& rhs) : 
		DynamicArrayBase<T>(rhs) 
	{ }

	/// <summary>
	/// Initializes a copy of the given dynamic array.
	/// </summary>
	explicit DynamicArray(const UniqueArray<T>& rhs) : 
		DynamicArrayBase<T>((DynamicArrayBase<T>&)rhs)
	{ }

	/// <summary>
	/// Initializes a new dynamic array by moving the contents of the given array to itself.
	/// </summary>
	DynamicArray(DynamicArray&& rhs) noexcept : 
		DynamicArrayBase<T>(std::move(rhs)) 
	{ }

	/// <summary>
	/// Initializes a new dynamic array by moving the contents of the given unique array to itself.
	/// </summary>
	explicit DynamicArray(UniqueArray<T>&& rhs) noexcept : 
		DynamicArrayBase<T>((DynamicArrayBase<T>&&)(rhs))
	{ }

	/// <summary>
	/// Copy assignment operator.
	/// </summary>
	DynamicArray& operator=(const DynamicArray& rhs)
	{ return static_cast<DynamicArray&>(DynamicArrayBase<T>::operator=(rhs)); }

	/// <summary>
	/// Move assignment operator.
	/// </summary>
	DynamicArray& operator=(DynamicArray&& rhs) noexcept 
	{ return static_cast<DynamicArray&>(DynamicArrayBase<T>::operator=(std::move(rhs))); }
};

/// <summary>
/// Unique dynamically allocated array. Implicit copy assignment disallowed.
/// </summary>
template<typename T>
class UniqueArray : public DynamicArrayBase<T>
{
private:
	/// <summary>
	/// Initializes a copy of the given unique array.
	/// </summary>
	UniqueArray(const UniqueArray& rhs) : 
		DynamicArrayBase<T>(rhs) 
	{ }

public:
	/// <summary>
	/// Initializes an empty array with a null pointer.
	/// </summary>
	UniqueArray() : 
		DynamicArrayBase<T>()
	{ }

	/// <summary>
	/// Initializes a new unique array from an initializer list.
	/// </summary>
	UniqueArray(const std::initializer_list<T>& initializerList) noexcept :
		DynamicArrayBase<T>(initializerList)
	{ }

	/// <summary>
	/// Initializes a unique array with the given length.
	/// </summary>
	explicit UniqueArray(size_t length) :
		DynamicArrayBase<T>(length) 
	{ }

	/// <summary>
	/// Creates a new unique array using the given pointer and length.
	/// </summary>
	explicit UniqueArray(T* data, size_t length) :
		DynamicArrayBase<T>(data, length) 
	{ }

	/// <summary>
	/// Initializes a copy of the given dynamic array.
	/// </summary>
	explicit UniqueArray(const DynamicArray<T>& rhs) : 
		DynamicArrayBase<T>((DynamicArrayBase<T>&)rhs)
	{ }

	/// <summary>
	/// Initializes a new unique array by moving the contents of the given unique array to itself.
	/// </summary>
	UniqueArray(UniqueArray&& rhs) noexcept : 
		DynamicArrayBase<T>(std::move(rhs)) 
	{ }

	/// <summary>
	/// Initializes a new unique array by moving the contents of the given dynamic array to itself.
	/// </summary>
	explicit UniqueArray(DynamicArray<T>&& rhs) noexcept :
		DynamicArrayBase<T>((DynamicArrayBase<T>&&)rhs)
	{ }
	
	/// <summary>
	/// Copy assignment operator. Disabled.
	/// </summary>
	UniqueArray& operator=(const UniqueArray& rhs) = delete;

	/// <summary>
	/// Move assignment operator.
	/// </summary>
	UniqueArray& operator=(UniqueArray&& rhs) noexcept 
	{ return static_cast<UniqueArray&>(DynamicArrayBase<T>::operator=(std::move(rhs))); }

	/// <summary>
	/// Returns a new copy of the array.
	/// </summary>
	UniqueArray GetCopy() 
	{ return UniqueArray(*this); }
};

/// <summary>
/// Move-only wrapper for std::vector(T)
/// </summary>
template<typename T>
class UniqueVector : public std::vector<T>
{
private:
	/// <summary>
	/// Initializes a new copy of the given unique vector.
	/// </summary>
	UniqueVector(const UniqueVector& rhs) : 
		std::vector<T>(rhs) 
	{ };

public:
	/// <summary>
	/// Initializes a new unique vector.
	/// </summary>
	UniqueVector() : 
		std::vector<T>() 
	{ }

	/// <summary>
	/// Initializes a new unique vector from an initializer list.
	/// </summary>
	UniqueVector(const std::initializer_list<T>& initializerList) noexcept :
		std::vector<T>(initializerList)
	{ }

	/// <summary>
	/// Initializes a new unique vector with the given capacity.
	/// </summary>
	explicit UniqueVector(size_t capacity) : 
		std::vector<T>(capacity) 
	{ }

	/// <summary>
	/// Initializes a new vector by moving the contents the given vector into itself.
	/// </summary>
	UniqueVector(UniqueVector&& rhs) noexcept :
		std::vector<T>(std::move(rhs))
	{ }

	/// <summary>
	/// Initializes a new unique vector by moving the contents the given std::vector into itself.
	/// </summary>
	explicit UniqueVector(std::vector<T>&& rhs) noexcept :
		std::vector<T>(std::move(rhs)) 
	{ }

	/// <summary>
	/// Copy assignment operator. Disabled.
	/// </summary>
	UniqueVector& operator=(const UniqueVector& rhs) = delete;

	/// <summary>
	/// Move assignment operator.
	/// </summary>
	UniqueVector& operator=(UniqueVector&& rhs) noexcept
	{ return static_cast<UniqueVector&>(std::vector<T>::operator=(std::move(rhs))); }

	/// <summary>
	/// Returns a new copy of the unique vector.
	/// </summary>
	UniqueVector GetCopy() 
	{ return UniqueVector(*this); }
};

#endif