#include "pch.hpp"
#include "WeaveUtils/Span.hpp"
#include "WeaveRender/D3D11.hpp"
#include "D3D11/Shaders/ShaderVariants.hpp"
#include "D3D11/Device.hpp"
#include "D3D11/ContextState.hpp"

using namespace glm;
using namespace Weave;
using namespace Weave::D3D11;

Context::Context() :
	ContextBase()
{ }

Context::Context(Device& dev, ComPtr<ID3D11DeviceContext>&& pContext) :
	ContextBase(dev, std::move(pContext))
{ }

Context::Context(Context&&) noexcept = default;

Context& Context::operator=(Context&&) noexcept = default;

Context::~Context() = default;

/// <summary>
/// Returns reference to context interface
/// </summary>
ID3D11DeviceContext& Context::Get() const { return *pContext.Get(); }

/// <summary>
/// Returns pointer to context interface
/// </summary>
ID3D11DeviceContext* Context::operator->() const { return pContext.Get(); }

static void ValidateResourceBounds(
	IColorBuffer2D& src, 
	IColorBuffer2D& dst, 
	ivec4& srcBox, 
	ivec4& dstBox, 
	bool canRescale = false)
{
	srcBox = glm::max(srcBox, ivec4(0));
	dstBox = glm::max(dstBox, ivec4(0));

	if (srcBox != ivec4(0) || dstBox != ivec4(0))
	{
		if (srcBox == ivec4(0))
			srcBox = ivec4(src.GetSize(), ivec2(0));

		if (dstBox == ivec4(0))
			dstBox = ivec4(dst.GetSize(), ivec2(0));

		 ivec2
			srcSize(srcBox.x, srcBox.y),
			dstSize(dstBox.x, dstBox.y),
			srcOffset(srcBox.z, srcBox.w),
			dstOffset(dstBox.z, dstBox.w),
			srcMax = glm::min(src.GetSize(), srcOffset + srcSize) - srcSize,
			dstMax = glm::min(dst.GetSize(), dstOffset + dstSize) - dstSize;

		 D3D_CHECK_MSG(srcMax == srcOffset,
			"[Blit] Source subregion exceeds the bounds of the source buffer.");

		 D3D_CHECK_MSG(dstMax == dstOffset,
			"[Blit] Destination subregion exceeds the bounds of the destination buffer.");

		if (!canRescale)
		{
			srcMax = srcOffset + srcSize;
			dstMax = dstOffset + dstSize;

			D3D_CHECK_MSG(srcMax.x <= dstMax.x && srcMax.y <= dstMax.y,
				"[Blit] Source subregion exceeds the bounds of the destination buffer.");
		}
	}
}

static bool CanDirectCopy(ITexture2DBase& src, ITexture2DBase& dst, const ivec4& srcBox, const ivec4& dstBox)
{
	if (srcBox != ivec4(0) || dstBox != ivec4(0))
	{
		return
			dst.GetUsage() != ResourceUsages::Immutable &&
			srcBox.x == dstBox.x && srcBox.y == dstBox.y &&
			src.GetFormat() == dst.GetFormat();
	}
	else
	{
		return 
			dst.GetUsage() != ResourceUsages::Immutable &&
			src.GetSize() == dst.GetSize() &&
			src.GetFormat() == dst.GetFormat();
	}
}

static void CopySubresource(Context& ctx, ITexture2DBase& src, ITexture2DBase& dst, ivec4& srcBox, ivec4& dstBox)
{
	if (srcBox != ivec4(0) || dstBox != ivec4(0))
	{ 
		ValidateResourceBounds(src, dst, srcBox, dstBox);

		D3D11_BOX srcBB = 
		{
			.left = (uint)std::max(srcBox.z, 0),
			.top = (uint)std::max(srcBox.w, 0),
			.front = 0u,
			.right = (uint)std::max(srcBox.z + srcBox.x, 0),
			.bottom = (uint)std::max(srcBox.w + srcBox.y, 0),
			.back = 1u
		};

		ctx->CopySubresourceRegion(
			dst.GetResource(), 0u,
			(uint)dstBox.z, (uint)dstBox.w, 0u, // Dst start bounds
			src.GetResource(), 0u, 
			&srcBB
		);
	}
	else
		ctx->CopyResource(dst.GetResource(), src.GetResource());
}

/// <summary>
/// Copies the contents of one texture to another
/// </summary>
void Context::Blit(IResizeableTexture2D& src, IRWTexture2D& dst)
{
	Blit((ITexture2D&)src, dst, ivec4(src.GetRenderSize(), src.GetRenderOffset()));
}

/// <summary>
/// Copies the contents of one texture to another
/// </summary>
void Context::Blit(ITexture2D& src, IRWTexture2D& dst, ivec4 srcBox)
{
	if (srcBox == ivec4(0))
		srcBox = ivec4(src.GetSize(), 0, 0);

	ivec4 dstBox(dst.GetRenderSize(), dst.GetRenderOffset());

	if (CanDirectCopy(src, dst, srcBox, dstBox))
	{
		CopySubresource(*this, src, dst, srcBox, dstBox);
		return;
	}

	if (srcBox.x == dstBox.x && srcBox.y == dstBox.y)
	{
		ValidateResourceBounds(src, dst, srcBox, dstBox);

		Renderer& renderer = GetRenderer();
		ComputeInstance& cs = renderer.GetDefaultCompute("TexCopy2D");

		cs.SetTexture("SrcTex", src);
		cs.SetRWTexture("DstTex", dst);
		cs.SetConstant("SrcOffset", ivec2(srcBox.z, srcBox.w));
		cs.SetConstant("DstOffset", ivec2(dstBox.z, dstBox.w));
		cs.Dispatch(*this, { dstBox.x, dstBox.y });
	}
	else
	{ 
		ValidateResourceBounds(src, dst, srcBox, dstBox, true);

		Renderer& renderer = GetRenderer();
		ComputeInstance& cs = renderer.GetDefaultCompute("TexCopyScaledSamp2D");

		const ivec2 srcRenderSize = vec2(srcBox.x, srcBox.y);
		const vec2 scale = vec2(srcRenderSize) / vec2(src.GetSize()),
			offset = vec2(srcBox.z, srcBox.w) * vec2(src.GetTexelSize());

		cs.SetTexture("SrcTex", src);
		cs.SetRWTexture("DstTex", dst);
		cs.SetSampler("Samp", renderer.GetDefaultSampler("LinearBorder"));
		cs.SetConstant("Scale", scale);
		cs.SetConstant("Offset", offset);
		cs.SetConstant("SrcOffset", ivec2(srcBox.z, srcBox.w));
		cs.SetConstant("DstOffset", ivec2(dstBox.z, dstBox.w));
		cs.SetConstant("DstTexelSize", dst.GetRenderTexelSize());
		cs.Dispatch(*this, { dstBox.x, dstBox.y });
	}
}

/// <summary>
/// Copies the contents of one texture to another
/// </summary>
void Context::Blit(ITexture2D& src, IResizeableTexture2D& dst, ivec4 srcBox)
{
	Blit(
		src,
		dst,
		srcBox,
		ivec4(dst.GetRenderSize(), dst.GetRenderOffset())
	);
}

/// <summary>
/// Copies the contents of one texture to another
/// </summary>
void Context::Blit(IResizeableTexture2D& src, ITexture2D& dst, ivec4 dstBox)
{
	Blit(
		src,
		dst,
		ivec4(src.GetRenderSize(), src.GetRenderOffset()),
		dstBox
	);
}

/// <summary>
/// Copies the contents of one texture to another
/// </summary>
void Context::Blit(IResizeableTexture2D& src, IResizeableTexture2D& dst)
{
	Blit(
		src, 
		dst, 
		ivec4(src.GetRenderSize(), src.GetRenderOffset()), 
		ivec4(dst.GetRenderSize(), dst.GetRenderOffset())
	);
}

/// <summary>
/// Copies the contents of one texture to another
/// </summary>
void Context::Blit(ITexture2DBase& src, ITexture2DBase& dst, ivec4 srcBox, ivec4 dstBox)
{
	D3D_CHECK_MSG(CanDirectCopy(src, dst, srcBox, dstBox), "Failed to copy texture. Destination incompatible with source.");
	ValidateResourceBounds(src, dst, srcBox, dstBox);
	CopySubresource(*this, src, dst, srcBox, dstBox);
}

/// <summary>
/// Copies the contents of a texture to a render target
/// </summary>
void Context::Blit(IResizeableTexture2D& src, IRenderTarget& dst)
{
	Blit(src, dst, ivec4(src.GetRenderSize(), src.GetRenderOffset()));
}

/// <summary>
/// Copies the contents of one texture to another
/// </summary>
void Context::Blit(ITexture2D& src, IRenderTarget& dst, ivec4 srcBox)
{
	ivec4 dstBox(dst.GetRenderSize(), dst.GetRenderOffset());

	if (srcBox == ivec4(0))
		srcBox = ivec4(src.GetSize(), 0, 0);
	else
		ValidateResourceBounds(src, dst, srcBox, dstBox, true);

	Renderer& renderer = GetRenderer();
	Mesh& quad = renderer.GetDefaultMesh("FSQuad");
	Material& quadFX = renderer.GetDefaultMaterial("PosTextured2D");
	Viewport vp = 
	{
		.offset = dst.GetRenderOffset(),
		.size = dst.GetRenderSize(),
		.zDepth = vec2(0)
	};
	bool wasRtvSet = false;
	bool wasVpSet = false;

	// Manually set render target if it isn't already set
	if (dst.GetRTV() != pState->renderTargets[0])
	{
		pContext->OMSetRenderTargets(1, dst.GetAddressRTV(), nullptr);
		wasRtvSet = true;
	}
	// Manually set VP if it isn't set
	if (vp != pState->viewports[0])
	{
		pContext->RSSetViewports(1, (D3D11_VIEWPORT*)&vp);
		wasVpSet = true;
	}

	const ivec2 srcRenderSize = vec2(srcBox.x, srcBox.y);
	const vec2 scale = vec2(srcRenderSize) / vec2(src.GetSize()),
		offset = vec2(srcBox.z, srcBox.w) * vec2(src.GetTexelSize());

	if (srcRenderSize == dst.GetRenderSize())
		quadFX.SetSampler("samp", renderer.GetDefaultSampler("PointBorder"));
	else
		quadFX.SetSampler("samp", renderer.GetDefaultSampler("LinearClamp"));

	quadFX.SetConstant("Scale", scale);
	quadFX.SetConstant("Offset", offset);
	quadFX.SetTexture("tex", src);

	Draw(quad, quadFX);

	// Manually restore previous state if changed
	if (wasRtvSet)
		pContext->OMSetRenderTargets(pState->rtCount, pState->renderTargets.GetData(), pState->GetDepthStencilView());
	if (wasVpSet)
		pContext->RSSetViewports(pState->vpCount, (D3D11_VIEWPORT*)pState->viewports.GetData());
}


