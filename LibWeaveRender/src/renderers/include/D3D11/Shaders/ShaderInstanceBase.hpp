#pragma once
#include "../Resources/ResourceBase.hpp"
#include "../Resources/Sampler.hpp"

namespace Weave::D3D11
{
	class ShaderVariantManager;
	class EffectVariant;
	class ResourceSet;

	class ShaderInstanceBase
	{
	public:
		MAKE_NO_COPY(ShaderInstanceBase)

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
		/// Sets the value corresponding to the given name to the
		/// given value.
		/// </summary>
		void SetConstant(uint nameID, const Span<byte>& newValue);

		/// <summary>
		/// Sets the value corresponding to the given name to the
		/// given value.
		/// </summary>
		template<typename T>
		void SetConstant(uint nameID, const T& value) { SetConstant(nameID, {(byte*)&value, sizeof(T)}); }

		/// <summary>
		/// Sets the value corresponding to the given name to the
		/// given value.
		/// </summary>
		template<>
		void SetConstant<mat4>(uint nameID, const mat4& value)
		{
			const mat4 x = transpose(value);
			SetConstant(nameID, {(byte*)&x, sizeof(mat4)});
		}

		/// <summary>
		/// Sets the value corresponding to the given name to the
		/// given value.
		/// </summary>
		template<>
		void SetConstant<mat3>(uint nameID, const mat3& value)
		{
			const mat3 x = transpose(value);
			SetConstant(nameID, {(byte*)&x, sizeof(mat3)});
		}

		/// <summary>
		/// Sets the value corresponding to the given name to the
		/// given value.
		/// </summary>
		void SetConstant(string_view name, const Span<byte>& newValue);

		/// <summary>
		/// Sets the value corresponding to the given name to the
		/// given value.
		/// </summary>
		template<typename T>
		void SetConstant(string_view name, const T& value) { SetConstant(name, {(byte*)&value, sizeof(T)}); }

		/// <summary>
		/// Sets the value corresponding to the given name to the
		/// given value.
		/// </summary>
		template<>
		void SetConstant<mat4>(string_view name, const mat4& value)
		{
			const mat4 x = transpose(value);
			SetConstant(name, {(byte*)&x, sizeof(mat4)});
		}

		/// <summary>
		/// Sets the value corresponding to the given name to the
		/// given value.
		/// </summary>
		template<>
		void SetConstant<mat3>(string_view name, const mat3& value)
		{
			const mat3 x = transpose(value);
			SetConstant(name, {(byte*)&x, sizeof(mat3)});
		}

		/// <summary>
		/// Sets global sampler
		/// </summary>
		void SetSampler(uint stringID, Sampler& samp);

		/// <summary>
		/// Sets global sampler
		/// </summary>
		void SetSampler(string_view name, Sampler& samp);

		/// <summary>
		/// Sets global texture
		/// </summary>
		void SetTexture(uint stringID, IShaderResource& tex);

		/// <summary>
		/// Sets global texture 
		/// </summary>
		void SetTexture(string_view name, IShaderResource& tex);

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

		~ShaderInstanceBase();

		ShaderInstanceBase(ShaderVariantManager& lib, uint nameID, uint vID);

		ShaderInstanceBase(ShaderInstanceBase&&) noexcept;

		ShaderInstanceBase& operator=(ShaderInstanceBase&&) noexcept;

		uint GetVariantID() const;

		virtual void SetVariantID(uint vID) = 0;
	};
}
