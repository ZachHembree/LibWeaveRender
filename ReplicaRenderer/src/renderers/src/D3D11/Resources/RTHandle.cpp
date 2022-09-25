#include "ReplicaInternalD3D11.hpp"

using namespace Replica;
using namespace Replica::D3D11;

RTHandle::RTHandle() : ppRTV(nullptr)
{ }

RTHandle::RTHandle(
	Device& dev,
	SwapChain* pSwapChain,
	ID3D11RenderTargetView** ppRTV,
	ivec2 offset,
	vec2 scale
) :
	DeviceChild(dev),
	pSwapChain(pSwapChain),
	ppRTV(ppRTV),
	offset(offset),
	renderScale(glm::clamp(scale, vec2(1E-6), vec2(1)))
{ }

/// <summary>
/// Returns the size of the underlying buffer
/// </summary>
ivec2 RTHandle::GetSize() const
{
	return pSwapChain->GetSize();
}

/// <summary>
/// Returns the offset set for this target in pixels
/// </summary>
ivec2 RTHandle::GetOffset() const
{
	offset = glm::clamp(offset, ivec2(0), GetSize());
	return offset;
}

void RTHandle::SetRenderSize(ivec2 renderSize)
{
	const vec2 size = glm::max(GetSize(), ivec2(1));
	vec2 newSize = glm::clamp(vec2(renderSize), vec2(1), size);
	SetRenderScale(newSize / size);
}

/// <summary>
/// Returns the size of the render area in pixels
/// </summary>
ivec2 RTHandle::GetRenderSize() const
{
	const vec2 size = GetSize();
	vec2 renderSize = glm::ceil(renderScale * size);
	return glm::clamp(ivec2(renderSize), ivec2(1), ivec2(size));
}

/// <summary>
/// Sets the renderSize to size ratio on (0, 1].
/// </summary>
void RTHandle::SetRenderScale(vec2 scale)
{
	renderScale = glm::clamp(scale, vec2(1E-6f), vec2(1));
}

/// <summary>
/// Returns the renderSize to size ratio on (0, 1].
/// </summary>
vec2 RTHandle::GetRenderScale()
{
	return renderScale;
}

ID3D11RenderTargetView* RTHandle::GetRTV() { return *ppRTV; }

ID3D11RenderTargetView** const RTHandle::GetAddressRTV() { return ppRTV; }

void RTHandle::Clear(
	Context& ctx,
	vec4 color
)
{
	if (ppRTV != nullptr)
	{ 
		ctx->ClearRenderTargetView(*ppRTV, reinterpret_cast<float*>(&color));
	}
}
