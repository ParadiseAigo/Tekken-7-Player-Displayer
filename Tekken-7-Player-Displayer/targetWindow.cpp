#include "player-displayer.h"
#include "pointers.h"

HWND getWindowHandle(const wchar_t* programWindowName) {
    HWND windowHandle;
    DWORD errorCode;
    windowHandle = FindWindow(NULL, programWindowName);
    if (windowHandle == NULL) {
        errorCode = GetLastError();
        //        print(std::string("Error! FindWindow in getWindowHandle failed! Code: ").append(std::to_string(errorCode)).append(std::string("\r\n")));
    }
    return windowHandle;
}

void setScreenMode(DWORD newScreenMode) {
    if (tekkenWindowHandle != NULL) {
        DWORD screenMode;
        std::string screenModeInEnglish;
        switch (newScreenMode) {
        case SCREEN_MODE_FULLSCREEN:
            screenModeInEnglish = "fullscreen";
            break;
        case SCREEN_MODE_BORDERLESS:
            screenModeInEnglish = "borderless";
            break;
        case SCREEN_MODE_WINDOWED:
            screenModeInEnglish = "windowed";
            break;
        default:
            screenModeInEnglish = "unknown";
            break;
        }
        screenMode = readDwordFromMemory(tekkenHandle, screenModePointer);
        if (screenMode != newScreenMode) {
            writeDwordToMemory(tekkenHandle, screenModePointer, newScreenMode);
            minimizeAndRestoreTekkenWindow();
            myGuiTerminalPrint(std::string("Screen mode set to ").append(screenModeInEnglish).append(std::string("\r\n")));
        }
    }
}

void minimizeAndRestoreTekkenWindow() {
    showWindow(tekkenWindowHandle, SW_MINIMIZE);
    showWindow(tekkenWindowHandle, SW_RESTORE);
}
