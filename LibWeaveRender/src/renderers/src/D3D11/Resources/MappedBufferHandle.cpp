#include "pch.hpp"
#include "D3D11/Resources/MappedBufferHandle.hpp"

using namespace Weave;
using namespace Weave::D3D11;

MappedBufferHandle::MappedBufferHandle() :
	DataBufferSpan(),
	msr({}),
	subresIndex(0u)
{}

MappedBufferHandle::MappedBufferHandle(MappedBufferHandle&&) noexcept = default;

MappedBufferHandle& MappedBufferHandle::operator=(MappedBufferHandle&&) noexcept = default;

bool MappedBufferHandle::GetCanRead() const { return (uint)(pParent->GetAccessFlags() & ResourceAccessFlags::Read) > 0; }

bool MappedBufferHandle::GetCanWrite() const { return (uint)(pParent->GetAccessFlags() & ResourceAccessFlags::Write) > 0; }

uint MappedBufferHandle::GetRowPitch() const { return msr.RowPitch; }

uint MappedBufferHandle::GetDepthPitch() const { return msr.DepthPitch; }

uint MappedBufferHandle::GetByteSize() const { return (uint)length; }

static uint GetTotalExtent(uivec3 dim, D3D11_MAPPED_SUBRESOURCE msr)
{
	const uint planeSize = msr.RowPitch * dim.y;
	const uint cubeSize = msr.DepthPitch * dim.z;
	return std::max(planeSize, cubeSize);
}

MappedBufferHandle::MappedBufferHandle(IBuffer& parent, D3D11_MAPPED_SUBRESOURCE msr) :
	DataBufferSpan(&parent, (byte*)msr.pData, GetTotalExtent(parent.GetDimensions(), msr)),
	msr(msr)
{ }
