#include <d3dcompiler.h>
#include "D3D11/dev/Device.hpp"
#include "D3D11/dev/VertexShader.hpp"

using namespace Replica::D3D11;
using namespace Microsoft::WRL;

VertexShader::VertexShader(Device& dev, const LPCWSTR file, const std::initializer_list<IAElement>& layout) :
	DeviceChild(&dev)
{
	ComPtr<ID3DBlob> vsBlob;
	GFX_THROW_FAILED(D3DReadFileToBlob(file, &vsBlob));
	GFX_THROW_FAILED(dev.Get()->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &pVS));

	this->layout = InputLayout(dev, vsBlob, layout);
}

ID3D11VertexShader* VertexShader::Get() const { return pVS.Get(); }

const InputLayout& VertexShader::GetLayout() const { return layout; }

void VertexShader::Bind(Context& ctx)
{
	ctx.Get()->VSSetShader(Get(), nullptr, 0);
	layout.Bind();
}