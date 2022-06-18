#pragma once
#include "WindowComponentBase.hpp"
#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")

class RendererD3D11 : public WindowComponentBase
{
public:
	RendererD3D11(MinWindow* window);

	~RendererD3D11();

	void Update() override;

private:
	ID3D11Device* d3dDevice;
	ID3D11DeviceContext* devContext;
	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* backBufRT;

};
