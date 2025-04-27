#pragma once
#include "WeaveUtils/WindowComponentBase.hpp"
#include "../Input.hpp"

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

		InputHandler& GetInput();

		void UpdateUI();

		bool OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

	private:
		InputHandler input;
		ImguiRenderComponent* pRenderComponent;
	};
}