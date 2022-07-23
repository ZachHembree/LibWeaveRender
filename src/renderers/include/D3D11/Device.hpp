#pragma once
#include "GfxException.hpp"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>
#include <wrl.h>

namespace Replica::D3D11
{
	class Device
	{
	public:
		ID3D11Device* Get() { return pDev.Get(); }
		ID3D11DeviceContext* GetContext() { return pCon.Get(); }

		ID3D11Device** GetAddressOf() { return pDev.GetAddressOf(); }
		ID3D11DeviceContext** GetAddressOfCon() { return pCon.GetAddressOf(); }

		Device()
		{
			GFX_THROW_FAILED(D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_HARDWARE,
				nullptr, 
				0,
				nullptr,
				0,
				D3D11_SDK_VERSION,
				&pDev,
				nullptr,
				&pCon
			));
		}

		/// <summary>
		/// Creates an RT view for accessing resource data
		/// </summary>
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetRtView(const Microsoft::WRL::ComPtr<ID3D11Resource>& buffer)
		{
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRtView;
			GFX_THROW_FAILED(pDev->CreateRenderTargetView(buffer.Get(), nullptr, &pRtView));

			return pRtView;
		}

	private:
		Microsoft::WRL::ComPtr<ID3D11Device> pDev;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pCon;

	};
}