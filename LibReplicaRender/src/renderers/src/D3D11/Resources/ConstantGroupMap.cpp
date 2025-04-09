#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Context.hpp"
#include "D3D11/Resources/ConstantGroupMap.hpp"
#include "D3D11/Resources/ConstantBuffer.hpp"

using namespace Replica;
using namespace Replica::Effects;
using namespace Replica::D3D11;

ConstantGroupMap::ConstantGroupMap() :
	totalSize(0u)
{ }

ConstantGroupMap::ConstantGroupMap(const std::optional<ConstBufGroupHandle>& layout) :
	cbufMaps(layout.has_value() ? layout->GetLength() : 0u),
	totalSize(0u)
{
	if (layout.has_value())
	{
		const uint cbufCount = (uint)cbufMaps.GetLength();

		// Copy constant layout for each buffer
		for (uint i = 0; i < cbufCount; i++)
		{
			const ConstBufDefHandle bufDef = (*layout)[i];
			const uint bufLen = (uint)bufDef.GetLength();
			cbufMaps[i] = UniqueArray<ConstDef>(bufLen);

			for (uint j = 0; j < bufLen; j++)
				cbufMaps[i][j] = bufDef[j];

			totalSize += bufDef.GetSize();
		}

		// Create data buffers
		cbufData = UniqueArray<byte>(totalSize);
		cbufSpans = UniqueArray<Span<byte>>(cbufCount);
		uint bufOffset = 0u;

		for (uint i = 0; i < cbufCount; i++)
		{
			const ConstBufDefHandle bufDef = (*layout)[i];
			const uint bufSize = bufDef.GetSize();
			cbufSpans[i] = Span<byte>(&cbufData[bufOffset], bufSize);
			bufOffset += bufSize;
		}
	}
}

const IDynamicArray<Span<byte>>& ConstantGroupMap::GetData(const GroupData& src) const
{
	for (uint cbuf = 0; cbuf < (uint)cbufMaps.GetLength(); cbuf++)
	{
		Span<byte>& bufSpan = cbufSpans[cbuf];

		for (const ConstDef& dstConst : cbufMaps[cbuf])
		{
			// Look up constant in source data using dst constant names
			const auto& it = src.stringConstMap.find(dstConst.stringID);

			if (it != src.stringConstMap.end()) // Src assigned this cbuf member
			{
				const ConstantData& srcConst = src.constants[it->second];
				const byte* pSrcStart = &src.data[srcConst.offset];
				byte* pDstStart = &bufSpan[dstConst.offset];
				
				GFX_ASSERT(srcConst.size <= dstConst.size, "Constant data source cannot be larger than constant destination");
				memcpy(pDstStart, pSrcStart, srcConst.size);
			}
		}
	}

	return cbufSpans;
}

uint ConstantGroupMap::GetTotalSize() const { return totalSize; }

uint ConstantGroupMap::GetBufferSize(sint buffer) const { return (uint)cbufSpans[buffer].GetLength(); }

uint ConstantGroupMap::GetBufferCount() const { return (uint)cbufMaps.GetLength(); }
