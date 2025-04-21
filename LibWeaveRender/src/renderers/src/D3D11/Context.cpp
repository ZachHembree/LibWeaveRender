#include "pch.hpp"
#include "D3D11/Device.hpp"
#include "D3D11/ContextState.hpp"
#include "D3D11/Resources/MappedBufferHandle.hpp"

using namespace glm;
using namespace Weave;
using namespace Weave::D3D11;

Context::Context() :
	ContextBase()
{ }

Context::Context(Device& dev, ComPtr<ID3D11DeviceContext1>&& pCtx) :
	ContextBase(dev, std::move(pCtx))
{ 
	D3D_ASSERT_MSG(GetIsImmediate(), "An immediate context wrapper cannot point to a deferred context.");
}

Context::Context(Context&&) noexcept = default;

Context& Context::operator=(Context&&) noexcept = default;

Context::~Context() = default;

MappedBufferHandle Context::GetMappedBufferHandle(IBuffer& buf)
{
	D3D11_MAP mapFlags = (D3D11_MAP)0;

	if ((uint)(buf.GetAccessFlags() & ResourceAccessFlags::Read))
		mapFlags = (D3D11_MAP)(mapFlags | D3D11_MAP_READ);

	if ((uint)(buf.GetAccessFlags() & ResourceAccessFlags::Write))
		mapFlags = (D3D11_MAP)(mapFlags | D3D11_MAP_WRITE);

	D3D11_MAPPED_SUBRESOURCE msr;
	D3D_CHECK_HR(pCtx->Map(
		buf.GetResource(),
		0u,
		mapFlags,
		0u,
		&msr
	));

	return MappedBufferHandle(buf, msr);
}
