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
		MAKE_NO_COPY(ComputeInstance)

		ComputeInstance();

		ComputeInstance(ShaderVariantManager& lib, uint nameID, uint vID);

		ComputeInstance(ComputeInstance&&) noexcept;

		ComputeInstance& operator=(ComputeInstance&&) noexcept;

		void Dispatch(ContextBase& ctx, ivec3 groups);

		void Dispatch(ContextBase& ctx, ivec2 groups);

		void Dispatch(ContextBase& ctx, int groups);

	protected:
		mutable const ComputeShaderVariant* pCS;

		const ComputeShaderVariant& GetShader() const;

		void SetVariantID(uint vID) override;
	};
}
