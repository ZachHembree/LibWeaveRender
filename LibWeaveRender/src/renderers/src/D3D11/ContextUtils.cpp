#include "pch.hpp"
#include "D3D11/ContextUtils.hpp"
#include "D3D11/Resources/ResourceBase.hpp"
#include "D3D11/Resources/Sampler.hpp"

using namespace Weave;
using namespace Weave::D3D11;

static_assert(((int)ShadeStages::Vertex == 0), "Vertex enum value does not match index");
static_assert(((int)ShadeStages::Hull == 1), "Hull enum value does not match index");
static_assert(((int)ShadeStages::Domain == 2), "Domain enum value does not match index");
static_assert(((int)ShadeStages::Geometry == 3), "Geometry enum value does not match index");
static_assert(((int)ShadeStages::Pixel == 4), "Pixel enum value does not match index");
static_assert(((int)ShadeStages::Compute == 5), "Compute enum value does not match index");
static_assert((g_ShadeStageCount == 6), "Shading stage count incorrect");

namespace Weave::D3D11
{
	/*
		Constant buffer setters
	*/
	using SetConstFunc = void (ID3D11DeviceContext::*)(UINT, UINT, ID3D11Buffer* const*);
	static constexpr std::array<SetConstFunc, g_ShadeStageCount> s_ConstBufSetters =
	{
		&ID3D11DeviceContext::VSSetConstantBuffers,
		&ID3D11DeviceContext::HSSetConstantBuffers,
		&ID3D11DeviceContext::DSSetConstantBuffers,
		&ID3D11DeviceContext::GSSetConstantBuffers,
		&ID3D11DeviceContext::PSSetConstantBuffers,
		&ID3D11DeviceContext::CSSetConstantBuffers
	};

	void SetConstantBuffers(ID3D11DeviceContext* pCtx, ShadeStages stage, uint startSlot, const Span<ID3D11Buffer*> buffers)
	{
		D3D_ASSERT_MSG(pCtx != nullptr, "Attempted to dereference a null device context");
		D3D_ASSERT_MSG((startSlot + buffers.GetLength()) <= D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT,
			"Constant buffer limit exceeded: {}", (startSlot + buffers.GetLength()));

		(pCtx->*s_ConstBufSetters[(int)stage])(startSlot, (uint)buffers.GetLength(), buffers.GetData());
	}

	/*
		Sampler setters
	*/
	using SetSampFunc = void (ID3D11DeviceContext::*)(UINT, UINT, ID3D11SamplerState* const*);
	static constexpr std::array<SetSampFunc, g_ShadeStageCount> s_SamplerSetters =
	{
		&ID3D11DeviceContext::VSSetSamplers,
		&ID3D11DeviceContext::HSSetSamplers,
		&ID3D11DeviceContext::DSSetSamplers,
		&ID3D11DeviceContext::GSSetSamplers,
		&ID3D11DeviceContext::PSSetSamplers,
		&ID3D11DeviceContext::CSSetSamplers
	};

	void SetSamplers(ID3D11DeviceContext* pCtx, ShadeStages stage, uint startSlot, const Span<ID3D11SamplerState*> res)
	{
		D3D_ASSERT_MSG(pCtx != nullptr, "Attempted to dereference a null device context");
		D3D_ASSERT_MSG((startSlot + res.GetLength()) <= D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, 
			"Sampler slot limit exceeded: {}", (startSlot + res.GetLength()));

		(pCtx->*s_SamplerSetters[(int)stage])(startSlot, (uint)res.GetLength(), res.GetData());
	}

	/*
		SRV setters
	*/
	using SetSRVFunc = void (ID3D11DeviceContext::*)(UINT, UINT, ID3D11ShaderResourceView* const*);
	static constexpr std::array<SetSRVFunc, g_ShadeStageCount> s_ResViewSetters =
	{
		&ID3D11DeviceContext::VSSetShaderResources,
		&ID3D11DeviceContext::HSSetShaderResources,
		&ID3D11DeviceContext::DSSetShaderResources,
		&ID3D11DeviceContext::GSSetShaderResources,
		&ID3D11DeviceContext::PSSetShaderResources,
		&ID3D11DeviceContext::CSSetShaderResources
	};

	void SetShaderResources(ID3D11DeviceContext* pCtx, ShadeStages stage, uint startSlot, const Span<ID3D11ShaderResourceView*> srvs)
	{
		D3D_ASSERT_MSG(pCtx != nullptr, "Attempted to dereference a null device context");
		D3D_ASSERT_MSG((startSlot + srvs.GetLength()) <= D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT,
			"Resource view count exceeded: {}", (startSlot + srvs.GetLength()));

		(pCtx->*s_ResViewSetters[(int)stage])(startSlot, (uint)srvs.GetLength(), srvs.GetData());
	}

	void CSSetUnorderedAccessViews(ID3D11DeviceContext* pCtx, uint startSlot, const Span<ID3D11UnorderedAccessView*> uavs, const uint* pInitialCounts)
	{
		D3D_ASSERT_MSG(pCtx != nullptr, "Attempted to dereference a null device context");
		D3D_ASSERT_MSG((startSlot + uavs.GetLength()) <= D3D11_1_UAV_SLOT_COUNT,
			"Resource view count exceeded: {}", (startSlot + uavs.GetLength()));
		
		pCtx->CSSetUnorderedAccessViews(startSlot, (uint)uavs.GetLength(), uavs.GetData(), pInitialCounts);
	}

	/*
		Shading stage setters
	*/
	template<typename T>
	static T* GetShaderSubtype(ID3D11DeviceChild* pShader)
	{
#ifdef NDEBUG
		return static_cast<T*>(pShader);
#else
		return static_cast<T*>(pShader);
#endif // !NDEBUG
	}

	static void VSSetShader(ID3D11DeviceContext* pCtx, ID3D11DeviceChild* pShader, ID3D11ClassInstance* const* ppClassInstances, UINT numClassInstances)
	{
		pCtx->VSSetShader(GetShaderSubtype<ID3D11VertexShader>(pShader), ppClassInstances, numClassInstances);
	}

	static void HSSetShader(ID3D11DeviceContext* pCtx, ID3D11DeviceChild* pShader, ID3D11ClassInstance* const* ppClassInstances, UINT numClassInstances)
	{
		pCtx->HSSetShader(GetShaderSubtype<ID3D11HullShader>(pShader), ppClassInstances, numClassInstances);
	}

	static void DSSetShader(ID3D11DeviceContext* pCtx, ID3D11DeviceChild* pShader, ID3D11ClassInstance* const* ppClassInstances, UINT numClassInstances)
	{
		pCtx->DSSetShader(GetShaderSubtype<ID3D11DomainShader>(pShader), ppClassInstances, numClassInstances);
	}

	static void GSSetShader(ID3D11DeviceContext* pCtx, ID3D11DeviceChild* pShader, ID3D11ClassInstance* const* ppClassInstances, UINT numClassInstances)
	{
		pCtx->GSSetShader(GetShaderSubtype<ID3D11GeometryShader>(pShader), ppClassInstances, numClassInstances);
	}

	static void PSSetShader(ID3D11DeviceContext* pCtx, ID3D11DeviceChild* pShader, ID3D11ClassInstance* const* ppClassInstances, UINT numClassInstances)
	{
		pCtx->PSSetShader(GetShaderSubtype<ID3D11PixelShader>(pShader), ppClassInstances, numClassInstances);
	}

	static void CSSetShader(ID3D11DeviceContext* pCtx, ID3D11DeviceChild* pShader, ID3D11ClassInstance* const* ppClassInstances, UINT numClassInstances)
	{
		pCtx->CSSetShader(GetShaderSubtype<ID3D11ComputeShader>(pShader), ppClassInstances, numClassInstances);
	}

	typedef void (*SetShaderFunc)(ID3D11DeviceContext*, ID3D11DeviceChild*, ID3D11ClassInstance* const*, UINT);
	static constexpr std::array<SetShaderFunc, g_ShadeStageCount> s_ShaderSetters =
	{
		&VSSetShader,
		&HSSetShader,
		&DSSetShader,
		&GSSetShader,
		&PSSetShader,
		&CSSetShader
	};

	void SetShader(ID3D11DeviceContext* pCtx, ShadeStages stage, ID3D11DeviceChild* pShader, ID3D11ClassInstance* const* ppClassInstances, UINT numClassInstances)
	{
		D3D_ASSERT_MSG(pCtx != nullptr, "Attempted to dereference a null device context");
		s_ShaderSetters[(int)stage](pCtx, pShader, ppClassInstances, numClassInstances);
	}

	void OMSetRenderTargets(ID3D11DeviceContext* pCtx, const Span<ID3D11RenderTargetView*> rtvs, ID3D11DepthStencilView* pDSV)
	{
		D3D_ASSERT_MSG(pCtx != nullptr, "Attempted to dereference a null device context");
		D3D_ASSERT_MSG((rtvs.GetLength()) <= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT,
			"Render target limit exceeded: {}", (rtvs.GetLength()));

		pCtx->OMSetRenderTargets((uint)rtvs.GetLength(), rtvs.GetData(), pDSV);
	}
}