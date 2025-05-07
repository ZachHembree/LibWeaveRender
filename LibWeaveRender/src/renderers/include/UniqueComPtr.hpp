#pragma once
#include "WeaveUtils/GlobalUtils.hpp"

namespace Weave
{
    /// <summary>
    /// Constraint for COM-like interfaces
    /// </summary>
    template <typename T>
    concept ComPtrType = requires(T * obj)
    {
        requires std::is_class_v<T>;
        { obj->AddRef() } -> std::convertible_to<unsigned long>;
        { obj->Release() } -> std::convertible_to<unsigned long>;
    };

    /// <summary>
    /// Constraint for COM interface subtype queries
    /// </summary>
    template <typename T, typename IdentT>
    concept HasComQueryInterface = requires(T * obj, IdentT riid, void** ppvObject)
    {
        requires ComPtrType<T>;
        requires std::is_class_v<IdentT>;
        { obj->QueryInterface(riid, ppvObject) } -> std::convertible_to<long>;
    };

    /// <summary>
    /// Move-only RAII COM pointer wrapper managing COM reference counting. COM constraints enforced lazily to facilitate 
    /// usage with incomplete types and forward declarations.
    /// </summary>
    template <typename T>
    class UniqueComPtr
    {
    public:
        MAKE_NO_COPY(UniqueComPtr);

        /// <summary>
        /// Underlying COM type pointed to by this object
        /// </summary>
        using InterfaceType = T;

        UniqueComPtr() noexcept : pCom(nullptr) {}

        /// <summary>
        /// Initializes UniqueComPtr to the given value and increments the COM reference count
        /// </summary>
        explicit UniqueComPtr(T* ptr) noexcept requires ComPtrType<T>
            : pCom(ptr)
        {
            if (pCom != nullptr)
                pCom->AddRef();
        }

        ~UniqueComPtr() noexcept { Release(); }

        /// <summary>
        /// Transfers ownership of the other objects COM pointer and releases the current 
        /// pointer.
        /// </summary>
        UniqueComPtr(UniqueComPtr&& other) noexcept :
            pCom(other.pCom)
        { 
            other.pCom = nullptr;
        }

        /// <summary>
        /// Transfers ownership of the other object's COM pointer and releases the current 
        /// pointer.
        /// </summary>
        UniqueComPtr& operator=(UniqueComPtr&& other) noexcept
        {
            Release();
            this->pCom = other.pCom;
            other.pCom = nullptr;
            return *this;
        }

        /// <summary>
        /// Releases the existing pointer and attaches to a new pointer.
        /// </summary>
        void Attach(T* p) noexcept requires ComPtrType<T> { Release(); pCom = p; }

        /// <summary>
        /// Releases the current COM interface and attaches the wrapper to a new pointer
        /// </summary>
        void Reset(T* p = nullptr) noexcept { Attach(p); }

        [[nodiscard]] T* Get() const noexcept { return pCom; }

        [[nodiscard]] T* operator->() const {  WV_ASSERT(pCom != nullptr); return pCom; }

        explicit operator bool() const noexcept { return pCom != nullptr; }

        bool operator==(std::nullptr_t) const noexcept { return pCom == nullptr; }

        bool operator==(const UniqueComPtr& other) const noexcept { return pCom == other.pCom; }

        /// <summary>
        /// Retrieves a pointer to the given interface subtype and increments the reference count.
        /// Returns HRESULT S_OK if successful.
        /// </summary>
        template<typename U>
        slong As(U** ppDst) requires ComPtrType<T> && ComPtrType<U>
        {
            return QueryInterface(__uuidof(U), reinterpret_cast<void**>(ppDst));
        }

        /// <summary>
        /// Releases the COM object and returns a pointer to pointer for internal storage
        /// </summary>
        [[nodiscard]] T** operator&() noexcept { return ReleaseAndGetAddressOf(); }

        /// <summary>
        /// Returns a pointer to pointer for the underlying ComPtr storage, but does not release the interface.
        /// </summary>
        [[nodiscard]] T* const* GetAddressOf() noexcept requires ComPtrType<T> { return &pCom; }

        /// <summary>
        /// Returns a pointer to pointer for the underlying ComPtr storage, but does not release the interface.
        /// </summary>
        [[nodiscard]] T* const* const GetAddressOf() const noexcept requires ComPtrType<T> { return &pCom; }

        /// <summary>
        /// Releases the underlying interface and returns a pointer to pointer for the ComPtr storage.
        /// </summary>
        [[nodiscard]] T** ReleaseAndGetAddressOf() noexcept requires ComPtrType<T> { Release(); return &pCom; }

        /// <summary>
        /// Decrement COM pointer reference count and reset
        /// </summary>
        void Release() noexcept
        {
            if (pCom != nullptr)
            {
                // Sanity check
                static_assert(ComPtrType<T>);
                pCom->Release();
                pCom = nullptr;
            }
        }

        /// <summary>
        /// Swaps the contents of two UniqueComPtr
        /// </summary>
        void Swap(UniqueComPtr& other) noexcept { std::swap(pCom, other.pCom); }

    private:
        T* pCom;

        /// <summary>
        /// Retrieves a pointer to the given interface subtype and increments the reference count.
        /// Returns HRESULT S_OK if successful.
        /// </summary>
        template <class IdentT>
        slong QueryInterface(const IdentT& ident, void** ppDst) requires HasComQueryInterface<T, IdentT>
        {
            WV_ASSERT(pCom != nullptr);
            return pCom->QueryInterface(ident, ppDst);
        }
    };
}
