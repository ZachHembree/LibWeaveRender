#pragma once
#include "ReplicaInternalD3D11.hpp"
#include "../Device.hpp"
#include "../Resources/DeviceChild.hpp"

namespace Replica::D3D11
{
	using Effects::ShaderDef;

	class ShaderLibrary;

	class ShaderVariantBase : public DeviceChild
	{
	public:
		MAKE_NO_COPY(ShaderVariantBase)

		string_view GetName() const;

		const ShaderLibrary& GetLibrary() const;

		ShaderLibrary& GetLibrary();

		const ShaderDef& GetDefinition() const;

		bool GetIsDefined(string_view define) const;

	protected:
		ShaderLibrary* pLib;
		const ShaderDef* pDef;

		ShaderVariantBase();

		ShaderVariantBase(Device& dev, ShaderLibrary& lib, const ShaderDef& def);

		ShaderVariantBase(ShaderVariantBase&& other) noexcept;

		ShaderVariantBase& operator=(ShaderVariantBase&& other) noexcept;
	};

	template<typename ShaderT>
	class ShaderVariant : public ShaderVariantBase
	{
	public:
		ShaderVariant() : ShaderVariantBase()
		{ }

		ShaderVariant(Device& dev, ShaderLibrary& lib, const ShaderDef& def) :
			ShaderVariantBase(dev, lib, def)
		{
			dev.CreateShader(def.binSrc, pShader);
		}

		ShaderT* Get() const { return pShader.Get(); }

		const ShaderVariant& GetVariant(const int vID) const 
		{
			ShaderVariantBase& newVariant = pLib->GetShader(GetName(), vID);
			GFX_ASSERT(newVariant.GetDefinition().stage == GetDefinition().stage, "Shader variant undefined");

			return reinterpret_cast<ShaderVariant&>(newVariant);
		}

	protected:
		ComPtr<ShaderT> pShader;
	};

	class VertexShaderVariant : public ShaderVariant<ID3D11VertexShader>
	{ 
		using ShaderVariant::ShaderVariant;
	};

	class PixelShaderVariant : public ShaderVariant<ID3D11PixelShader>
	{
		using ShaderVariant::ShaderVariant;
	};

	class ComputeShaderVariant : public ShaderVariant<ID3D11ComputeShader>
	{
		using ShaderVariant::ShaderVariant;
	};
}
