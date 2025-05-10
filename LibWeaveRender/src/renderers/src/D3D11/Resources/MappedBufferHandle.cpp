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

uint MappedBufferHandle::GetRowPitch() const { return msr.rowPitch; }

uint MappedBufferHandle::GetDepthPitch() const { return msr.depthPitch; }

uint MappedBufferHandle::GetByteSize() const { return (uint)GetLength(); }

static uint GetTotalExtent(uivec3 dim, MappedSubresource msr)
{
	const uint planeSize = msr.rowPitch * dim.y;
	const uint cubeSize = msr.depthPitch * dim.z;
	return std::max(planeSize, cubeSize);
}

MappedBufferHandle::MappedBufferHandle(IBuffer& parent, MappedSubresource msr) :
	DataBufferSpan(&parent, (byte*)msr.pData, GetTotalExtent(parent.GetDimensions(), msr)),
	msr(msr)
{ }
