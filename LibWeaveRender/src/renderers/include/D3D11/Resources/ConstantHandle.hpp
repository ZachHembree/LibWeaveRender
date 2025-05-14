#pragma once
#include "../D3D11Utils.hpp"
#include "WeaveUtils/Span.hpp"

namespace Weave::D3D11
{
	class ResourceSet;
	struct ConstantGroup;

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
		MAKE_DEF_MOVE_COPY(ConstantDescHandle);

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
		ConstantGroup* pConstants;
		ConstantDesc desc;
	};

	/// <summary>
	/// Strongly typed wrapper template for a constant buffer member.
	/// </summary>
	template<typename T>
	class ConstantHandle : public ConstantDescHandle
	{
	public:
		MAKE_DEF_MOVE_COPY(ConstantHandle);

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
}