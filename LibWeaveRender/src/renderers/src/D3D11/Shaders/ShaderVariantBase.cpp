#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/CtxBase.hpp"
#include "D3D11/ShaderLibrary.hpp"
#include "D3D11/Shaders/ShaderVariantBase.hpp"

using namespace Weave;
using namespace Weave::D3D11;

ShaderVariantBase::ShaderVariantBase(ShaderVariantBase&& other) noexcept = default;

ShaderVariantBase& ShaderVariantBase::operator=(ShaderVariantBase&& other) noexcept = default;

uint ShaderVariantBase::GetNameID() const { return def.GetNameID(); }

ShaderDefHandle ShaderVariantBase::GetDefinition() const { return def; }

const ConstantGroupMap* ShaderVariantBase::GetConstantMap() const { return pConstants.get(); }

IDynamicArray<ConstantBuffer>& ShaderVariantBase::GetConstantBuffers() const { return cbufs; }

const ResourceViewMap* ShaderVariantBase::GetSRVMap() const { return pSrvMap.get(); }

const SamplerMap* ShaderVariantBase::GetSampMap() const { return pSampMap.get(); }

const UnorderedAccessMap* ShaderVariantBase::GetUAVMap() const { return nullptr; }

ShaderVariantBase::ShaderVariantBase() :
	DeviceChild()
{ }

ShaderVariantBase::ShaderVariantBase(Device& dev, const ShaderDefHandle& def) :
	DeviceChild(dev),
	def(def)
{ 
	std::optional<ConstBufGroupHandle> bufData = def.GetConstantBuffers();

	if (bufData.has_value())
	{ 
		pConstants.reset(new ConstantGroupMap(bufData));
		cbufs = UniqueArray<ConstantBuffer>(bufData->GetLength());

		for (int i = 0; i < cbufs.GetLength(); i++)
			cbufs[i] = ConstantBuffer(dev, (*bufData)[i].GetSize());
	}

	std::optional<ResourceGroupHandle> resData = def.GetResources();

	if (resData.has_value())
	{
		pSampMap.reset(new SamplerMap(resData));
		pSrvMap.reset(new ResourceViewMap(resData));
	}
}

