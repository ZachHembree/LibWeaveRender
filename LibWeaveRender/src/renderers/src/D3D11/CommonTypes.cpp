#include "pch.hpp"
#include "D3D11/CommonTypes.hpp"
#include "D3D11/InternalD3D11.hpp"

using namespace Weave;
using namespace Weave::D3D11;

static_assert(sizeof(MappedSubresource) == sizeof(D3D11_MAPPED_SUBRESOURCE), "Weave D3D struct alignment does not match internal data type.");

D3D11_MAPPED_SUBRESOURCE* MappedSubresource::GetD3DPtr()
{
	return reinterpret_cast<D3D11_MAPPED_SUBRESOURCE*>(this);
}

static_assert(sizeof(BufferDesc) == sizeof(D3D11_BUFFER_DESC), "Weave D3D struct alignment does not match internal data type.");

D3D11_BUFFER_DESC* BufferDesc::GetD3DPtr()
{
	return reinterpret_cast<D3D11_BUFFER_DESC*>(this);
}

static_assert(sizeof(SampleDesc) == sizeof(DXGI_SAMPLE_DESC), "Weave D3D struct alignment does not match internal data type.");

DXGI_SAMPLE_DESC* SampleDesc::GetD3DPtr()
{
	return reinterpret_cast<DXGI_SAMPLE_DESC*>(this);
}

static_assert(sizeof(Texture2DDesc) == sizeof(D3D11_TEXTURE2D_DESC), "Weave D3D struct alignment does not match internal data type.");

D3D11_TEXTURE2D_DESC* Texture2DDesc::GetD3DPtr()
{
	return reinterpret_cast<D3D11_TEXTURE2D_DESC*>(this);
}
