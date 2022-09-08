#pragma once
#include "RepLeanWin.h"

const constexpr wchar_t* g_Name = L"Replica";

// Has title bar | Has minimize button | Has window menu on its title bar | Can maximize | Can resize
const long g_DefaultWndStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_MAXIMIZEBOX | WS_SIZEBOX;
