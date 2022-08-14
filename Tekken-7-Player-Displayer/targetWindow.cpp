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

std::string ipAddressToString(u_long ip) {  
    // the ip address is a hex number with 8 digits: 2 digits for each number in the ip address
    u_long ip4thNumber = ip % 0x100;
    u_long ip3rdNumber = (ip / 0x100) % 0x100;
    u_long ip2ndNumber = (ip / 0x10000) % 0x100;
    u_long ip1stNumber = (ip / 0x1000000) % 0x100;
    return std::to_string(ip1stNumber).append(".")
        .append(std::to_string(ip2ndNumber)).append(".")
        .append(std::to_string(ip3rdNumber)).append(".")
        .append(std::to_string(ip4thNumber));
}
