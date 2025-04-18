#pragma once
#include "WeaveEffects/ShaderData.hpp"
#include "D3D11/InternalD3D11.hpp"

namespace Weave::D3D11
{
	/// <summary>
	/// Binds one or more constant buffers to a specific shader stage in the Direct3D 11 pipeline.
	/// This is a convenience function that abstracts the stage-specific SetConstantBuffers calls
	/// (e.g., VSSetConstantBuffers, PSSetConstantBuffers) based on the provided 'stage' enum.
	/// </summary>
	void SetConstantBuffers(ShadeStages stage, ID3D11DeviceContext* pCtx, UINT startSlot, UINT numBuffers, ID3D11Buffer* const* pCBufs);

	/// <summary>
	/// Binds one or more sampler states to a specific shader stage in the Direct3D 11 pipeline.
	/// This function abstracts the stage-specific SetSamplers calls (e.g., VSSetSamplers, PSSetSamplers).
	/// </summary>
	void SetSamplers(ShadeStages stage, ID3D11DeviceContext* pCtx, UINT startSlot, UINT count, ID3D11SamplerState* const* pSamplers);

	/// <summary>
	/// Binds one or more shader resource views (SRVs) to a specific shader stage in the Direct3D 11 pipeline.
	/// SRVs provide shader access to resources like textures and buffers. This function abstracts
	/// the stage-specific SetShaderResources calls (e.g., VSSetShaderResources, PSSetShaderResources).
	/// </summary>
	void SetShaderResources(ShadeStages stage, ID3D11DeviceContext* pCtx, UINT startSlot, UINT count, ID3D11ShaderResourceView* const* pSRVs);

	/// <summary>
	/// Binds a specific shader object (Vertex, Pixel, Compute, etc.) to its corresponding stage in the Direct3D 11 pipeline.
	/// This function abstracts the stage-specific SetShader calls (e.g., VSSetShader, PSSetShader).
	/// </summary>
	void SetShader(ShadeStages stage, ID3D11DeviceContext* pCtx, ID3D11DeviceChild* pShader, ID3D11ClassInstance* const* ppClassInstances, UINT numClassInstances);
}