#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Shaders/ShaderVariants.hpp"

using namespace Weave;
using namespace Weave::D3D11;

DEF_DEST_MOVE(VertexShaderVariant);
DEF_DEST_MOVE(PixelShaderVariant);
DEF_DEST_MOVE(ComputeShaderVariant);

VertexShaderVariant::VertexShaderVariant(Device& dev, const ShaderDefHandle& def) :
	ShaderVariant(dev, def),
	layout(dev, def.GetBinSrc().GetData(), def.GetBinSrc().GetLength(), def.GetInLayout())
{ }

ComputeShaderVariant::ComputeShaderVariant(Device& dev, const ShaderDefHandle& def) :
	ShaderVariant(dev, def)
{
	std::optional<ResourceGroupHandle> resData = def.GetResources();
	
	if (resData.has_value())
	{
		pUAVmap.reset(new UnorderedAccessMap(resData));
	}
}

const UnorderedAccessMap* ComputeShaderVariant::GetUAVMap() const { return pUAVmap.get(); }
