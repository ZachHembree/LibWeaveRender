#pragma once
#include "WeaveUtils/WinUtils.hpp"
#include "InternalD3D11.hpp"
#include "Context.hpp"

namespace Weave::D3D11
{	
	class Renderer;

	class Device
	{
	public:
		MAKE_MOVE_ONLY(Device)

		Device(Renderer& renderer);

		/// <summary>
		/// Returns reference to renderer using this device
		/// </summary>
		Renderer& GetRenderer() const;

		/// <summary>
		/// Returns pointer to COM device interface
		/// </summary>
		ID3D11Device1* Get();

		/// <summary>
		/// Returns reference to COM device interface
		/// </summary>
		ID3D11Device1* operator->();

		/// <summary>
		/// Returns raw D3D immediate context pointer. Used internally for external library 
		/// integrations.
		/// </summary>
		ID3D11DeviceContext1* GetImmediateContext();

		/// <summary>
		/// Returns reference to immediate context
		/// </summary>
		Context& GetContext();

		void CreateShader(const IDynamicArray<byte>& binSrc, ComPtr<ID3D11VertexShader>& pVS);

		void CreateShader(const IDynamicArray<byte>& binSrc, ComPtr<ID3D11PixelShader>& pPS);

		void CreateShader(const IDynamicArray<byte>& binSrc, ComPtr<ID3D11ComputeShader>& pCS);

	private:
		Renderer* pRenderer;
		ComPtr<ID3D11Device1> pDev;
		ComPtr<ID3D11DeviceContext1> pCtxImm;
		Context context;
	};
}