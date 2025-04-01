#pragma once
#include "ShaderInstanceBase.hpp"

namespace Replica::D3D11
{
	class ShaderVariantManager;
	class ResourceSet;
	class ComputeShaderVariant;

	class ComputeInstance : public ShaderInstanceBase
	{
	public:
		MAKE_NO_COPY(ComputeInstance)

		ComputeInstance();

		ComputeInstance(ShaderVariantManager& lib, uint nameID, int vID);

		ComputeInstance(ComputeInstance&&) noexcept;

		ComputeInstance& operator=(ComputeInstance&&) noexcept;

		void Dispatch(Context& ctx, ivec3 groups);

		void Dispatch(Context& ctx, ivec2 groups);

		void Dispatch(Context& ctx, int groups);

	protected:
		mutable const ComputeShaderVariant* pCS;

		const ComputeShaderVariant& GetShader() const;

		void SetVariantID(int vID) override;
	};
}
