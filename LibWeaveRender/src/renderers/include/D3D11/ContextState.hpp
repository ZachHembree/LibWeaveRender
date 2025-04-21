#pragma once
#include <unordered_map>
#include "WeaveEffects/ShaderData.hpp"
#include "Resources/ResourceBase.hpp"
#include "D3D11/Resources/ResourceMap.hpp"
#include "D3D11/Resources/ResourceSet.hpp"
#include "D3D11/Viewport.hpp"

namespace Weave::D3D11
{
	using Effects::ShadeStages;
	using Effects::ShaderTypes;

	class IDepthStencil;

	enum class PrimTopology;
	class InputLayout;
	class VertexBuffer;

	class ConstantBuffer;
	class ResourceSet;
	class Sampler;
	class ShaderVariantBase;

	/// <summary>
	/// Stores mirrored state of a D3D11 context
	/// </summary>
	class ContextState
	{
	public:
		MAKE_NO_COPY(ContextState);

		using SamplerList = IDynamicArray<Sampler*>;
		using CBufList = IDynamicArray<ID3D11Buffer*>;
		using SRVList = IDynamicArray<IShaderResource*>;
		using UAVList = IDynamicArray<IUnorderedAccess*>;

		using RenderTargetList = IDynamicArray<ID3D11RenderTargetView*>;
		using ViewportList = IDynamicArray<Viewport>;
		using VertexBufList = IDynamicArray<VertexBuffer*>;

		struct StageState
		{
			ShadeStages stage;
			const ShaderVariantBase* pShader;
			UniqueArray<Sampler*> samplers;
			UniqueArray<ID3D11Buffer*> cbuffers;
			UniqueArray<IShaderResource*> srvs;

			uint sampCount;
			uint cbufCount;
			uint srvCount;

			StageState();

			void Reset();
		};

		PrimTopology topology;
		ID3D11InputLayout* pInputLayout;
		ID3D11Buffer* pIndexBuffer;
		IDepthStencil* pDepthStencil;
		ID3D11BlendState* pBlendState;
		ID3D11RasterizerState* pRasterState;

		UniqueArray<ID3D11RenderTargetView*> renderTargets;
		UniqueArray<Viewport> viewports;
		UniqueArray<ID3D11Buffer*> vertexBuffers;
		UniqueArray<uint> vbStrides;
		UniqueArray<uint> vbOffsets;
		UniqueArray<IUnorderedAccess*> uavs;

		uint rtCount;
		uint vpCount;
		uint vertBufCount;
		uint uavCount;

		UniqueArray<StageState> stages;

		ContextState();

		ContextState(ContextState&& other) noexcept;

		ContextState& operator=(ContextState&& rhs) noexcept;

		~ContextState();

		/// <summary>
		/// Initializes internal buffers
		/// </summary>
		void Init();

		/// <summary>
		/// Clears internal buffers
		/// </summary>
		void Reset();

		/// <summary>
		/// Returns true if the state has been initialized
		/// </summary>
		bool GetIsValid() const;

		/// <summary>
		/// Returns state information for the given stage
		/// </summary>
		StageState& GetStage(ShadeStages stage);

		ID3D11DepthStencilView* GetDepthStencilView() const;

		ID3D11DepthStencilState* GetDepthStencilState() const;

		vec2 GetDepthStencilRange() const;

		/// <summary>
		/// Updates resources bound in the state cache and returns the size of the bound resource range
		/// affected that requires update.
		/// </summary>
		uint TryUpdateResources(ShadeStages stage, IDynamicArray<ConstantBuffer>& cbufs);

		/// <summary>
		/// Updates resources bound in the state cache and returns the size of the bound resource range
		/// affected that requires update.
		/// </summary>
		uint TryUpdateResources(ShadeStages stage, const ResourceSet::SamplerList& resSrc, const SamplerMap* pMap);

		/// <summary>
		/// Updates resources bound in the state cache and returns the size of the bound resource range
		/// affected that requires update.
		/// </summary>
		uint TryUpdateResources(ShadeStages stage, const ResourceSet::SRVList& resSrc, const ResourceViewMap* pMap);

		/// <summary>
		/// Updates resources bound in the state cache and returns the size of the bound resource range
		/// affected that requires update.
		/// </summary>
		uint TryUpdateResources(const ResourceSet::UAVList& resSrc, const UnorderedAccessMap* pMap);

	private:
		bool isInitialized;

		/// <summary>
		/// Updates resources bound in the state cache and returns the size of the bound resource range
		/// affected that requires update.
		/// </summary>
		template<typename ResT>
		uint TryUpdateResources(
			IDynamicArray<ResT*>& stateRes,
			uint& stateCount,
			IDynamicArray<ResT*>& newRes
		);

		/// <summary>
		/// Updates resources bound in the state cache and returns the size of the bound resource range
		/// affected that requires update.
		/// </summary>
		template<typename ResT, typename MapT, typename DataT>
		uint TryUpdateResources(
			IDynamicArray<ResT*>& stateRes,
			uint& stateCount,
			const DataT& resSrc,
			const MapT* pResMap
		);
	};
}