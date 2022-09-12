#pragma once
#include "../src/utils/include/WindowComponentBase.hpp"

namespace Replica::D3D11
{
	class ImguiRenderComponent;

	/// <summary>
	/// Manages initialization and cleanup of Imgui
	/// </summary>
	class ImguiHandler : public WindowComponentBase
	{
	public:
		static bool enableDemoWindow;

		ImguiHandler();

		ImguiHandler(MinWindow& window, Renderer& renderer);

		~ImguiHandler();

		void UpdateUI();

		bool OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

	private:
		ImguiRenderComponent* pRenderComponent;
	};
}