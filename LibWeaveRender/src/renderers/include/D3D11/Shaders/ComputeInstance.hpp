#pragma once
#include "ShaderInstanceBase.hpp"

namespace Weave::D3D11
{
	class ShaderVariantManager;
	class ResourceSet;
	class ComputeShaderVariant;

	class ComputeInstance : public ShaderInstanceBase
	{
	public:
		DECL_DEST_MOVE(ComputeInstance)

		ComputeInstance();

		ComputeInstance(ShaderVariantManager& lib, uint nameID, uint vID);

		/// <summary>
		/// Returns the shader's thread group dimensions
		/// </summary>
		uivec3 GetThreadGroupSize() const;

		/// <summary>
		/// Remaps the instance to the shader kernel with the given name within the same effect library. 
		/// Retains resources and resets defines.
		/// </summary>
		void SetKernel(uint nameID);

		/// <summary>
		/// Remaps the instance to the shader kernel with the given name within the same effect library. 
		/// Retains resources and resets defines.
		/// </summary>
		void SetKernel(string_view name);

		/// <summary>
		/// Returns a read/writable handle for the UAV slot with the given name
		/// </summary>
		ComputeBufferHandle GetComputeBuffer(uint nameID);

		/// <summary>
		/// Returns a read/writable handle for the UAV slot with the given name
		/// </summary>
		ComputeBufferHandle GetComputeBuffer(string_view name);

		void SetComputeBuffer(uint nameID, const IShaderResource& buf);

		void SetComputeBuffer(string_view name, const IShaderResource& buf);

		RWComputeBufferHandle GetRWComputeBuffer(uint nameID);

		RWComputeBufferHandle GetRWComputeBuffer(string_view name);

		void SetRWComputeBuffer(uint nameID, IUnorderedAccess& buf);

		void SetRWComputeBuffer(string_view name, IUnorderedAccess& buf);

		/// <summary>
		/// Dispatches the given number of thread groups. Total threads dispatched is equal to groups * GetThreadGroupSize().
		/// </summary>
		void Dispatch(CtxBase& ctx, uivec3 groups);

		/// <summary>
		/// Dispatches the given number of thread groups. Total threads dispatched is equal to groups * GetThreadGroupSize().
		/// </summary>
		void Dispatch(CtxBase& ctx, uivec2 groups);

		/// <summary>
		/// Dispatches the given number of thread groups. Total threads dispatched is equal to groups * GetThreadGroupSize().
		/// </summary>
		void Dispatch(CtxBase& ctx, uint groupX, uint groupY = 1, uint groupZ = 1);

		/// <summary>
		/// Dispatches the given number of threads by normalizing to group size with glm::ceil.
		/// groups ~= ceil(threads / GetThreadGroupSize())
		/// </summary>
		void DispatchThreads(CtxBase& ctx, uivec3 threads);

		/// <summary>
		/// Dispatches the given number of threads by normalizing to group size with glm::ceil.
		/// groups ~= ceil(threads / GetThreadGroupSize())
		/// </summary>
		void DispatchThreads(CtxBase& ctx, uivec2 threads);

		/// <summary>
		/// Dispatches the given number of threads by normalizing to group size with glm::ceil.
		/// groups ~= ceil(threads / GetThreadGroupSize())
		/// </summary>
		void DispatchThreads(CtxBase& ctx, uint threadsX, uint threadsY = 1, uint threadsZ = 1);

	protected:
		mutable const ComputeShaderVariant* pCS;

		const ComputeShaderVariant& GetShader() const;

		void SetVariantID(uint vID) override;
	};
}
