#pragma once
#include "ShaderLibMap.hpp"
#include "D3D11/Shaders/EffectVariant.hpp"
#include "D3D11/Shaders/ShaderVariants.hpp"

namespace Replica::D3D11
{
	using Effects::ShaderLibDef;
	using Effects::ShaderLibMap;
	using Effects::ShadeStages;
	using Effects::EffectDef;

	class Device;
	class Material;
	class ComputeInstance;

	class ShaderLibrary
	{
	public:
		MAKE_NO_COPY(ShaderLibrary)

		ShaderLibrary();

		ShaderLibrary(Device& device, const ShaderLibDef& def);

		ShaderLibrary(Device& device, ShaderLibDef&& def);

		/// <summary>
		/// Returns a new material using the given effect
		/// </summary>
		Material GetMaterial(uint effectNameID);

		/// <summary>
		/// Returns a new Compute shader instance using the given shader name
		/// </summary>
		ComputeInstance GetComputeInstance(uint nameID);

		/// <summary>
		/// Returns a new material using the given effect
		/// </summary>
		Material GetMaterial(string_view effectName);

		/// <summary>
		/// Returns a new Compute shader instance using the given shader name
		/// </summary>
		ComputeInstance GetComputeInstance(string_view name);

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
				GFX_THROW("Invalid shader specified");
		}

		/// <summary>
		/// Retrieves the give effect variant
		/// </summary>
		const EffectVariant& GetEffect(uint effectID);

		/// <summary>
		/// Retrieves interface for querying shader definitions
		/// </summary>
		const ShaderLibMap& GetLibMap() const;

		/// <summary>
		/// Retrieves interface for querying string IDs used in library resources
		/// </summary>
		const StringIDMap& GetStringMap() const { return libMap.GetStringMap(); }

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
