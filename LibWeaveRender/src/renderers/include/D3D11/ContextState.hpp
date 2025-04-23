#pragma once
#include <unordered_map>
#include <array>
#include "WeaveEffects/ShaderData.hpp"
#include "Resources/ResourceBase.hpp"
#include "D3D11/Resources/ResourceMap.hpp"
#include "D3D11/Resources/ResourceSet.hpp"
#include "D3D11/Viewport.hpp"

namespace Weave::D3D11
{
	using Effects::ShadeStages;
	using Effects::ShaderTypes;

	enum class PrimTopology;
	class InputLayout;
	class VertexBuffer;

	class ConstantBuffer;
	class ResourceSet;
	class Sampler;
	class ShaderVariantBase;

	/// <summary>
	/// Mirrored cache for D3D11 device context state
	/// </summary>
	class ContextState
	{
	public:
		MAKE_NO_COPY(ContextState);

		/// <summary>
		/// Resource type usages tracked for internal conflict detection.
		/// </summary>
		enum class RWResourceUsages : byte
		{
			Unused = 0,
			Read = 1u << 1u,
			Write = 1u << 2u,

			ShaderResourceView = 1u << 3u | Read,
			UnorderedAccessView = 1u << 4u | Write,
			RenderTargetView = 1u << 5u | Write,
			DepthStencilView = 1u << 6u | Write
		};

		static string_view GetUsageName(RWResourceUsages usage);

		/// <summary>
		/// Tracks the state of an individual shading stage's resources
		/// </summary>
		struct StageState
		{
			ShadeStages stage;
			const ShaderVariantBase* pShader;
			ulong drawCount;

			UniqueArray<Sampler*> samplers;
			uint sampCount;

			UniqueArray<ID3D11Buffer*> cbuffers;
			uint cbufCount;

			UniqueArray<IShaderResource*> srvs;
			uint srvCount;

			StageState();

			void Reset();

			const Span<Sampler*> GetSamplers(sint offset = 0, uint extent = (uint)-1) const;

			const Span<ID3D11Buffer*> GetCBuffers(sint offset = 0, uint extent = (uint)-1) const;

			const Span<IShaderResource*> GetSRVs(sint offset = 0, uint extent = (uint)-1) const;
		};

		/// <summary>
		/// A set of counters used to report cache ranges affected by internal conflicts
		/// </summary>
		struct ConflictState
		{
			/// <summary>
			/// Per-stage array indicating the extent of the ranges of SRVs where conflicts
			/// were detected. Zero if no conflicts were detected.
			/// </summary>
			std::array<byte, g_ShadeStageCount> srvsExtent;

			/// <summary>
			/// Indicates the maximum extent of the UAV conflicts found in the cache.
			/// </summary>
			uint uavsExtent;

			/// <summary>
			/// Indicates the maximum extent of the RTV or DSV conflicts found in the cache.
			/// A DSV conflict will force all RTVs to be rebound.
			/// </summary>
			uint rtvExtent;

			/// <summary>
			/// Returns true if no conflicts were detected.
			/// </summary>
			bool IsEmpty() const;
		};

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

		ulong GetDrawCount() const;

		void IncrementDraw();

		/// <summary>
		/// Returns state information for the given stage
		/// </summary>
		const StageState& GetStage(ShadeStages stage) const;

		const Span<IRenderTarget*> GetRenderTargets() const;

		IDepthStencil* GetDepthStencil() const;

		const Span<Viewport> GetViewports();

		ID3D11Buffer* GetIndexBuffer() const;

		ID3D11InputLayout* GetInputLayout() const;

		const Span<ID3D11Buffer*> GetVertexBuffers() const;

		const Span<uint> GetVertStrides() const;

		const Span<uint> GetVertOffsets() const;

		const Span<IUnorderedAccess*> CSGetUAVs(sint offset = 0, uint extent = (uint)-1) const;

		/// <summary>
		/// Returns interface to depth stencil resource view
		/// </summary>
		ID3D11DepthStencilView* GetDepthStencilView() const;

		/// <summary>
		/// Returns configuration interface for defining how depth-stencil operations performed
		/// </summary>
		ID3D11DepthStencilState* GetDepthStencilState() const;

		/// <summary>
		/// Returns the depth range, e.g. [0, 1] used for depth testing
		/// </summary>
		vec2 GetDepthStencilRange() const;

		/// <summary>
		/// Updates state cache with the given primitive topology. Returns true if a change occured.
		/// </summary>
		bool TryUpdateTopology(PrimTopology topology);

		/// <summary>
		/// Assigns the given vertex buffers to the state cache. Returns the extent of the range changed.
		/// </summary>
		uint TryUpdateVertexBuffers(IDynamicArray<VertexBuffer>& vertBuffers, sint startSlot);

		/// <summary>
		/// Clears the given range of vertex buffers. Returns the extent of the range changed.
		/// </summary>
		uint TryResetVertexBuffers(sint startSlot = 0, uint count = (uint)-1);

		/// <summary>
		/// Updates the state cache with the given index buffer. Returns true if a change occured.
		/// </summary>
		bool TryUpdateIndexBuffer(ID3D11Buffer* pIndexBuffer);

		/// <summary>
		/// Updates the vertex input layout in the state cache. Returns true if a change occured.
		/// </summary>
		bool TryUpdateInputLayout(ID3D11InputLayout* pLayout);

		/// <summary>
		/// Updates the shader for the given stage in the cache and returns true if a change was made.
		/// </summary>
		bool TrySetShader(ShadeStages stage, const ShaderVariantBase* pShader);

		/// <summary>
		/// Updates the cached depth stencil and returns true if any changes were made that need to
		/// be applied by the context.
		/// </summary>
		bool TryUpdateDepthStencil(IDepthStencil* pDepthStencil);

		/// <summary>
		/// Updates RTV state cache and returns true if any changes were made that need to
		/// be applied by the context.
		/// </summary>
		bool TryUpdateRenderTargets(IDynamicArray<IRenderTarget*>& newRTVs, uint startSlot);

		/// <summary>
		/// Automatically updates viewports to align with DRS area specified by Weave RT objects and 
		/// returns true if changes were made
		/// </summary>
		bool TryUpdateViewports();

		/// <summary>
		/// Updates resources bound in the state cache and returns the resource range affected that 
		/// requires update.
		/// </summary>
		const Span<ID3D11Buffer*> TryUpdateResources(ShadeStages stage, IDynamicArray<ConstantBuffer>& cbufs);

		/// <summary>
		/// Updates resources bound in the state cache and returns the resource range affected that 
		/// requires update.
		/// </summary>
		const Span<Sampler*> TryUpdateResources(ShadeStages stage, const ResourceSet::SamplerList& resSrc, const SamplerMap* pMap);

		/// <summary>
		/// Updates resources bound in the state cache and returns the resource range affected that 
		/// requires update.
		/// </summary>
		const Span<IShaderResource*> TryUpdateResources(ShadeStages stage, const ResourceSet::SRVList& resSrc, const ResourceViewMap* pMap);

		/// <summary>
		/// Updates resources bound in the state cache and returns the resource range affected that 
		/// requires update.
		/// </summary>
		const Span<IUnorderedAccess*> TryUpdateResources(const ResourceSet::UAVList& resSrc, const UnorderedAccessMap* pMap);

		/// <summary>
		/// Attempts to resolve any conflicts in cache by setting them to null. Returns a state object with counters
		/// indicating the maximum extent of values that will need to be updated in the context.
		/// </summary>
		ConflictState TryResolveConflicts();

	private:
		/// <summary>
		/// An array of per-stage usage flags for a given resource
		/// </summary>
		using UsageList = std::array<RWResourceUsages, g_ShadeStageCount>;

		/// <summary>
		/// An array of per-stage bind slots corresponding to a given resource
		/// </summary>
		using SlotList = std::array<byte, g_ShadeStageCount>;

		/// <summary>
		/// Describes the location of a resource identified as causing a read-write conflict 
		/// within the state cache.
		/// </summary>
		struct RWConflictDesc
		{
			ShadeStages nextStage;
			RWResourceUsages nextUsage;
			ShadeStages lastStage;
			RWResourceUsages lastUsage;
			uint slot;
		};

		/// <summary>
		/// Tracks the usage of an individual resource in the pipeline
		/// </summary>
		class UsageDesc
		{
		public:
			MAKE_DEF_MOVE_COPY(UsageDesc);			

			UsageDesc();

			UsageDesc(ShadeStages stage, RWResourceUsages usage, uint slot);

			/// <summary>
			/// Returns the slot the resource is bound to for the given stage and usage type.
			/// Undefined if not bound to the given stage.
			/// </summary>
			uint GetSlot(ShadeStages stage) const;

			/// <summary>
			/// Returns the usage type for the resource bound to the given stage. Set to Unused if
			/// not bound to the given stage.
			/// </summary>
			RWResourceUsages GetUsage(ShadeStages stage) const;

			/// <summary>
			/// Adds the given resource usage to the description, removes any previous usages that
			/// would cause RW conflicts, and adds them to the conflict vector.
			/// </summary>
			uint SetUsage(ShadeStages stage, RWResourceUsages usage, uint slot, Vector<RWConflictDesc>& conflictBuffer);

			/// <summary>
			/// Clears usage flags for the given stage
			/// </summary>
			void ResetUsage(ShadeStages stage);

			/// <summary>
			/// Returns true if zero stages are using this resource
			/// </summary>
			bool IsEmpty() const;

		private:
			UsageList uses;
			SlotList slots;
		};

		ulong drawCount;
		UniqueArray<StageState> stages;

		PrimTopology topology;
		ID3D11InputLayout* pInputLayout;
		ID3D11Buffer* pIndexBuffer;

		UniqueArray<ID3D11Buffer*> vertexBuffers;
		uint vertBufCount;
		UniqueArray<uint> vbStrides;
		UniqueArray<uint> vbOffsets;

		IDepthStencil* pDepthStencil;
		ID3D11BlendState* pBlendState;
		ID3D11RasterizerState* pRasterState;

		UniqueArray<IUnorderedAccess*> uavs;
		uint uavCount;

		UniqueArray<IRenderTarget*> rtvs;
		uint rtCount;

		UniqueArray<Viewport> viewports;
		uint vpCount;

		std::unordered_map<const IResource*, UsageDesc> resUsageMap;
		UniqueVector<RWConflictDesc> conflictBuffer;
		bool isInitialized;

		template<RWResourceUsages UsageT, typename ResT>
		void UpdateUsageMap(ShadeStages stage, const Span<ResT*> stateRes, const IDynamicArray<ResT*>& newRes);

		void LogInvalidStateTransitions(RWConflictDesc conflict);
	};
}