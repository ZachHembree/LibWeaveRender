#pragma once
#include "WeaveUtils/WindowComponentBase.hpp"
#include "WeaveUtils/TextUtils.hpp"
#include "WeaveUtils/ObjectPool.hpp"

namespace Weave::D3D11
{
	class ImGuiRenderComponent;
	class Renderer;
	
	/// <summary>
	/// ImGui singleton wrapper for default Win32 and D3D11 backends
	/// </summary>
	class ImGuiHandler : public WindowComponentBase
	{
	public:
		/// <summary>
		/// Enables or disables the built-in ImGui demo UI
		/// </summary>
		static bool enableDemoWindow;

		/// <summary>
		/// Returns true if the handler has been initialized
		/// </summary>
		static bool GetIsInitialized();

		/// <summary>
		/// Initializes ImGui to use the given D3D11 rendering backend
		/// </summary>
		static void Init(Renderer& renderer);

		/// <summary>
		/// Resets backend to initial state
		/// </summary>
		static void Reset();

		/// <summary>
		/// Creates a temporary formatted string from an internal pool that will be reclaimed 
		/// after the end of the next frame.
		/// </summary>
		template<typename... FmtArgs>
		static string_view GetTempFormatStr(string_view fmt, FmtArgs&&... args)
		{
			WV_ASSERT(s_pInstance != nullptr);
			string& buf = s_pInstance->GetTmpString();
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
			WV_ASSERT(s_pInstance != nullptr);
			string& buf = s_pInstance->GetTmpString();
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

		void Update() override;

		bool OnWndMessage(HWND hWnd, uint msg, ulong wParam, slong lParam) override;

	private:
		friend ManagerT;
		static ImGuiHandler* s_pInstance;

		Renderer* pRenderer;
		ImGuiRenderComponent* pRenderComponent;
		bool isInitialized;

		ObjectPool<string> stringPool;
		UniqueVector<string> activeText;

		ImGuiHandler(MinWindow& parent, Renderer& renderer);

		~ImGuiHandler();

		string& GetTmpString();
	};
}