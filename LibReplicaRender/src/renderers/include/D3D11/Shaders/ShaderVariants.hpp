#pragma once
#include "ShaderVariantBase.hpp"
#include "D3D11/Resources/InputLayout.hpp"
#include "D3D11/Context.hpp"

namespace Replica::D3D11
{
	template<typename ShaderT, ShadeStages StageT>
	class ShaderVariant : public ShaderVariantBase
	{
	public:
		MAKE_MOVE_ONLY(ShaderVariant)

		typedef ShaderT ShaderPtr;
		static const ShadeStages Stage = StageT;

		ShaderVariant() : ShaderVariantBase()
		{ }

		ShaderVariant(Device& dev, const ShaderDefHandle& def) :
			ShaderVariantBase(dev, def)
		{
			dev.CreateShader(def.GetBinSrc(), pShader);
		}

		ShaderT* Get() const { return pShader.Get(); }		

	protected:
		ComPtr<ShaderT> pShader;
	};

	class VertexShaderVariant : public ShaderVariant<ID3D11VertexShader, ShadeStages::Vertex>
	{
	public:
		MAKE_MOVE_ONLY(VertexShaderVariant)

		using ShaderVariant::ShaderVariant;

		VertexShaderVariant(Device& dev, const ShaderDefHandle& def);

		void Bind(Context& ctx, const ResourceSet& res) const override;

		void Unbind(Context& ctx) const override;

		void MapResources(Context& ctx, const ResourceSet& res) const override;

	private:
		InputLayout layout;
	};

	class PixelShaderVariant : public ShaderVariant<ID3D11PixelShader, ShadeStages::Pixel>
	{
	public:
		MAKE_MOVE_ONLY(PixelShaderVariant)

		using ShaderVariant::ShaderVariant;

		void Bind(Context& ctx, const ResourceSet& res) const override;

		void Unbind(Context& ctx) const override;

	private:
	};

	class ComputeShaderVariant : public ShaderVariant<ID3D11ComputeShader, ShadeStages::Compute>
	{
	public:
		MAKE_MOVE_ONLY(ComputeShaderVariant)

		using ShaderVariant::ShaderVariant;
	
		ComputeShaderVariant(Device& dev, const ShaderDefHandle& def);

		void Bind(Context& ctx, const ResourceSet& res) const override;

		void Unbind(Context& ctx) const override;

		void MapResources(Context& ctx, const ResourceSet& res) const override;

		void UnmapResources(Context& ctx) const override;

	private:
		UnorderedAccessMap uavMap;
	};
}
