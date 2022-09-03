#pragma once
#include "D3D11/D3DUtils.hpp"
#include <d3d11.h>
#include "DynamicCollections.hpp"
#include "D3D11/Context.hpp"

namespace Replica::D3D11
{	
	class Device : protected MoveOnlyObjBase
	{
	public:
		Device();

		/// <summary>
		/// Returns pointer to COM device interface
		/// </summary>
		ID3D11Device* Get();

		/// <summary>
		/// Returns pointer to COM device context interface
		/// </summary>
		Context& GetContext();

		/// <summary>
		/// Creates an RT view for accessing resource data
		/// </summary>
		ComPtr<ID3D11RenderTargetView> GetRtView(const ComPtr<ID3D11Resource>& buffer);

	private:
		ComPtr<ID3D11Device> pDev;
		Context context;
	};
}