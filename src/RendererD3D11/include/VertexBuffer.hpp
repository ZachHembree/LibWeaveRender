#pragma once
#include "BufferBase.hpp"

class VertexBuffer : public BufferBase
{
public:

	template<typename T>
	VertexBuffer(
		const Microsoft::WRL::ComPtr<ID3D11Device>& pDevice,
		const DynamicArrayBase<T>& data,
		BufferUsages usage = BufferUsages::Default, 
		BufferAccessFlags cpuAccess = BufferAccessFlags::None) : 
		BufferBase(BufferTypes::Vertex, usage, cpuAccess, pDevice, data)
	{ }

	template<typename T>
	VertexBuffer(
		const Microsoft::WRL::ComPtr<ID3D11Device>& pDevice,
		const std::vector<T>& data,
		BufferUsages usage = BufferUsages::Default,
		BufferAccessFlags cpuAccess = BufferAccessFlags::None) :
		BufferBase(BufferTypes::Vertex, usage, cpuAccess, pDevice, data)
	{ }

private:
};