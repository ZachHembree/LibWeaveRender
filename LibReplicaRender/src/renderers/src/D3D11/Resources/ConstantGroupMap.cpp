#include "pch.hpp"
#include "ReplicaInternalD3D11.hpp"
#include "D3D11/Context.hpp"
#include "D3D11/Resources/ConstantGroupMap.hpp"
#include "D3D11/Resources/ConstantBuffer.hpp"

using namespace Replica;
using namespace Replica::Effects;
using namespace Replica::D3D11;

ConstantGroupMap::ConstantGroupMap()
{ }

ConstantGroupMap::ConstantGroupMap(const ConstBufGroupHandle& layout) :
	group(layout.GetLength()),
	size(0u)
{
	for (int i = 0; i < group.GetLength(); i++)
	{
		const ConstBufDefHandle bufDef = layout[i];
		ConstantMap& map = group[i];
		uint offset = 0u;
		map.index = i;
		map.size = bufDef.GetSize();
		map.constMap.reserve(layout.GetLength());

		for (int j = 0; j < bufDef.GetLength(); j++)
		{
			const ConstDef& member = bufDef[i];
			const Constant entry(member.size, offset);

			map.constMap.emplace(member.stringID, entry);
			offset += member.size;
		}

		size += map.size;
	}
}

bool ConstantGroupMap::GetMemberExists(uint nameID, sint buffer) const
{
	const auto& pair = group[buffer].constMap.find(nameID);
	return pair != group[buffer].constMap.end();
}

void ConstantGroupMap::SetData(const GroupData& src, IDynamicArray<Span<byte>>& dstBufs) const
{
	for (const ConstantData& srcConst : src.constants)
	{
		for (const ConstantMap& map : group)
		{
			const auto& it = map.constMap.find(srcConst.stringID);

			if (it != map.constMap.end())
			{
				IDynamicArray<byte>& subBuf = dstBufs[map.index];
				const Constant& dstConst = it->second;
				const byte* pSrcStart = &src.data[srcConst.offset];
				byte* pDstStart = &subBuf[dstConst.offset];

				GFX_ASSERT(dstConst.size >= srcConst.size, "Constant data cannot be larger than constant destination")
				memcpy(pDstStart, pSrcStart, dstConst.size);

				break;
			}
		}
	}
}

uint ConstantGroupMap::GetTotalSize() const { return size; }

uint ConstantGroupMap::GetBufferSize(sint buffer) const { return group[buffer].size; }

uint ConstantGroupMap::GetBufferCount() const { return (uint)group.GetLength(); }

ConstantGroupMap::Constant::Constant() : 
	size(0), offset(0)
{ }

ConstantGroupMap::Constant::Constant(const uint size, const uint offset) :
	size(size),
	offset(offset)
{ }
