#pragma once
#include "WeaveUtils/Span.hpp"
#include "../Resources/ResourceBase.hpp"
#include "../Resources/Sampler.hpp"
#include "../Resources/ResourceHandles.hpp"
#include "../Resources/ComputeBuffer.hpp"

namespace Weave::D3D11
{
	class ShaderVariantManager;
	class EffectVariant;
	class ResourceSet;

	using TextureHandle = ResourceHandle<const ITexture2D, const IShaderResource>;
	using RWTextureHandle = ResourceHandle<IRWTexture2D, IUnorderedAccess>;
	using ComputeBufferHandle = ResourceHandle<const ComputeBuffer, const IShaderResource>;
	using RWComputeBufferHandle = ResourceHandle<ComputeBuffer, IUnorderedAccess>;
	using SamplerHandle = ResourceHandle<const Sampler, const Sampler>;

	class ShaderInstanceBase
	{
	public:
		DECL_MOVE_ONLY(ShaderInstanceBase)

		/// <summary>
		/// Returns a unique int ID associated with the name of the underlying unique resource
		/// </summary>
		uint GetNameID() const;

		/// <summary>
		/// Returns the name of the underlying unique resource
		/// </summary>
		string_view GetName() const;

		/// <summary>
		/// Returns a unique int ID for the resource corresponding to the given name
		/// </summary>
		uint GetStringID(string_view str) const;

		/// <summary>
		/// Attempts to retrieve a unique int ID for the resource corresponding to the given name
		/// </summary>
		bool TryGetStringID(string_view str, uint& id) const;

		/// <summary>
		/// Returns the string corresponding to the given ID
		/// </summary>
		string_view GetString(uint stringID) const;

		/// <summary>
		/// Returns a strongly typed constant wrapper for the given constant
		/// </summary>
		template<typename T>
		ConstantHandle<T> GetConstant(uint nameID) { WV_ASSERT(pRes.get() != nullptr); return ConstantHandle<T>(nameID, *pRes.get()); }

		/// <summary>
		/// Returns a strongly typed constant wrapper for the given constant
		/// </summary>
		template<typename T>
		ConstantHandle<T> GetConstant(string_view name) { return GetConstant<T>(GetStringID(name)); }

		/// <summary>
		/// Writes the given raw binary to the corresponding shader constant. Validates size, but is not type safe.
		/// </summary>
		void SetConstant(uint nameID, const Span<byte>& newValue);

		/// <summary>
		/// Writes the given value to the corresponding named shader constant. Validates size, not type. The value given 
		/// must match the value declared in the shader.
		/// </summary>
		template<typename T>
		void SetConstant(uint nameID, const T& value) { SetConstant(nameID, {(byte*)&value, sizeof(T)}); }

		/// <summary>
		/// Writes the given value to the corresponding named shader constant. Validates size, not type. The value given 
		/// must match the value declared in the shader.
		/// </summary>
		template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
		void SetConstant(uint nameID, const glm::mat<C, R, T, Q>& value)
		{
			const glm::mat<C, R, T, Q> x = transpose(value);
			SetConstant(nameID, {(byte*)&x, sizeof(glm::mat<C, R, T, Q>)});
		}

		/// <summary>
		/// Writes the given raw binary to the corresponding shader constant. Validates size, but is not type safe.
		/// </summary>
		void SetConstant(string_view name, const Span<byte>& newValue);

		/// <summary>
		/// Writes the given value to the corresponding named shader constant. Validates size, not type. The value given 
		/// must match the value declared in the shader.
		/// </summary>
		template<typename T>
		void SetConstant(string_view name, const T& value) { SetConstant(name, {(byte*)&value, sizeof(T)}); }

		/// <summary>
		/// Writes the given value to the corresponding named shader constant. Validates size, not type. The value given 
		/// must match the value declared in the shader.
		/// </summary>
		template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
		void SetConstant(string_view name, const glm::mat<C, R, T, Q>& value)
		{
			const glm::mat<C, R, T, Q> x = transpose(value);
			SetConstant(name, {(byte*)&x, sizeof(glm::mat<C, R, T, Q>)});
		}

		/// <summary>
		/// Returns a read/writable handle for the sampler slot with the given name
		/// </summary>
		SamplerHandle GetSampler(uint stringID);

		/// <summary>
		/// Returns a read/writable handle for the sampler slot with the given name
		/// </summary>
		SamplerHandle GetSampler(string_view name);

		/// <summary>
		/// Sets global sampler
		/// </summary>
		void SetSampler(uint stringID, const Sampler& samp);

		/// <summary>
		/// Sets global sampler
		/// </summary>
		void SetSampler(string_view name, const Sampler& samp);

		/// <summary>
		/// Returns a read/writable handle for the SRV slot with the given name
		/// </summary>
		TextureHandle GetTexture(uint stringID);

		/// <summary>
		/// Returns a read/writable handle for the SRV slot with the given name
		/// </summary>
		TextureHandle GetTexture(string_view name);

		/// <summary>
		/// Sets global texture
		/// </summary>
		void SetTexture(uint stringID, const IShaderResource& tex);

		/// <summary>
		/// Sets global texture 
		/// </summary>
		void SetTexture(string_view name, const IShaderResource& tex);

		/// <summary>
		/// Returns a read/writable handle for the UAV slot with the given name
		/// </summary>
		RWTextureHandle GetRWTexture(uint stringID);

		/// <summary>
		/// Returns a read/writable handle for the UAV slot with the given name
		/// </summary>
		RWTextureHandle GetRWTexture(string_view name);

		/// <summary>
		/// Sets global random access texture/buffer
		/// </summary>
		void SetRWTexture(uint stringID, IUnorderedAccess& rwTex);

		/// <summary>
		/// Sets global random access texture/buffer
		/// </summary>
		void SetRWTexture(string_view name, IUnorderedAccess& rwTex);

		/// <summary>
		/// Returns string IDs for mode name and flags set
		/// </summary>
		void GetDefines(Vector<uint>& nameIDs) const;

		/// <summary>
		/// Returns mode name and flags set
		/// </summary>
		void GetDefines(Vector<string_view>& names) const;

		/// <summary>
		/// Returns true if the given mode or flag is set
		/// </summary>
		bool GetIsDefined(uint nameID) const;

		/// <summary>
		/// Returns true if the given mode or flag is set
		/// </summary>
		bool GetIsDefined(string_view name) const;

		/// <summary>
		/// Sets the given flag to the given value
		/// </summary>
		void SetFlag(uint nameID, bool value);

		/// <summary>
		/// Sets the given flag to the given value
		/// </summary>
		void SetFlag(string_view name, bool value);

		/// <summary>
		/// Sets the variant to the given mode
		/// </summary>
		void SetMode(uint nameID);

		/// <summary>
		/// Sets the variant to the given mode
		/// </summary>
		void SetMode(string_view name);

		/// <summary>
		/// Resets the variant to its default mode
		/// </summary>
		void ResetMode();

		/// <summary>
		/// Resets mode and all flags
		/// </summary>
		void ResetDefines();

	protected:
		std::unique_ptr<ResourceSet> pRes;
		ShaderVariantManager* pLib;
		uint nameID;
		uint vID;

		ShaderInstanceBase();

		virtual ~ShaderInstanceBase();

		ShaderInstanceBase(ShaderVariantManager& lib, uint nameID, uint vID);

		uint GetVariantID() const;

		virtual void SetVariantID(uint vID) = 0;
	};
}
