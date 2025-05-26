#include "pch.hpp"
#include "D3D11/Resources/MappedBufferHandle.hpp"
#include "D3D11/CtxBase.hpp"

using namespace Weave;
using namespace Weave::D3D11;

MappedBufferHandle::MappedBufferHandle() :
	DataBufferSpan(),
	pContext(nullptr),
	msr({})
{}

MappedBufferHandle::~MappedBufferHandle()
{
	if (pContext != nullptr && msr.pData != nullptr)
	{
		pContext->ReturnMappedBufferHandle(std::move(*this));
	}

	pContext = nullptr;
	msr.pData = nullptr;
}

MappedBufferHandle::MappedBufferHandle(MappedBufferHandle&& other) noexcept :
	DataBufferSpan(std::move(other)),
	pContext(other.pContext),
	msr(other.msr)
{
	other.pContext = nullptr;
	other.msr.pData = nullptr;
}

MappedBufferHandle& MappedBufferHandle::operator=(MappedBufferHandle&& other) noexcept
{
	DataBufferSpan::operator=(std::move(other));
	pContext = other.pContext;
	msr = other.msr;

	other.pContext = nullptr;
	other.msr.pData = nullptr;
	return *this;
}

bool MappedBufferHandle::GetCanRead() const { return (uint)(pParent->GetAccessFlags() & ResourceAccessFlags::Read) > 0; }

bool MappedBufferHandle::GetCanWrite() const { return (uint)(pParent->GetAccessFlags() & ResourceAccessFlags::Write) > 0; }

uint MappedBufferHandle::GetRowPitch() const { return msr.rowPitch; }

uint MappedBufferHandle::GetDepthPitch() const { return msr.depthPitch; }

uint MappedBufferHandle::GetByteSize() const { return (uint)GetLength(); }

static uint GetTotalExtent(uivec3 dim, const MappedSubresource& msr)
{
	const uint planeSize = msr.rowPitch * dim.y;
	const uint cubeSize = msr.depthPitch * dim.z;
	return std::max(planeSize, cubeSize);
}

MappedBufferHandle::MappedBufferHandle(CtxBase& ctx, IBuffer& parent, const MappedSubresource& msr) :
	DataBufferSpan(&parent, (byte*)msr.pData, GetTotalExtent(parent.GetDimensions(), msr)),
	pContext(&ctx),
	msr(msr)
{ }

void MappedBufferHandle::UpdateExtent()
{
	MappedBufferHandle newHandle(*pContext, *pParent, msr);
	pContext = nullptr;
	*this = std::move(newHandle);
}
