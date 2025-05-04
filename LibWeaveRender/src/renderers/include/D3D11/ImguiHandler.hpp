#pragma once
#include "WeaveUtils/WindowComponentBase.hpp"
#include "WeaveUtils/TextUtils.hpp"
#include "WeaveUtils/ObjectPool.hpp"

namespace Weave::D3D11
{
	class ImguiRenderComponent;
	class Renderer;
	
	/// <summary>
	/// Manages initialization and cleanup of Imgui
	/// </summary>
	class ImGuiHandler : public WindowComponentBase
	{
	public:
		static bool enableDemoWindow;

		static void Init(Renderer& renderer);

		static void Reset();

		static ImGuiHandler& GetInstance();

		/// <summary>
		/// Creates a temporary formatted string from an internal pool that will be reclaimed 
		/// after the end of the next frame.
		/// </summary>
		template<typename... FmtArgs>
		static string_view GetTempFormatStr(string_view fmt, FmtArgs&&... args)
		{
			string& buf = GetInstance().GetTempString();
			VFormatTo(buf, fmt, std::forward<FmtArgs>(args)...);

			if (buf.empty() || buf.back() != '\0')
				buf.push_back('\0');

			return buf;
		}

		/// <summary>
		/// Creates a temporary formatted string from an internal pool that will be reclaimed 
		/// after the end of the next frame.
		/// </summary>
		template<typename... FmtArgs>
		static char* GetTempFormatCStr(string_view fmt, FmtArgs&&... args)
		{
			string& buf = GetInstance().GetTempString();
			VFormatTo(buf, fmt, std::forward<FmtArgs>(args)...);

			if (buf.empty() || buf.back() != '\0')
				buf.push_back('\0');
				
			return buf.data();
		}

		/// <summary>
		/// Creates a temporary string containing a copy of a UTF16LE string that will be reclaimed 
		/// after the end of the next frame.
		/// </summary>
		static string_view GetTmpNarrowStr(wstring_view str);

		/// <summary>
		/// Creates a temporary string containing a copy of a UTF16LE string that will be reclaimed 
		/// after the end of the next frame.
		/// </summary>
		static char* GetTmpNarrowCStr(wstring_view str);

		void UpdateUI();

		bool OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

		~ImGuiHandler();

	private:
		Renderer* pRenderer;
		ImguiRenderComponent* pRenderComponent;

		ImGuiHandler(Renderer& renderer);

		string& GetTempString();
	};
}