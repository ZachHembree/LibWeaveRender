#pragma once
#include "WeaveUtils/Span.hpp"
#include "WeaveEffects/ShaderData.hpp"
#include "D3D11/InternalD3D11.hpp"

namespace Weave::D3D11
{
	class Sampler;
	class IShaderResource;
	class IUnorderedAccess;
	class IRenderTarget;
	class IDepthStencil;

	/// <summary>
	/// Binds one or more constant buffers to a specific shader stage in the Direct3D 11 pipeline.
	/// This is a convenience function that abstracts the stage-specific SetConstantBuffers calls
	/// (e.g., VSSetConstantBuffers, PSSetConstantBuffers) based on the provided 'stage' enum.
	/// </summary>
	void SetConstantBuffers(ID3D11DeviceContext* pCtx, ShadeStages stage, uint startSlot, const Span<ID3D11Buffer*> buffers);

	/// <summary>
	/// Binds one or more sampler states to a specific shader stage in the Direct3D 11 pipeline.
	/// This function abstracts the stage-specific SetSamplers calls (e.g., VSSetSamplers, PSSetSamplers).
	/// </summary>
	void SetSamplers(ID3D11DeviceContext* pCtx, ShadeStages stage, uint startSlot, const Span<ID3D11SamplerState*> res);

	/// <summary>
	/// Binds one or more shader resource views (SRVs) to a specific shader stage in the Direct3D 11 pipeline.
	/// SRVs provide shader access to resources like textures and buffers. This function abstracts
	/// the stage-specific SetShaderResources calls (e.g., VSSetShaderResources, PSSetShaderResources).
	/// </summary>
	void SetShaderResources(ID3D11DeviceContext* pCtx, ShadeStages stage, uint startSlot, const Span<ID3D11ShaderResourceView*> res);

	/// <summary>
	/// Binds one or more unordered access views (UAVs) to the D3D11 compute pipeline, providing random read-write access
	/// to textures and buffers.
	/// </summary>
	void CSSetUnorderedAccessViews(ID3D11DeviceContext* pCtx, uint startSlot, const Span<ID3D11UnorderedAccessView*> uavs, const uint* pInitialCounts);

	/// <summary>
	/// Binds a specific shader object (Vertex, Pixel, Compute, etc.) to its corresponding stage in the Direct3D 11 pipeline.
	/// This function abstracts the stage-specific SetShader calls (e.g., VSSetShader, PSSetShader).
	/// </summary>
	void SetShader(ID3D11DeviceContext* pCtx, ShadeStages stage, 
		ID3D11DeviceChild* pShader, ID3D11ClassInstance* const* ppClassInstances, UINT numClassInstances);

	/// <summary>
	/// Binds one or more render targets with or without a depth stencil using the given context.
	/// </summary>
	void OMSetRenderTargets(ID3D11DeviceContext* pCtx, const Span<ID3D11RenderTargetView*> rtvs, ID3D11DepthStencilView* pDSV);
}