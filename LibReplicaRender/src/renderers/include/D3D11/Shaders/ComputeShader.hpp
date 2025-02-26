#pragma once
#include "ShaderBase.hpp"

namespace Replica::D3D11
{
	class ComputeShader : public ShaderInstance<ID3D11ComputeShader>
	{
	public:
		using ShaderInstance::SetSampler;
		using ShaderInstance::SetTexture;

		ComputeShader();

		ComputeShader(const ComputeShaderVariant& cs);

		/// <summary>
		/// Sets RWTexture2D using last context
		/// </summary>
		virtual void SetRWTexture(string_view name, IRWTexture2D& tex);

		/// <summary>
		/// Dispatch the shader in the given context with the given number of
		/// groups. Automatically binds the shader as needed.
		/// </summary>
		void Dispatch(Context& ctx, int groups);

		/// <summary>
		/// Dispatch the shader in the given context with the given number of
		/// groups. Automatically binds the shader as needed.
		/// </summary>
		void Dispatch(Context& ctx, ivec2 groups);

		/// <summary>
		/// Dispatch the shader in the given context with the given number of
		/// groups. Automatically binds the shader as needed.
		/// </summary>
		void Dispatch(Context& ctx, ivec3 groups);

		/// <summary>
		/// Binds the compute shader using the given context
		/// </summary>
		void Bind(Context& ctx) override;

		/// <summary>
		/// Unbinds compute shader
		/// </summary>
		void Unbind(Context& ctx) override;

		void UnmapResources(Context& ctx) override;

	private:
		ResourceMap<ID3D11UnorderedAccessView> uavBuffers;
	};
}