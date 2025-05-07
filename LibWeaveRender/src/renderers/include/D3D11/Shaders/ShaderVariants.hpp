#pragma once
#include "ShaderVariantBase.hpp"
#include "../Resources/InputLayout.hpp"
#include "../CtxImm.hpp"
#include "../Device.hpp"

namespace Weave::D3D11
{
	/// <summary>
	/// Abstract base template for shading stage subtypes used internally, represents an individual compiled variant and
	/// relevant resource mappings.
	/// </summary>
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

		virtual ~ShaderVariant() = 0;

		/// <summary>
		/// Returns a pointer to the underlying compiled shader object
		/// </summary>
		ID3D11DeviceChild* Get() const override { return pShader.Get(); }

		/// <summary>
		/// Returns a pointer to the underlying compiled shader object subtype
		/// </summary>
		ShaderT* GetSubtype() const { return pShader.Get(); }

		/// <summary>
		/// Returns an enum indicating the shader's subtype.
		/// </summary>
		ShadeStages GetStage() const override { return Stage; }

	protected:
		UniqueComPtr<ShaderT> pShader;
	};

	template<typename ShaderT, ShadeStages StageT>
	inline ShaderVariant<ShaderT, StageT>::~ShaderVariant() {}

	/// <summary>
	/// Contains a variant of an individual compiled vertex shader and its resource mappings
	/// </summary>
	class VertexShaderVariant : public ShaderVariant<ID3D11VertexShader, ShadeStages::Vertex>
	{
	public:
		DECL_DEST_MOVE(VertexShaderVariant)
		using ShaderVariant::ShaderVariant;

		VertexShaderVariant(Device& dev, const ShaderDefHandle& def);

		const InputLayout& GetInputLayout() const { return layout; }

	private:
		InputLayout layout;
	};

	/// <summary>
	/// Contains a variant of an individual compiled pixel shader and its resource mappings
	/// </summary>
	class PixelShaderVariant : public ShaderVariant<ID3D11PixelShader, ShadeStages::Pixel>
	{
	public:
		DECL_DEST_MOVE(PixelShaderVariant)
		using ShaderVariant::ShaderVariant;

	private:
	};

	/// <summary>
	/// Contains a variant of an individual compiled compute shader and its resource mappings
	/// </summary>
	class ComputeShaderVariant : public ShaderVariant<ID3D11ComputeShader, ShadeStages::Compute>
	{
	public:
		DECL_DEST_MOVE(ComputeShaderVariant)
		using ShaderVariant::ShaderVariant;
	
		ComputeShaderVariant(Device& dev, const ShaderDefHandle& def);

		/// <summary>
		/// Returns the layout map for unordered access views. Returns nullptr if undefined or empty.
		/// </summary>
		const UnorderedAccessMap* GetUAVMap() const override;

	private:
		std::unique_ptr<UnorderedAccessMap> pUAVmap;
	};
}
