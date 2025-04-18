#include "pch.hpp"
#include "D3D11/Shaders/ShaderVariants.hpp"

using namespace Weave;
using namespace Weave::D3D11;

VertexShaderVariant::VertexShaderVariant(Device& dev, const ShaderDefHandle& def) :
	ShaderVariant(dev, def),
	layout(dev, def.GetBinSrc().GetData(), def.GetBinSrc().GetLength(), def.GetInLayout())
{ }

VertexShaderVariant::~VertexShaderVariant() = default;

ComputeShaderVariant::ComputeShaderVariant(Device& dev, const ShaderDefHandle& def) :
	ShaderVariant(dev, def)
{
	std::optional<ResourceGroupHandle> resData = def.GetResources();
	
	if (resData.has_value())
	{
		pUAVmap.reset(new UnorderedAccessMap(resData));
	}
}

ComputeShaderVariant::~ComputeShaderVariant() = default;

const UnorderedAccessMap* ComputeShaderVariant::GetUAVMap() const { return pUAVmap.get(); }

PixelShaderVariant::~PixelShaderVariant() = default;
