#pragma once
#include "D3D11/IResource.hpp"
#include "D3D11/dev/VertexShader.hpp"
#include "D3D11/dev/PixelShader.hpp"

namespace Replica::D3D11
{
	struct EffectDef
	{
		VertexShaderDef vsDef;
		PixelShaderDef psDef;
	};

	class Effect : public IResource
	{
	public:
		Effect(
			Device& dev, 
			const EffectDef& effectDef
		) :
			vs(dev, effectDef.vsDef),
			ps(dev, effectDef.psDef)
		{ }

		/// <summary>
		/// Sets the value corresponding to the given name to the
		/// given value.
		/// </summary>
		template<typename T>
		void SetConstant(wstring_view name, const T& value)
		{
			vs.SetConstant(name, value);
			ps.SetConstant(name, value);
		}

		/// <summary>
		/// Sets sampler using last context
		/// </summary>
		void SetSampler(wstring_view name, Sampler& samp)
		{
			vs.SetSampler(name, samp);
			ps.SetSampler(name, samp);
		}

		/// <summary>
		/// Sets Texture2D using last context
		/// </summary>
		void SetTexture(wstring_view name, Texture2D& tex)
		{
			vs.SetTexture(name, tex);
			ps.SetTexture(name, tex);
		}

		void Setup(Context& ctx) override
		{
			vs.Bind();
			ps.Bind();
		}

	private:
		VertexShader vs;
		PixelShader ps;
	};
}
