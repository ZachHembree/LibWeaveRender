#pragma once
#include "ReplicaWinUtils.hpp"
#include <d3d11.h>
#include "Context.hpp"

namespace Replica::D3D11
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
		ID3D11Device& Get();

		/// <summary>
		/// Returns reference to COM device interface
		/// </summary>
		ID3D11Device* operator->();

		/// <summary>
		/// Returns pointer to COM device context interface
		/// </summary>
		Context& GetContext();

		void CreateShader(const IDynamicArray<byte>& binSrc, ComPtr<ID3D11VertexShader>& pVS);

		void CreateShader(const IDynamicArray<byte>& binSrc, ComPtr<ID3D11PixelShader>& pPS);

		void CreateShader(const IDynamicArray<byte>& binSrc, ComPtr<ID3D11ComputeShader>& pCS);

	private:
		Renderer* pRenderer;
		ComPtr<ID3D11Device> pDev;
		Context context;
	};
}