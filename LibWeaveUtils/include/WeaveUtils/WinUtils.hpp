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

#include "WeaveUtils/GlobalUtils.hpp"
#include "WeaveUtils/Math.hpp"

#ifndef _WINDOWS_
#ifndef CALLBACK
#define CALLBACK    __stdcall
#endif
#ifndef WINAPI
#define WINAPI      __stdcall
#endif

struct HWND__;
typedef HWND__* HWND;

struct HMONITOR__;
typedef HMONITOR__* HMONITOR;

struct tagMSG;
typedef tagMSG MSG;

struct HINSTANCE__;
typedef HINSTANCE__* HINSTANCE;

struct tagTRACKMOUSEEVENT;
typedef tagTRACKMOUSEEVENT TRACKMOUSEEVENT;
#endif