#pragma once
#include "../D3D11Utils.hpp"
#include "WeaveUtils/Span.hpp"

namespace Weave::D3D11
{
	class Sampler;
	class ComputeBuffer;
	class ResourceSet;
	struct ConstantGroupBuffer;

	/// <summary>
	/// Stores a pointer to a resource view paired with a stringID
	/// </summary>
	template<typename T>
	struct ResNamePair
	{
		uint stringID;
		T* pView;
	};

	/// <summary>
	/// Stores mappings for string ID <-> view lookup
	/// </summary>
	template<typename T>
	struct ResViewBuffer
	{
		UniqueVector<ResNamePair<T>> data;
		// stringID -> index
		std::unordered_map<uint, uint> stringViewMap;

		uint GetOrAddResource(uint stringID)
		{
			const auto& it = stringViewMap.find(stringID);

			if (it != stringViewMap.end())
			{
				return it->second;
			}
			else
			{
				stringViewMap.emplace(stringID, (uint)data.GetLength());
				data.EmplaceBack().stringID = stringID;
				return (uint)(data.GetLength() - 1);
			}
		}

		/// <summary>
		/// Updates or adds the resource with the given string ID
		/// </summary>
		void SetResource(uint stringID, T* pValue)
		{
			data[GetOrAddResource(stringID)].pView = pValue;
		}

		/// <summary>
		/// Resets the collection
		/// </summary>
		void Clear()
		{
			data.Clear();
			stringViewMap.clear();
		}
	};

	/// <summary>
	/// Descriptor for a member in a constant buffer
	/// </summary>
	struct ConstantDesc
	{
		uint stringID;
		uint size;
		// Position in byte vector
		uint offset;
	};

	/// <summary>
	/// Wrapper for setting or getting the last constant set with a given name in a ResourceSet.
	/// Type size is enforced, but is otherwise unsafe.
	/// </summary>
	class ConstantDescHandle
	{
	public:
		MAKE_MOVE_ONLY(ConstantDescHandle);

		ConstantDescHandle();

		ConstantDescHandle(uint stringID, uint size, ResourceSet& resources);

		/// <summary>
		/// Returns locally unique identifier for the constant's name in the ResourceSet
		/// </summary>
		uint GetStringID() const;

		/// <summary>
		/// Returns the size of the constant
		/// </summary>
		uint GetSize() const;

		/// <summary>
		/// Returns temporary span to the constant's storage. May be invalidated between 
		/// modifications.
		/// </summary>
		Span<byte> GetValue();

		/// <summary>
		/// Writes the given value to the constant.
		/// </summary>
		void SetValue(const Span<byte>& value);

	private:
		ConstantGroupBuffer* pConstants;
		ConstantDesc desc;
	};

	/// <summary>
	/// Strongly typed wrapper template for a constant buffer member.
	/// </summary>
	template<typename T>
	class ConstantHandle : public ConstantDescHandle
	{
	public:
		MAKE_MOVE_ONLY(ConstantHandle);

		ConstantHandle() = default;

		ConstantHandle(uint stringID, ResourceSet& resources) :
			ConstantDescHandle(stringID, (uint)sizeof(T), resources)
		{ }

		/// <summary>
		/// Returns a copy of the constant as a strongly typed value
		/// </summary>
		explicit operator T()
		{
			return reinterpret_cast<T&>(*GetValue().GetData());
		}

		/// <summary>
		/// Sets the constant to the given value
		/// </summary>
		ConstantHandle& operator=(const T& value)
		{
			SetValue(Span<byte>((byte*)&value, sizeof(T)));
			return *this;
		}
	};

	/// <summary>
	/// Strongly typed wrapper template mapping a resource subtype to a compatible slot view
	/// </summary>
	template<typename ResT, typename SlotT>
	requires std::is_convertible_v<ResT*, SlotT*>
	class ResourceHandle
	{
	public:
		MAKE_MOVE_ONLY(ResourceHandle);

		ResourceHandle() : 
			pBuf(nullptr), stringID(-1), index(-1)
		{ }

		ResourceHandle(uint stringID, ResViewBuffer<SlotT>& buf) :
			pBuf(&buf),
			stringID(stringID),
			index(buf.GetOrAddResource(stringID))
		{ }

		/// <summary>
		/// Returns string ID corresponding to the resource's name
		/// </summary>
		uint GetStringID() const { return stringID; }

		/// <summary>
		/// Returns a temporary pointer to the pointer's storage. May be invalidated after accessing 
		/// other resources.
		/// </summary>
		SlotT* GetValue() const { return pBuf->data[index].pView; }

		/// <summary>
		/// Assigns the given value to the underlying slot buffer
		/// </summary>
		void SetValue(SlotT* pValue) { pBuf->data[index].pView = pValue; }

		/// <summary>
		/// Returns a reference to the resource mapped to the slot
		/// </summary>
		explicit operator ResT&() const 
		{ 
			SlotT* pValue = GetValue();
			WV_ASSERT(pValue != nullptr);
			return static_cast<ResT&>(*pValue);
		}

		/// <summary>
		/// Assigns the given resource subtype to the slot
		/// </summary>
		ResourceHandle& operator=(ResT& value) { SetValue(&value); return *this; }

	private:
		ResViewBuffer<SlotT>* pBuf;
		uint stringID;
		uint index;
	};
}