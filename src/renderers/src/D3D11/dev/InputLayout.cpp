#include "D3D11/dev/InputLayout.hpp"

using namespace Replica::D3D11;
using namespace Microsoft::WRL;

InputLayout::InputLayout(const Device& dev, 
	const ComPtr<ID3DBlob>& vsBlob, 
	const std::initializer_list<IAElement>& layout
) :
	pLayout(dev.CreateInputLayout(UniqueArray(layout), vsBlob))
{ }

ID3D11InputLayout* InputLayout::Get() const { return pLayout.Get(); };

IAElement::IAElement(LPCSTR semantic,
	Formats format,
	UINT semanticIndex,
	UINT iaSlot,
	InputClass slotClass,
	UINT instStepRate,
	UINT offset
) :
	SemanticName(semantic),
	SemanticIndex(semanticIndex),
	Format(format),
	InputSlot(iaSlot),
	AlignedByteOffset(offset),
	InputSlotClass(slotClass),
	InstanceDataStepRate(instStepRate)
{ }

IAElement::IAElement(LPCSTR semantic,
	UINT semanticIndex,
	Formats format,
	UINT iaSlot,
	InputClass slotClass,
	UINT instStepRate,
	UINT offset
) :
	SemanticName(semantic),
	SemanticIndex(semanticIndex),
	Format(format),
	InputSlot(iaSlot),
	AlignedByteOffset(offset),
	InputSlotClass(slotClass),
	InstanceDataStepRate(instStepRate)
{ }