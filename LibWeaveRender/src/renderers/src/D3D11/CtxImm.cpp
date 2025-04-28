#include "pch.hpp"
#include "D3D11/Device.hpp"
#include "D3D11/CtxImm.hpp"
#include "D3D11/ContextState.hpp"
#include "D3D11/Resources/MappedBufferHandle.hpp"
#include "D3D11/SwapChain.hpp"

using namespace glm;
using namespace Weave;
using namespace Weave::D3D11;

CtxImm::CtxImm() :
	CtxBase()
{ }

CtxImm::CtxImm(Device& dev, ComPtr<ID3D11DeviceContext1>&& pCtx) :
	CtxBase(dev, std::move(pCtx))
{ 
	D3D_ASSERT_MSG(GetIsImmediate(), "An immediate context wrapper cannot point to a deferred context.");
}

CtxImm::CtxImm(CtxImm&&) noexcept = default;

CtxImm& CtxImm::operator=(CtxImm&&) noexcept = default;

CtxImm::~CtxImm() = default;

MappedBufferHandle CtxImm::GetMappedBufferHandle(IBuffer& buf)
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

void CtxImm::EndFrame()
{
	Span<IRenderTarget*> nullRT;
	pState->TryUpdateRenderTargets(nullRT, 0);

	for (int i = 0; i < g_ShadeStageCount; i++)
		pState->TrySetShader((ShadeStages)i, nullptr);
}
