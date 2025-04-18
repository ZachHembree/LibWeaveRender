#pragma once
#include "WeaveEffects/ShaderData.hpp"
#include "Resources/ResourceBase.hpp"

namespace Weave::D3D11
{
	using Effects::ShadeStages;

	struct Viewport;
	enum class PrimTopology;
	class InputLayout;

	class IDepthStencil;
	class VertexBuffer;
	class ResourceSet;
	class ShaderVariantBase;

	class ContextState
	{
	public:
		MAKE_NO_COPY(ContextState);

		using SamplerList = IDynamicArray<ID3D11SamplerState*>;
		using CBufList = IDynamicArray<ID3D11Buffer*>;
		using SRVList = IDynamicArray<ID3D11ShaderResourceView*>;
		using UAVList = IDynamicArray<ID3D11UnorderedAccessView*>;

		using RenderTargetList = IDynamicArray<ID3D11RenderTargetView*>;
		using ViewportList = IDynamicArray<Viewport>;
		using VertexBufList = IDynamicArray<VertexBuffer*>;

		struct StageState
		{
			const ShaderVariantBase* pShader;
			UniqueArray<ID3D11SamplerState*> samplers;
			UniqueArray<ID3D11Buffer*> cbuffers;
			UniqueArray<ID3D11ShaderResourceView*> resViews;

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
		UniqueArray<ID3D11UnorderedAccessView*> uavs;

		uint rtCount;
		uint vpCount;
		uint vertBufCount;
		uint uavCount;

		UniqueArray<StageState> stages;

		ContextState();

		ContextState(ContextState&& other) noexcept;

		ContextState& operator=(ContextState&& rhs) noexcept;

		~ContextState();

		void Init();

		void Reset();

		bool GetIsValid() const;

		StageState& GetStage(ShadeStages stage);

		ID3D11DepthStencilView* GetDepthStencilView() const;

		ID3D11DepthStencilState* GetDepthStencilState() const;

		vec2 GetDepthStencilRange() const;

	private:
		bool isInitialized;
	};
}