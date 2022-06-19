#pragma once
#include "WindowComponentBase.hpp"
#include "GfxException.hpp"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

class RendererD3D11 : public WindowComponentBase
{
public:
	RendererD3D11(MinWindow* window);

	void Update() override;

private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pBackBufView;
};
