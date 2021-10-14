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
char* lastFoundName;

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
}

void loadTargetProcess() {
	initTekkenHandle();
	initTekkenWindowHandle();
	initPointers();
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
	screenModePointer = (void*)getDynamicPointer(tekkenHandle, (void*)SCREEN_MODE_STATIC_POINTER, SCREEN_MODE_POINTER_OFFSETS);
	initModuleAdresses();
	myGuiTerminalPrint(std::string("Pointers loaded.\r\n"));
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
	char* currentLoadedOpponentName;
	bool areMessagesClean;
	int delayWhileSearching = 1000/10; // "10fps" (updates 10 times a sec)
	int delayWhileFighting = 2000; // 2 seconds, unused variable :(
	currentLoadedOpponentName = (char*) malloc(10 * sizeof(char)); // dummy value
	currentLoadedOpponentName[0] = '\0'; // empty string
	lastFoundSteamId = -1;  // global variable
	isSteamIdFound = false; // global variable
	lastFoundName = (char*) malloc(10 * sizeof(char)); // dummy value	// global variable
	lastFoundName[0] = '\0'; // empty string
	areMessagesClean = true;
	lastNameInPlayerlist = getLastNameInPlayerlist((char*)PLAYERLIST_PATH); //global variable  // set equal to NULL if player list is empty
	while (true) {
		Sleep(delayWhileSearching);
		updateOpponentNameTwo();
		if (isNewOpponentReceived()) {
			cleanAllGuiMessages();
			handleNewReceivedOpponent();
			displayOpponentInfoFromWeb(lastFoundSteamId);
			areMessagesClean = false;
		}
		else if ((!areMessagesClean) && (!isSteamIdFound)) {
			cleanAllGuiMessages();
			areMessagesClean = true;
		}
		if (isNewFightAccepted()) {
			if ((lastNameInPlayerlist != NULL) && (lastNameInPlayerlist != currentLoadedOpponentName)) {
				free(lastNameInPlayerlist);
			}
			currentLoadedOpponentName = saveNewOpponentInPlayerlist(currentLoadedOpponentName);
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
