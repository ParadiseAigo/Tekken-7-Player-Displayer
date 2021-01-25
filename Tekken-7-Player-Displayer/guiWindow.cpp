#include "player-displayer.h"

HWND createWindow(DWORD extendedStyle, LPCWSTR className, LPCWSTR windowName, DWORD style,
    int x, int y, int width, int height, HWND parentWindowHandle) {

    return CreateWindowEx(extendedStyle, className, windowName, style, x, y, width, height,
        parentWindowHandle, NULL, NULL, NULL);
}

void registerWindowClass(WNDCLASS windowClass) {
    if (!RegisterClass(&windowClass)) {
        myGuiTerminalPrint(std::string("Error registering window class in registerWindowClass, error code = ").append(std::to_string(GetLastError())).append(std::string("\r\n")));
    }
}

void sendMessage(HWND windowHandle, UINT msg, WPARAM wparam, LPARAM lparam) {
    SendMessage(windowHandle, msg, wparam, lparam);
}

void showWindow(HWND windowHandle, int showCommand) {
    ShowWindow(windowHandle, showCommand);
}

bool isWindow(HWND windowHandle) {
    return IsWindow(windowHandle);
}

BOOL isWindowVisible(HWND windowHandle) {
    return IsWindowVisible(windowHandle);
}

void setForegroundWindow(HWND windowHandle) {
    SetForegroundWindow(windowHandle);
}

void setFocus(HWND windowHandle) {
    SetFocus(windowHandle);
}

void destroyWindow(HWND windowHandle) {
    DestroyWindow(windowHandle); // posts a WM_DESTROY message
}
