#pragma once
#include "DeviceChild.hpp"
#include "Formats.hpp"
#include "ShaderDataHandles.hpp"

namespace Replica::D3D11
{
	using Effects::IOElementDef;

	/// <summary>
	/// Defines the layout of vertex data supplied to the input-assembler
	/// </summary>
	class InputLayout : public DeviceChild
	{
	public:

		InputLayout() noexcept;

		/// <summary>
		/// Constructs layout definition associated with given shader bytecode
		/// </summary>
		InputLayout(
			Device& dev,
			const byte* pVS,
			size_t srcSize,
			IOLayoutHandle layout
		);

		/// <summary>
		/// Returns pointer to COM layout interface. Null if uninitialized.
		/// </summary>
		ID3D11InputLayout* Get() const;

		/// <summary>
		/// Binds the layout to the input assembler for the associated context
		/// </summary>
		void Bind(Context& ctx) const;

	private:
		ComPtr<ID3D11InputLayout> pLayout;
	};
}