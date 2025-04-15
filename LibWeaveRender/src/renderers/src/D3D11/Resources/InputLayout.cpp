#include "pch.hpp"
#include "WeaveUtils/StringIDMap.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Resources/InputLayout.hpp"
#include "D3D11/Context.hpp"
#include "D3D11/Device.hpp"

using namespace Weave;
using namespace Weave::D3D11;
using namespace Microsoft::WRL;

static DXGI_FORMAT GetFormat(const IOElementDef& def)
{
	if (def.dataType == D3D_REGISTER_COMPONENT_FLOAT32)
	{
		switch (def.componentCount) 
		{
			case 1: return DXGI_FORMAT_R32_FLOAT;
			case 2: return DXGI_FORMAT_R32G32_FLOAT;
			case 3: return DXGI_FORMAT_R32G32B32_FLOAT;
			case 4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
	}
	else if (def.dataType == D3D_REGISTER_COMPONENT_UINT32)
	{
		switch (def.componentCount)
		{
			case 1: return DXGI_FORMAT_R32_UINT;
			case 2: return DXGI_FORMAT_R32G32_UINT;
			case 3: return DXGI_FORMAT_R32G32B32_UINT;
			case 4: return DXGI_FORMAT_R32G32B32A32_UINT;
		}
	}
	else if (def.dataType == D3D_REGISTER_COMPONENT_SINT32)
	{
		switch (def.componentCount)
		{
			case 1: return DXGI_FORMAT_R32_SINT;
			case 2: return DXGI_FORMAT_R32G32_SINT;
			case 3: return DXGI_FORMAT_R32G32B32_SINT;
			case 4: return DXGI_FORMAT_R32G32B32A32_SINT;
		}
	}

	return DXGI_FORMAT_UNKNOWN;
}

InputLayout::InputLayout() noexcept : DeviceChild() { }

InputLayout::InputLayout(
	Device& dev,
	const byte* pVS,
	size_t srcSize,
	std::optional<IOLayoutHandle> desc
) : DeviceChild(dev)
{
	if (desc.has_value())
	{
		D3D_ASSERT_MSG(desc->GetLength() <= D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT, "Vertex input layout size limit exceeded");
		D3D11_INPUT_ELEMENT_DESC descBuf[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

		for (int i = 0; i < desc->GetLength(); i++)
		{
			descBuf[i].Format = GetFormat((*desc)[i]);
			descBuf[i].SemanticIndex = (*desc)[i].semanticIndex;
			descBuf[i].SemanticName = desc->GetStringMap().GetString((*desc)[i].semanticID).data();

			descBuf[i].InputSlot = 0u;
			descBuf[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			descBuf[i].InstanceDataStepRate = 0u;
			descBuf[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		}

		D3D_CHECK_HR_MSG(dev->CreateInputLayout(
			reinterpret_cast<const D3D11_INPUT_ELEMENT_DESC*>(&descBuf),
			(uint)desc->GetLength(),
			pVS,
			(uint)srcSize,
			&pLayout
		));
	}
}

ID3D11InputLayout* InputLayout::Get() const { return pLayout.Get(); };

void InputLayout::Bind(Context& ctx) const
{
	ctx->IASetInputLayout(Get());
}