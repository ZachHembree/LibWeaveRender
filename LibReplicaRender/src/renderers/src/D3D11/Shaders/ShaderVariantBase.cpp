#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Context.hpp"
#include "D3D11/ShaderLibrary.hpp"
#include "D3D11/Shaders/ShaderVariantBase.hpp"

using namespace Replica;
using namespace Replica::D3D11;

uint ShaderVariantBase::GetNameID() const { return def.GetNameID(); }

ShaderDefHandle ShaderVariantBase::GetDefinition() const { return def; }

ShaderVariantBase::ShaderVariantBase() :
	DeviceChild()
{ }

ShaderVariantBase::ShaderVariantBase(Device& dev, const ShaderDefHandle& def) :
	DeviceChild(dev),
	def(def),
	sampMap(def.GetResources(), ShaderTypes::Sampler),
	srvMap(def.GetResources(), ShaderTypes::Texture)
{ 
	ConstBufGroupHandle bufData = def.GetConstantBuffers();
	cbufs = UniqueArray<ConstantBuffer>(bufData.GetLength());

	for (int i = 0; i < cbufs.GetLength(); i++)
	{
		cbufs[i] = ConstantBuffer(dev, bufData[i].GetSize());
	}
}

ShaderVariantBase::ShaderVariantBase(ShaderVariantBase&& other) noexcept = default;

ShaderVariantBase& ShaderVariantBase::operator=(ShaderVariantBase&& other) noexcept = default;

void ShaderVariantBase::MapResources(Context& ctx, const ResourceSet& res) const
{
	const ShadeStages stage = def.GetStage();
	ctx.SetSamplers(res.GetSamplers(), sampMap, stage);
	ctx.SetSRVs(res.GetSRVs(), srvMap, stage);

	const IDynamicArray<Span<byte>>& constData = res.GetMappedConstants(constants);
	ctx.SetConstants(constData, cbufs, stage);
}

void ShaderVariantBase::UnmapResources(Context& ctx) const
{
	const ShadeStages stage = def.GetStage();
	ctx.ClearSamplers(0, (uint)sampMap.GetCount(), stage);
	ctx.ClearSRVs(0, (uint)srvMap.GetCount(), stage);
	ctx.ClearConstants(0, (uint)constants.GetBufferCount(), stage);
}