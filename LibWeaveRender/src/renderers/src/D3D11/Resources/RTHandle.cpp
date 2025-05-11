#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Resources/RTHandle.hpp"
#include "D3D11/SwapChain.hpp"
#include "D3D11/Device.hpp"
#include "D3D11/CtxImm.hpp"

using namespace Weave;
using namespace Weave::D3D11;

RTHandle::RTHandle() : ppRTV(nullptr)
{ }

RTHandle::RTHandle(
	Device& dev,
	SwapChain* pSwapChain,
	ID3D11Resource** ppRes,
	ID3D11RenderTargetView** ppRTV,
	uivec2 offset,
	vec2 scale
) :
	DeviceChild(dev),
	pSwapChain(pSwapChain),
	ppRes(ppRes),
	ppRTV(ppRTV),
	renderOffset(offset),
	renderScale(glm::clamp(scale, vec2(1E-6f), vec2(1)))
{ }

uivec3 RTHandle::GetDimensions() const
{
	return uivec3(pSwapChain->GetSize(), 1u);
}

ResourceUsages RTHandle::GetUsage() const { return ResourceUsages::Immutable; }

ResourceBindFlags RTHandle::GetBindFlags() const { return ResourceBindFlags::RenderTarget; }

ResourceAccessFlags RTHandle::GetAccessFlags() const { return ResourceAccessFlags::None; }

ID3D11Resource* RTHandle::GetResource() { return *ppRes; }

ID3D11Resource* const* RTHandle::GetResAddress() { return ppRes; }

uivec2 RTHandle::GetSize() const
{
	return pSwapChain->GetSize();
}

vec4 RTHandle::GetTexelSize() const
{
	uivec2 size = GetSize();
	return vec4(1.0f / vec2(size), size);
}

Formats RTHandle::GetFormat() const { return pSwapChain->GetBufferFormat(); }

/// <summary>
/// Sets the offset for this target in pixels
/// </summary>
void RTHandle::SetRenderOffset(ivec2 offset)
{
	this->renderOffset = glm::clamp(vec2(offset) / vec2(GetSize()), vec2(-1), vec2(1));
}

/// <summary>
/// Returns the offset set for this target in pixels
/// </summary>
ivec2 RTHandle::GetRenderOffset() const
{
	return uivec2(glm::round(renderOffset * vec2(GetSize())));
}

void RTHandle::SetRenderSize(uivec2 renderSize)
{
	const vec2 size = glm::max(GetSize(), uivec2(1));
	vec2 newSize = glm::clamp(vec2(renderSize), vec2(1), size);
	SetRenderScale(newSize / size);
}

/// <summary>
/// Returns the size of the render area in pixels
/// </summary>
uivec2 RTHandle::GetRenderSize() const
{
	const vec2 size = GetSize();
	vec2 renderSize = glm::round(renderScale * size);
	return glm::clamp(ivec2(renderSize), ivec2(1), ivec2(size));
}

/// <summary>
/// Returns combined scaled (DRS) texel size and dim fp vector.
/// XY == Texel Size; ZW == Dim
/// </summary>
vec4 RTHandle::GetRenderTexelSize() const
{
	const vec2 renderSize = GetRenderSize();
	return vec4(1.0f / renderSize.x, 1.0f / renderSize.y, renderSize);
}

/// <summary>
/// Sets the renderSize to size ratio on (0, 1].
/// </summary>
void RTHandle::SetRenderScale(vec2 scale)
{
	scale = glm::round(scale * 1E6f) * 1E-6f;
	renderScale = glm::clamp(scale, vec2(1E-6f), vec2(1));
}

/// <summary>
/// Returns the renderSize to size ratio on (0, 1].
/// </summary>
vec2 RTHandle::GetRenderScale() const
{
	return renderScale;
}

ID3D11RenderTargetView* RTHandle::GetRTV() { return *ppRTV; }

ID3D11RenderTargetView* const* const RTHandle::GetAddressRTV() { return ppRTV; }

void RTHandle::Clear(
	CtxBase& ctx,
	vec4 color
)
{
	if (ppRTV != nullptr && *ppRTV != nullptr)
		ctx.ClearRenderTarget(*this, color);
}
