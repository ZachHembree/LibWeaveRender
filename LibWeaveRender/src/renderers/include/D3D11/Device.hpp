#pragma once
#include "CtxImm.hpp"
#include "Resources/DisplayOutput.hpp"

struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11ComputeShader;

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
		CtxImm& GetContext();

		/// <summary>
		/// Returns the name of the active graphics adapter
		/// </summary>
		string_view GetAdapterName() const;

		/// <summary>
		/// Returns an array of active displays
		/// </summary>
		IDynamicArray<DisplayOutput>& GetDisplays();

		/// <summary>
		/// Returns an array of active displays
		/// </summary>
		const IDynamicArray<DisplayOutput>& GetDisplays() const;

		/// <summary>
		/// Creates and compiles a new vertex shader
		/// </summary>
		void CreateShader(const IDynamicArray<byte>& binSrc, ComPtr<ID3D11VertexShader>& pVS);

		/// <summary>
		/// Creates and compiles a new pixel shader
		/// </summary>
		void CreateShader(const IDynamicArray<byte>& binSrc, ComPtr<ID3D11PixelShader>& pPS);

		/// <summary>
		/// Creates and compiles a new compute shader
		/// </summary>
		void CreateShader(const IDynamicArray<byte>& binSrc, ComPtr<ID3D11ComputeShader>& pCS);

	private:
		Renderer* pRenderer;
		ComPtr<ID3D11Device1> pDev;
		ComPtr<ID3D11DeviceContext1> pCtxImm;
		CtxImm context;
		string name;
		UniqueVector<DisplayOutput> displays;
	};
}