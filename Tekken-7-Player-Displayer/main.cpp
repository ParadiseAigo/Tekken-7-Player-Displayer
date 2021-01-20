#include "player-displayer.h"
#include "pointers.h"

//TODO:
//-> /

HANDLE tekkenHandle;
HWND tekkenWindowHandle;
int tekkenPid;
QWORD lastFoundSteamId;
char* lastFoughtOpponentName;

void* fightThisPlayerMessagePointer;
void* secondsRemainingMessagePointer;
void* opponentFoundMessagePointer;
void* opponentNamePointer;
void* screenModePointer;
void* steamIdPointer;

int SCREEN_WIDTH;
int SCREEN_HEIGHT;

Windows windows;
Fonts fonts;

HBRUSH solidBrush;
WNDPROC defaultEditProc;

Hotkey* hotkeys[NR_OF_HOTKEYS];
HANDLE threadHandles[NR_OF_THREADS];
bool continueThreads = true;

int main() {
    createThreads();
    waitForThreadsToTerminate();
    closeThreads();
}

void createThreads() {
    threadHandles[0] = beginThread(&readAndUpdateTekkenMemory);
    threadHandles[1] = beginThread(&createWindowAndHandleInput);
}

HANDLE beginThread(_beginthreadex_proc_type proc) {
    unsigned threadId = 0;
    return (HANDLE)_beginthreadex(NULL, 0, proc, NULL, 0, &threadId);
}

void endThread() {
    _endthreadex(0);
}

void waitForThreadsToTerminate() {
    WaitForMultipleObjects(NR_OF_THREADS, threadHandles, TRUE, INFINITE);
}

void closeThreads() {
    for (int i = 0; i < NR_OF_THREADS; i++) {
        CloseHandle(threadHandles[i]);
    }
}