#pragma once
#include "ReplicaInternalD3D11.hpp"
#include "ShaderDataHandles.hpp"
#include "../Device.hpp"
#include "../Resources/DeviceChild.hpp"
#include "../Resources/ConstantBuffer.hpp"
#include "../Resources/ConstantGroupMap.hpp"

namespace Replica::D3D11
{
	using Effects::ShaderDef;
	using Effects::ShaderDefHandle;

	class ShaderLibrary;

	class ShaderVariantBase : public DeviceChild
	{
	public:
		MAKE_NO_COPY(ShaderVariantBase)

		uint GetNameID() const;

		ShaderDefHandle GetDefinition() const;

		virtual void Bind(Context& ctx, const ResourceSet& res) const = 0;

		virtual void Unbind(Context& ctx) const = 0;

		virtual void MapResources(Context& ctx, const ResourceSet& res) const;

		virtual void UnmapResources(Context& ctx) const;

	protected:
		ShaderDefHandle def;
		ConstantGroupMap constants;
		SamplerMap sampMap;
		ResourceViewMap srvMap;

		mutable UniqueArray<ConstantBuffer> cbufs;

		ShaderVariantBase();

		ShaderVariantBase(Device& dev, const ShaderDefHandle& def);

		ShaderVariantBase(ShaderVariantBase&& other) noexcept;

		ShaderVariantBase& operator=(ShaderVariantBase&& other) noexcept;
	};
}
