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
		/// Remaps the instance to the shader kernel with the given name within the same ShaderLibrary.
		/// Retains resource and flag configurations.
		/// </summary>
		void SetKernel(uint nameID);

		/// <summary>
		/// Remaps the instance to the shader kernel with the given name within the same ShaderLibrary.
		/// Retains resource and flag configurations.
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

		void Dispatch(CtxBase& ctx, ivec3 groups);

		void Dispatch(CtxBase& ctx, ivec2 groups);

		void Dispatch(CtxBase& ctx, int groups);

	protected:
		mutable const ComputeShaderVariant* pCS;

		const ComputeShaderVariant& GetShader() const;

		void SetVariantID(uint vID) override;
	};
}
