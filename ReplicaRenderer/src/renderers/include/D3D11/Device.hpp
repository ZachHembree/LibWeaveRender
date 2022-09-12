#pragma once
#include "../D3DUtils.hpp"
#include <d3d11.h>
#include "Context.hpp"

namespace Replica::D3D11
{	
	class Renderer;

	class Device : protected MoveOnlyObjBase
	{
	public:
		Device(Renderer& renderer);

		Device(Device&&) = default;

		Device& operator=(Device&&) = default;

		/// <summary>
		/// Returns reference to renderer using this device
		/// </summary>
		Renderer& GetRenderer();

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

	private:
		Renderer* pRenderer;
		ComPtr<ID3D11Device> pDev;
		Context context;
	};
}