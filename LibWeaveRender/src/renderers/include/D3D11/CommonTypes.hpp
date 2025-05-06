#pragma once
#include "D3D11Utils.hpp"
#include "CommonEnums.hpp"

struct ID3D11Buffer;
struct ID3D11Resource;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11UnorderedAccessView;
struct ID3D11DepthStencilState;
struct ID3D11DepthStencilView;

struct IDXGIOutput1;
struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11ComputeShader;
struct ID3D11SamplerState;

struct D3D11_MAPPED_SUBRESOURCE;
struct D3D11_BUFFER_DESC;
struct D3D11_TEXTURE2D_DESC;
struct DXGI_SAMPLE_DESC;

namespace Weave::D3D11
{
	struct MappedSubresource
	{
		void* pData;
		uint rowPitch;
		uint depthPitch;

		D3D11_MAPPED_SUBRESOURCE* GetD3DPtr();
	};

	struct BufferDesc
	{
		uint byteWidth;
		ResourceUsages usage;
		ResourceBindFlags bindFlags;
		ResourceAccessFlags cpuAccessFlags;
		uint miscFlags;
		uint structuredStride;

		D3D11_BUFFER_DESC* GetD3DPtr();
	};

	struct SampleDesc
	{
		uint count;
		uint quality;

		DXGI_SAMPLE_DESC* GetD3DPtr();
	};

	struct Texture2DDesc
	{
		uivec2 size;
		uint mipLevels;
		uint arraySize;
		Formats format;
		SampleDesc sampleDesc;
		ResourceUsages usage;
		ResourceBindFlags bindFlags;
		ResourceAccessFlags cpuAccessFlags;
		uint miscFlags;

		D3D11_TEXTURE2D_DESC* GetD3DPtr();
	};
}