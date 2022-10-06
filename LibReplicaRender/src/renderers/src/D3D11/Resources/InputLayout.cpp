#include "ReplicaInternalD3D11.hpp"

using namespace Replica::D3D11;
using namespace Microsoft::WRL;

InputLayout::InputLayout() noexcept : DeviceChild() { }

InputLayout::InputLayout(Device& dev, 
	const byte* pVS,
	const size_t srcSize,
	const IDynamicArray<IAElement>& description
) : DeviceChild(dev)
{ 
	GFX_THROW_FAILED(dev->CreateInputLayout(
		reinterpret_cast<const D3D11_INPUT_ELEMENT_DESC*>(description.GetPtr()),
		(uint)description.GetLength(),
		pVS,
		(uint)srcSize,
		&pLayout
	));
}

ID3D11InputLayout* InputLayout::Get() const { return pLayout.Get(); };

void InputLayout::Bind(Context& ctx)
{
	ctx->IASetInputLayout(Get());
}

IAElement::IAElement(string_view semantic,
	Formats format,
	UINT semanticIndex,
	UINT iaSlot,
	InputClass slotClass,
	UINT instStepRate,
	UINT offset
) :
	SemanticName(semantic.data()),
	SemanticIndex(semanticIndex),
	Format(format),
	InputSlot(iaSlot),
	AlignedByteOffset(offset),
	InputSlotClass(slotClass),
	InstanceDataStepRate(instStepRate)
{ }

IAElement::IAElement(string_view semantic,
	UINT semanticIndex,
	Formats format,
	UINT iaSlot,
	InputClass slotClass,
	UINT instStepRate,
	UINT offset
) :
	SemanticName(semantic.data()),
	SemanticIndex(semanticIndex),
	Format(format),
	InputSlot(iaSlot),
	AlignedByteOffset(offset),
	InputSlotClass(slotClass),
	InstanceDataStepRate(instStepRate)
{ }