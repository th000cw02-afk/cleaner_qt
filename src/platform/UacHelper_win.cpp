#include "UacHelper_win.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <shellapi.h>
#endif

bool UacHelper::restartElevated(const wchar_t* arguments)
{
#ifdef _WIN32
    wchar_t exePath[MAX_PATH]{};
    if (GetModuleFileNameW(nullptr, exePath, MAX_PATH) == 0) {
        return false;
    }
    const HINSTANCE result = ShellExecuteW(nullptr, L"runas", exePath, arguments, nullptr, SW_SHOWNORMAL);
    return reinterpret_cast<intptr_t>(result) > 32;
#else
    (void)arguments;
    return false;
#endif
}
