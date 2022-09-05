#pragma once
#include "WindowComponentBase.hpp"

namespace Replica::D3D11
{
	class ImguiRenderComponent;

	/// <summary>
	/// Manages initialization and cleanup of Imgui
	/// </summary>
	class ImguiHandler : public WindowComponentBase
	{
	public:
		ImguiHandler();

		ImguiHandler(MinWindow& window, Renderer& renderer);

		~ImguiHandler();

		void OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

	private:
		ImguiRenderComponent* pRenderComponent;
	};
}