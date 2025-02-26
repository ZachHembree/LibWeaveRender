#pragma once
#include "ShaderLibGen/ShaderLibMap.hpp"
#include "../D3D11/EffectVariant.hpp"
#include "../D3D11/Shaders/ShaderVariantBase.hpp"

namespace Replica::D3D11
{
	using Effects::ShaderLibDef;
	using Effects::ShaderLibMap;
	using Effects::ShadeStages;
	using Effects::EffectDef;

	class Device;

	class ShaderLibrary
	{
	public:
		MAKE_NO_COPY(ShaderLibrary)

		ShaderLibrary();

		ShaderLibrary(Device& device, const ShaderLibDef& def);

		ShaderLibrary(Device& device, ShaderLibDef&& def);

		bool TryGetShader(const int shaderID, const VertexShaderVariant*& pVS, const int vID = 0);

		bool TryGetShader(const int shaderID, const PixelShaderVariant*& pPS, const int vID = 0);

		bool TryGetShader(const int shaderID, const ComputeShaderVariant*& pCS, const int vID = 0);

		EffectVariant& GetEffect(const int effectID, const int vID = 0);

		EffectVariant& GetEffect(string_view name, const int vID = 0);

		/// <summary>
		/// Retrieves interface for querying shader definitions
		/// </summary>
		const ShaderLibMap& GetLibMap() const;

		template<typename ShaderT> const ShaderT& GetShader(const int shaderID, const int vID = 0)
		{
			const ShaderT* pShader = nullptr;

			if (TryGetShader(shaderID, pShader, vID))
				return *pShader;
			else
				GFX_THROW("Invalid shader specified");
		}

		template<typename ShaderT> const ShaderT& GetShader(string_view name, const int vID = 0)
		{
			return GetShader<ShaderT>(libMap.TryGetShaderID(name, vID), vID);
		}

	private:
		struct Variant
		{		
			std::unordered_map<int, VertexShaderVariant> vertexShaders;
			std::unordered_map<int, PixelShaderVariant> pixelShaders;
			std::unordered_map<int, ComputeShaderVariant> computeShaders;
			std::unordered_map<int, EffectVariant> effects;	
		};

		std::unordered_map<int, Variant> variants;
		ShaderLibMap libMap;
		Device* pDev;

		Variant& GetVariant(const int vID);
	};
}
