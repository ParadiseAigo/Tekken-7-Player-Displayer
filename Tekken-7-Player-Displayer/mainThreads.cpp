#include "player-displayer.h"
#include "pointers.h"

//TODO:
//-> /

HANDLE tekkenHandle;
HWND tekkenWindowHandle;
int tekkenPid;
QWORD lastFoundSteamId;
bool isSteamIdFound; // helps keep track of  lastFoundSteamId
QWORD userSteamId;
char* lastNameInPlayerlist;
bool silentMode;
bool isTekkenLoaded;

void* fightThisPlayerMessagePointer;
void* secondsRemainingMessagePointer;
void* opponentFoundMessagePointer;
void* opponentNamePointer;
void* screenModePointer;
void* steamModulePointer;

ProgramThreads programThreads;

int main() {
	createThreads();
	waitForThreadsToTerminate();
	closeThreads();
}

void createThreads() {
	programThreads.mainThread = beginThread(&mainThread);  
	programThreads.guiThread = beginThread(&guiThread);
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
	WaitForSingleObject(programThreads.guiThread, INFINITE);
}

void closeThreads() {
	CloseHandle(programThreads.guiThread);
	DWORD delayBeforeClosingMainThread = 200;
	Sleep(delayBeforeClosingMainThread);
	CloseHandle(programThreads.mainThread);
}

unsigned __stdcall mainThread(void* arguments) {
	initPlayerlist();
	initVariables();
	loadTargetProcess();
	editTargetProcessLoop();
	endThread();
	return ERROR_SUCCESS;
}

void initPlayerlist() {
	myGuiTerminalPrint(std::string("Checking your list  \"Tekken Player List.txt\"  ...\r\n"));
	if (!doesFileExist((char*)PLAYERLIST_PATH)) {
		myGuiTerminalPrint(std::string("\"Tekken Player List.txt\" not found.\r\n"));
		myGuiTerminalPrint(std::string("Attempting to create a new one.\r\n"));
		createExamplePlayerlist();
	}
	else {
		myGuiTerminalPrint(std::string("Good  \"Tekken Player List.txt\"  is OK.\r\n"));
	}
}

void initVariables() {
	silentMode = true; // global variable
	isTekkenLoaded = false;  // global variable
}

void loadTargetProcess() {
	initTekkenHandle();
	initTekkenWindowHandle();
	initPointers();
	cleanAllProcessMessages();
	isTekkenLoaded = true;
	myGuiTerminalPrint(std::string("Program ready!\r\n"));
}

void initTekkenHandle() {
	DWORD pid, secondsDelay;
	pid = getProcessId(TEXT(TEKKEN_EXE_NAME));
	secondsDelay = 2;
	while (pid == 0) {
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
	while (NULL == ((tekkenWindowHandle = getWindowHandle(TEXT(TEKKEN_WINDOW_NAME))))) { // global variable
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
	while (!isGameLoaded()) {
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
	initModuleAdresses();
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

void initModuleAdresses() {
	steamModulePointer = (void*)getModuleBaseAddress(tekkenPid, STEAM_API_MODULE_EDITED_NAME);
	if (steamModulePointer == 0) {
		steamModulePointer = (void*)getModuleBaseAddress(tekkenPid, STEAM_API_MODULE_NAME);
	}
	void* userSteamIdPointer = (void*)getDynamicPointer(tekkenHandle, (void*) ((QWORD)steamModulePointer + STEAM_ID_USER_STATIC_POINTER), STEAM_ID_USER_POINTER_OFFSETS);
	userSteamId = readQwordFromMemory(tekkenHandle, userSteamIdPointer);
}

void editTargetProcessLoop() {
	char* playerName;
	//char* lastReceivedName;  // name obtained directly from web, this variable is not need anymore
	char* currentOpponentName;
	char* currentLoadedOpponentName;
	bool areMessagesClean;
	int delayInSearch = 1000/60; // "60fps"
	int delayInFight = 2000; // 2 seconds
	playerName = readStringFromMemory(tekkenHandle, opponentNamePointer);
	//lastReceivedName = readStringFromMemory(tekkenHandle, opponentNamePointer);
	currentOpponentName = readStringFromMemory(tekkenHandle, opponentNamePointer);
	currentLoadedOpponentName = (char*) malloc(10 * sizeof(char)); // dummy value
	currentLoadedOpponentName[0] = '\0'; // empty string
	lastFoundSteamId = -1;  // global variable
	isSteamIdFound = false; // global variable
	areMessagesClean = true;
	lastNameInPlayerlist = getLastNameInPlayerlist((char*)PLAYERLIST_PATH); //global variable  // set equal to NULL if player list is empty
	while (true) {
		Sleep(delayInSearch);
		if (isNewOpponentReceived()) {
			cleanAllProcessMessages();
			cleanAllGuiMessages();
			handleNewReceivedOpponent();
			displayOpponentInfoFromWeb(lastFoundSteamId);
			areMessagesClean = false;
		}
		// opponent name is obtained directly from steam using the steam id; no need to scan the process for the name
		/*
		else if (isNewNameReceived(playerName, lastReceivedName) && isSteamIdFound) {
			if (lastReceivedName != NULL) {
				free(lastReceivedName);
			}
			lastReceivedName = readStringFromMemory(tekkenHandle, opponentNamePointer);
			displayOpponentName();
			areMessagesClean = false;
		}
		*/
		else if ((!areMessagesClean) && (!isSteamIdFound)) {
			cleanAllProcessMessages();
			cleanAllGuiMessages();
			areMessagesClean = true;
		}
		if (isNewFightAccepted()) {
			if ((lastNameInPlayerlist != NULL) && (lastNameInPlayerlist != currentLoadedOpponentName)) {
				free(lastNameInPlayerlist);
			}
			currentLoadedOpponentName = saveNewOpponentInPlayerlist(playerName, currentOpponentName, currentLoadedOpponentName);
			lastNameInPlayerlist = currentLoadedOpponentName;
		}
		if (!isWindow(tekkenWindowHandle)) {
			Sleep(3000); // wait to make sure the tekken process has closed after the window was closed
			loadTargetProcess();
		}
	}
}

void closeProgram() {
	CloseHandle(tekkenHandle);
}
