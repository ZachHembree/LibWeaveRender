#pragma once

// Essential defines
#define WIN32_LEAN_AND_MEAN      // Exclude rarely-used stuff from Windows headers
#define NOMINMAX                 // Exclude min/max macros

#define NOGDICAPMASKS
#define NORASTEROPS  
#define NOCOLOR      
#define NODRAWTEXT   
#define NOTEXTMETRIC 
#define NOMETAFILE   
#define NOBITMAP
// Exclude User32 components
#define NOSYSMETRICS             // System metrics
#define NOMENUS                  // Menus
#define NOICONS                  // Icons
#define NOSYSCOMMANDS            // System commands (minimize, maximize, close)
#define NOCLIPBOARD              // Clipboard APIs
#define NOCTLMGR                 // Control manager routines
#define NOSCROLL                 // Scroll bar routines
#define NOWH                     // Window Hooks
#define NODEFERWINDOWPOS         // DeferWindowPos routines
// Exclude other less common APIs
#define NOATOM                   // Atom manipulation routines
#define NOMEMMGR                 // Global/Local memory routines
#define NOOPENFILE               // OpenFile routine
#define NOSERVICE                // Service Controller routines
#define NOSOUND                  // Sound APIs
#define NOCOMM                   // Communication APIs
#define NOKANJI                  // Kanji support stuff
#define NOHELP                   // Help engine interfaces
#define NOPROFILER               // Profiler APIs
#define NOMCX                    // Modem Configuration Extensions
#define NORPC                    // RPC stuff (often covered by LEAN_AND_MEAN)
#define NOPROXYSTUB              // RPC stuff (often covered by LEAN_AND_MEAN)
#define NOIMAGE                  // LoadImage routine
#define NOTAPE                   // Tape APIs
#define NOKERNEL

#include <wrl/client.h>
#include "WeaveUtils/Math.hpp"
#include "WeaveUtils/Utils.hpp"

namespace Weave
{
	typedef glm::tvec2<DWORD> WndStyle;
	typedef glm::tvec2<uint> uivec2;

	using glm::ivec2;
	using glm::vec2;

	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
}