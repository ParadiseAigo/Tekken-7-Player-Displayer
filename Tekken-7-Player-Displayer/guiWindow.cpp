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

bool isWindowOffScreen(int windowX, int windowY, int windowWidth, int windowHeight) {
    RECT rect;
    rect.left = windowX;
    rect.top = windowY;
    rect.right = windowX + windowWidth;
    rect.bottom = windowY + windowHeight;
    HMONITOR monitorHandle = MonitorFromRect(&rect, MONITOR_DEFAULTTONULL); // returns NULL if the rectangle does not intersect any display monitor
    return monitorHandle == NULL;
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

void drawPictureOnWindow(LPPICTURE picture, HWND hWnd) {
    if (picture == NULL) {
        return;
    }
    // get picture size
    long pictureWidth = 0;
    long pictureHeight = 0;
    picture->get_Width(&pictureWidth);
    picture->get_Height(&pictureHeight);

    // get window size
    RECT rc;
    GetClientRect(hWnd, &rc);
    int rectWidth = rc.right;
    int rectHeight = rc.bottom;

    // draw picture on window
    HDC hdc = GetDC(hWnd);
    picture->Render(hdc, 0, 0, rectWidth, rectHeight, 0, pictureHeight, pictureWidth, -pictureHeight, &rc);
    ReleaseDC(hWnd, hdc);
}