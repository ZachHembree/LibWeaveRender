#include "pch.hpp"
#include "D3D11/Resources/ResourceSet.hpp"

using namespace Weave;
using namespace Weave::D3D11;

ConstantDescHandle::ConstantDescHandle() {}

ConstantDescHandle::ConstantDescHandle(uint stringID, uint size, ResourceSet& resources) :
	pConstants(&resources.constants),
	desc(resources.constants.GetOrAddValue(stringID, size))
{}

uint ConstantDescHandle::GetStringID() const { return desc.stringID; }

uint ConstantDescHandle::GetSize() const { return desc.size; }

Span<byte> ConstantDescHandle::GetValue()
{
	byte* pValue = &pConstants->data[desc.offset];
	return Span(pValue, desc.size);
}

void ConstantDescHandle::SetValue(const Span<byte>& value)
{
	byte* pValue = &pConstants->data[desc.offset];
	memcpy(pValue, value.GetData(), value.GetLength());
}
