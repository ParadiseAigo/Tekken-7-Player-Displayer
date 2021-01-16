#include "player-displayer.h"
#include "pointers.h"

void handleInput() {
	DWORD currentPid = GetCurrentProcessId();
	int KEY_W = 0x57;
	int KEY_F = 0x46;
	int KEY_O = 0x4F;
	int delay = 1500;
	AttachThreadInput(currentPid, tekkenPid, true); //attach
	if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(KEY_W) ) {  //if CTRL+W pressed
		setScreenMode(SCREEN_MODE_WINDOWED);
		Sleep(delay); //avoid unintended multiple presses
	}
	if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(KEY_F) ) {  //if CTRL+F pressed
		setScreenMode(SCREEN_MODE_FULLSCREEN);
		Sleep(delay); //avoid unintended multiple presses
	}
	if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(KEY_O)) {  //if CTRL+O pressed
		setScreenMode(SCREEN_MODE_WINDOWED);
		openPlayerlist();
		Sleep(delay); //avoid unintended multiple presses
	}
	AttachThreadInput(currentPid, tekkenPid, false); //detach
}
