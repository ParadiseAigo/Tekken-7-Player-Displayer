#include "player-displayer.h"
#include "pointers.h"

HWND getWindowHandle(const wchar_t* programWindowName) {  //change the argument type to  (const wchar_t*)
	HWND windowHandle;
	DWORD errorCode;
	windowHandle = FindWindow(NULL, programWindowName);
	if (windowHandle == NULL) {
		errorCode = GetLastError();
		//printf("Error! FindWindow in getWindowHandle failed! Code: %d.\n", errorCode);
	}
	return windowHandle;
}

void setScreenMode(DWORD newScreenMode) {
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
		ShowWindow(tekkenWindowHandle, SW_MINIMIZE);
		ShowWindow(tekkenWindowHandle, SW_RESTORE);
		std::cout << "Screen mode set to = " << screenModeInEnglish << std::endl;
	}
}
