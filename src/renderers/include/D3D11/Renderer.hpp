#pragma once
#include "WindowComponentBase.hpp"
#include "GfxException.hpp"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>
#include <wrl.h>
#include "D3D11/VertexBuffer.hpp"
#include "D3D11/IndexBuffer.hpp"
#include "D3D11/Device.hpp"
#include "D3D11/SwapChain.hpp"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "Dxgi.lib")

namespace Replica::D3D11
{ 
	class Renderer : public WindowComponentBase
	{
	public:
		Renderer(MinWindow* window);

		void Update() override;

	private:
		Device device;
		SwapChain swap;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pBackBufView;
	};
}
