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

HANDLE threadHandles[NR_OF_THREADS];
bool continueThreads = true;

int main() {
	createThreads();
	waitForThreadsToTerminate();
	closeThreads();
}

void createThreads() {
	threadHandles[0] = beginThread(&mainThread);  
	threadHandles[1] = beginThread(&guiThread);
}

HANDLE beginThread(_beginthreadex_proc_type proc) {
	unsigned* threadId = new unsigned int;
	*threadId = 0;
	return (HANDLE)_beginthreadex(NULL, 0, proc, NULL, 0, threadId);
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

unsigned __stdcall mainThread(void* arguments) {
	loadTargetProcess();
	editTargetProcessLoop();
	endThread();
	return ERROR_SUCCESS;
}

void loadTargetProcess() {
	initTekkenHandle();
	initTekkenWindowHandle();
	initPointers();
	cleanAllProcessMessages();
	myGuiTerminalPrint(std::string("Program ready!\r\n"));
}

void initTekkenHandle() {
	DWORD pid, secondsDelay;
	pid = getProcessId(TEXT(TEKKEN_EXE_NAME));
	secondsDelay = 2;
	while (continueThreads && (pid == 0)) {
		myGuiTerminalPrint(std::string("Tekken not found. Trying again in ")
			.append(std::to_string(secondsDelay))
			.append(std::string(" seconds...\r\n"))
		);
		Sleep(secondsDelay * 1000); // milliseconds
		pid = getProcessId(TEXT(TEKKEN_EXE_NAME));
	}
	tekkenPid = pid; // global variable
	myGuiTerminalPrint(std::string("Tekken found! pid = (").append(std::to_string(pid)).append(")\r\n"));
	myGuiTerminalPrint(std::string("Opening Tekken process...\r\n"));
	tekkenHandle = getProcessHandle(pid);  // global variable
	myGuiTerminalPrint(std::string("Opening Tekken success!\r\n"));
}

void initTekkenWindowHandle() {
	DWORD secondsDelay;
	secondsDelay = 2;
	while (continueThreads && NULL == ((tekkenWindowHandle = getWindowHandle(TEXT(TEKKEN_WINDOW_NAME))))) { // global variable
		myGuiTerminalPrint(std::string("Searching for Tekken window. Trying again in ")
			.append(std::to_string(secondsDelay))
			.append(std::string(" seconds...\r\n"))
		);
		Sleep(secondsDelay * 1000); // milliseconds
	}
	myGuiTerminalPrint(std::string("Tekken window found!\r\n"));
}

void initPointers() {
	DWORD secondsDelay;
	secondsDelay = 2;
	while (continueThreads && !isGameLoaded()) {
		myGuiTerminalPrint(std::string("Game not loaded yet. Waiting ")
			.append(std::to_string(secondsDelay))
			.append(std::string(" seconds...\r\n"))
		);
		Sleep(secondsDelay * 1000); // milliseconds
	}
	opponentNamePointer = (void*)getDynamicPointer(tekkenHandle, (void*)OPPONENT_NAME_STATIC_POINTER, OPPONENT_NAME_POINTER_OFFSETS);
	fightThisPlayerMessagePointer = (void*)getDynamicPointer(tekkenHandle, (void*)FIGHT_THIS_PLAYER_MESSAGE_STATIC_POINTER, FIGHT_THIS_PLAYER_MESSAGE_POINTER_OFFSETS);
	secondsRemainingMessagePointer = (void*)getDynamicPointer(tekkenHandle, (void*)SECONDS_REMAINING_MESSAGE_STATIC_POINTER, SECONDS_REMAINING_MESSAGE_POINTER_OFFSETS);
	opponentFoundMessagePointer = (void*)getDynamicPointer(tekkenHandle, (void*)OPPONENT_FOUND_MESSAGE_STATIC_POINTER, OPPONENT_FOUND_MESSAGE_POINTER_OFFSETS);
	screenModePointer = (void*)getDynamicPointer(tekkenHandle, (void*)SCREEN_MODE_STATIC_POINTER, SCREEN_MODE_POINTER_OFFSETS);
	steamIdPointer = (void*)getDynamicPointer(tekkenHandle, (void*)STEAM_ID_STATIC_POINTER, STEAM_ID_POINTER_OFFSETS);
	char* playerName = readStringFromMemory(tekkenHandle, opponentNamePointer);
	myGuiTerminalPrint(std::string("Pointers loaded.\r\nPlayer name (yours): ")
		.append(std::string(playerName)).append(std::string("\r\n"))
		.append(std::string("WARNING! If the above name is not your name, "))
		.append(std::string("restart the program and when you do, make sure you are not "))
		.append(std::string("in a match or you havent just declined an opponent "))
		.append(std::string("or are in the process of accepting a match.\r\n"))
	);
	setPlayerNameInGui(playerName);
	free(playerName);
}

void editTargetProcessLoop() {
	char* playerName;
	char* currentOpponentName;
	char* currentLoadedOpponentName;
	enum tekkenState currentState;
	int delayInSearch = 1000/60; // "60fps"
	int delayInFight = 2000; // 2 seconds
	playerName = readStringFromMemory(tekkenHandle, opponentNamePointer);
	currentOpponentName = readStringFromMemory(tekkenHandle, opponentNamePointer);
	currentLoadedOpponentName = (char*) malloc(10 * sizeof(char)); // dummy value
	currentLoadedOpponentName[0] = '\0'; // empty string
	currentState = IN_SEARCH;
	while (continueThreads) {
		if (currentState == IN_SEARCH) {
			Sleep(delayInSearch);
		}
		else if (currentState == IN_FIGHT) {
			Sleep(delayInFight);
		}
		if (isTimeToCleanMessages(playerName, currentOpponentName)) {
			cleanAllProcessMessages();
			cleanAllGuiMessages();
			currentOpponentName[0] = '\0';
			currentState = IN_SEARCH;
			myGuiTerminalPrint(std::string("Ready to find the next opponent.\r\n"));
		}
		if (isNewOpponentReceived(playerName, currentOpponentName)) {
			if (isOpponentSteamIdValid()) {
				currentOpponentName = handleNewOpponent(currentOpponentName);
			}
			else {  // if opponent steam id is "late",
					// update in-game messages anyway
					// (using opponents name instead of steam id)
					// or it might not ever update in-game
				updateMessagesWithoutSteamId();
			}
		}
		if (isNewFightAccepted(playerName, currentOpponentName, currentLoadedOpponentName)) {
			currentLoadedOpponentName = saveNewOpponentInPlayerlist(playerName, currentOpponentName, currentLoadedOpponentName);
			lastFoughtOpponentName = currentLoadedOpponentName;
			currentState = IN_FIGHT;
		}
		if (!isWindow(tekkenWindowHandle)) {
			Sleep(3000); // wait to make sure the tekken process has closed after the window was closed
			loadTargetProcess();
		}
	}
}

void closeProgram() {
	CloseHandle(tekkenHandle);
	continueThreads = false;
}
