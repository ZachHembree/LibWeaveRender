#include "pch.hpp"
#include "ReplicaInternalD3D11.hpp"
#include "ShaderLibGen/ShaderData.hpp"
#include "D3D11/Shaders/ShaderBase.hpp"
#include "D3D11/Resources/Sampler.hpp"

using namespace Replica::Effects;
using namespace Replica::D3D11;
using namespace Replica;

ShaderBase::ShaderBase() = default;

ShaderBase::ShaderBase(Device& dev, const ShaderDef& def) :
	samplers(def.res, ShaderTypes::Sampler),
	textures(def.res, ShaderTypes::Texture),
	cBuffers(def.constBufs.GetLength())
{
	for (int i = 0; i < def.constBufs.GetLength(); i++)
	{
		const ConstBufLayout& layout = def.constBufs[i];
		constants.emplace(string_view(layout.name), ConstantMap(layout));
		cBuffers[i] = ConstantBuffer(dev, layout.size);
	}
}

ShaderBase::ShaderBase(ShaderBase&& other) noexcept = default;

ShaderBase& ShaderBase::operator=(ShaderBase&& other) noexcept = default;

void ShaderBase::SetBuffer(string_view name, const byte* pSrc, const size_t size)
{
	const auto it = constants.find(name);

	if (it != constants.end())
	{
		it->second.SetData(pSrc, size);
	}
}

void ShaderBase::SetConstant(string_view name, const byte* pSrc, const size_t size)
{
	for (auto& pair : constants)
	{
		if (pair.second.GetMemberExists(name))
		{
			pair.second.SetMember(name, pSrc, size);
			break;
		}
	}
}

void ShaderBase::SetSampler(string_view name, Sampler& samp)
{
	samplers.SetResource(name, samp.Get());
}

void ShaderBase::SetTexture(string_view name, ITexture2D& tex)
{
	textures.SetResource(name, tex.GetSRV());
}

void ShaderBase::UpdateConstants(Context& ctx)
{
	int i = 0;

	for (const auto& pair : constants)
	{
		pair.second.UpdateConstantBuffer(cBuffers[i], ctx);
		i++;
	}
}
