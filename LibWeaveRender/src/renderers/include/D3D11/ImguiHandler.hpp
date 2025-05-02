#pragma once
#include "WeaveUtils/WindowComponentBase.hpp"

namespace Weave::D3D11
{
	class ImguiRenderComponent;
	class Renderer;
	
	/// <summary>
	/// Manages initialization and cleanup of Imgui
	/// </summary>
	class ImguiHandler : public WindowComponentBase
	{
	public:
		static bool enableDemoWindow;

		ImguiHandler(MinWindow& window, Renderer& renderer);

		~ImguiHandler();

		void UpdateUI();

		bool OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

	private:
		Renderer* pRenderer;
		ImguiRenderComponent* pRenderComponent;
	};
}