#pragma once
#include "DeviceChild.hpp"
#include "../CommonEnums.hpp"
#include "WeaveEffects/ShaderDataHandles.hpp"

namespace Weave::D3D11
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
			std::optional<IOLayoutHandle> layout
		);

		/// <summary>
		/// Returns pointer to COM layout interface. Null if uninitialized.
		/// </summary>
		ID3D11InputLayout* Get() const;

	private:
		ComPtr<ID3D11InputLayout> pLayout;
	};
}