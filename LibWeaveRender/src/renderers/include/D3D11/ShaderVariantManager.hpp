#pragma once
#include "WeaveEffects/ShaderLibMap.hpp"
#include "Shaders/ShaderVariants.hpp"
#include "Shaders/EffectVariant.hpp"

namespace Weave::D3D11
{
	using Effects::ShaderLibDef;
	using Effects::ShaderLibMap;
	using Effects::ShadeStages;
	using Effects::EffectDef;

	class ShaderVariantManager
	{
	public:
		MAKE_NO_COPY(ShaderVariantManager)

		ShaderVariantManager();

		ShaderVariantManager(Device& device, const ShaderLibDef& def);

		ShaderVariantManager(Device& device, ShaderLibDef&& def);

		/// <summary>
		/// Retrieves interface for querying string IDs used in library resources
		/// </summary>
		const StringIDMap& GetStringMap() const;

		/// <summary>
		/// Tries to retrieve the given shader as a vertex shader
		/// </summary>
		bool TryGetShader(uint shaderID, const VertexShaderVariant*& pVS);

		/// <summary>
		/// Tries to retrieve the given shader as a pixel shader
		/// </summary>
		bool TryGetShader(uint shaderID, const PixelShaderVariant*& pPS);

		/// <summary>
		/// Tries to retrieve the given shader as a compute shader
		/// </summary>
		bool TryGetShader(uint shaderID, const ComputeShaderVariant*& pCS);

		/// <summary>
		/// Retrieve the given shader as the given subtype
		/// </summary>
		template<typename ShaderT> const ShaderT& GetShader(uint shaderID)
		{
			const ShaderT* pShader = nullptr;

			if (TryGetShader(shaderID, pShader))
				return *pShader;
			else
				D3D_THROW("Invalid shader specified");
		}

		/// <summary>
		/// Retrieves interface for querying shader definitions
		/// </summary>
		const ShaderLibMap& GetLibMap() const;

		/// <summary>
		/// Retrieves the give effect variant
		/// </summary>
		const EffectVariant& GetEffect(uint effectID);

	private:
		// Sparse variant instantiation
		std::unordered_map<uint, VertexShaderVariant> vertexShaders;
		std::unordered_map<uint, PixelShaderVariant> pixelShaders;
		std::unordered_map<uint, ComputeShaderVariant> computeShaders;
		std::unordered_map<uint, EffectVariant> effects;

		ShaderLibMap libMap;
		Device* pDev;
	};
}