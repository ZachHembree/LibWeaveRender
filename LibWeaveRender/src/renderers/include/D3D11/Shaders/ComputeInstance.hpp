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

		void SetComputeBuffer(uint nameID, IShaderResource& buf);

		void SetComputeBuffer(string_view name, IShaderResource& buf);

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
