#include "D3D11/dev/VertexShader.hpp"
#include <d3dcompiler.h>

using namespace Replica::D3D11;
using namespace Microsoft::WRL;

VertexShader::VertexShader(const Device& dev, const LPCWSTR file, const std::initializer_list<IAElement>& layout) :
	Child(&dev)
{
	ComPtr<ID3DBlob> vsBlob;
	GFX_THROW_FAILED(D3DReadFileToBlob(file, &vsBlob));
	GFX_THROW_FAILED(dev.Get()->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &pVS));

	this->layout = InputLayout(dev, vsBlob, layout);
}

ID3D11VertexShader* VertexShader::Get() const { return pVS.Get(); }

const InputLayout& VertexShader::GetLayout() const { return layout; }

void VertexShader::Bind()
{
	pDev->GetContext()->VSSetShader(Get(), nullptr, 0);
	layout.Bind();
}