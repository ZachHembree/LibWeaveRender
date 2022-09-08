#pragma once
#include "D3DUtils.hpp"
#include <d3d11.h>
#include "DynamicCollections.hpp"
#include "D3D11/Context.hpp"

namespace Replica::D3D11
{	
	class Device : protected MoveOnlyObjBase
	{
	public:
		Device();

		Device(Device&&) = default;

		Device& operator=(Device&&) = default;

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
		ComPtr<ID3D11Device> pDev;
		Context context;
	};
}