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

void initWindowsSocketsAPI() {
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;
    long errorCode = WSAStartup(wVersionRequested, &wsaData);
    if (errorCode != 0) {
        myGuiTerminalPrint(std::string("Error finding a usable WinSock DLL, error code = ").append(std::to_string(GetLastError())).append(std::string("\r\n")));
    }
}

std::string ipAddressToString(u_long ip) {  
    struct sockaddr_in ipAddress = {};
    ipAddress.sin_family = AF_INET;
    u_long ipReversed = htonl(ip);
    memcpy(&ipAddress.sin_addr, &ipReversed, sizeof(struct in_addr));

    const int maxBytesIpAddress = 16;
    wchar_t ipString[maxBytesIpAddress];
    long errorCode = WSAAddressToString((struct sockaddr*)&ipAddress, sizeof(struct sockaddr_in), 0, ipString, (LPDWORD)&maxBytesIpAddress);
    if (errorCode != 0) {
        myGuiTerminalPrint(std::string("Error in ipAddressToString, error code = ").append(std::to_string(WSAGetLastError())).append(std::string("\r\n")));
    }
    return wcharPtrToString(ipString);
}
