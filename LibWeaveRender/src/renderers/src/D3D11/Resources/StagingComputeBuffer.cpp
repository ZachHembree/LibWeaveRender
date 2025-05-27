#include "pch.hpp"
#include "D3D11/Resources/StagingComputeBuffer.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Device.hpp"

using namespace Weave;
using namespace Weave::D3D11;

DEF_DEST_MOVE(StagingComputeBuffer);

StagingComputeBuffer::StagingComputeBuffer() = default;

StagingComputeBuffer::StagingComputeBuffer(Device& device) :
	ComputeBufferBase(
		ResourceBindFlags::None,
		ResourceUsages::Staging,
		ResourceAccessFlags::ReadWrite,
		device
	)
{ }

StagingComputeBuffer::StagingComputeBuffer(Device& device, const uint count, const uint typeSize, const void* data) :
	ComputeBufferBase(
		ResourceBindFlags::None,
		ResourceUsages::Staging,
		ResourceAccessFlags::ReadWrite,
		device,
		count,
		typeSize,
		data)
{ }
