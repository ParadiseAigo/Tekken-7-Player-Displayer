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

int main() {
	init();
	mainLoop();
	closeProgram();
}

void init() {
	initTekkenHandle();
	initTekkenWindowHandle();
	initPointers();
	cleanAllProcessMessages();
	std::cout << "Program ready!" << std::endl;
}

void initTekkenHandle() {
	DWORD pid, secondsDelay;
	pid = getProcessId(TEXT(TEKKEN_EXE_NAME));
	secondsDelay = 2;
	while (pid == 0) {
		std::cout << "Tekken not found. Trying again in " << secondsDelay << " seconds..." << std::endl;
		Sleep(secondsDelay * 1000); // milliseconds
		pid = getProcessId(TEXT(TEKKEN_EXE_NAME));
	}
	tekkenPid = pid; // global variable
	std::cout << "Tekken found! pid = (" << pid << ")" << std::endl;
	std::cout << "Opening Tekken process..." << std::endl;
	tekkenHandle = getProcessHandle(pid);  // global variable
	std::cout << "Opening Tekken process success!" << std::endl;
}

void initTekkenWindowHandle() {
	DWORD secondsDelay;
	secondsDelay = 2;
	while ( (tekkenWindowHandle = getWindowHandle(TEXT(TEKKEN_WINDOW_NAME))) == NULL ) { // global variable
		std::cout << "Searching for Tekken window. Trying again in " << secondsDelay << " seconds..." << std::endl;
		Sleep(secondsDelay * 1000); // milliseconds
	}
	std::cout << "Tekken window found!" << std::endl;
}

void initPointers() {
	DWORD secondsDelay;
	secondsDelay = 2;
	while (! isGameLoaded() ) {
		std::cout << "Game not loaded yet. Waiting " << secondsDelay << " seconds..." << std::endl;
		Sleep(secondsDelay * 1000); // milliseconds
	}
	opponentNamePointer = (void*)getDynamicPointer(tekkenHandle, (void*)OPPONENT_NAME_STATIC_POINTER, OPPONENT_NAME_POINTER_OFFSETS);
	fightThisPlayerMessagePointer = (void*)getDynamicPointer(tekkenHandle, (void*)FIGHT_THIS_PLAYER_MESSAGE_STATIC_POINTER, FIGHT_THIS_PLAYER_MESSAGE_POINTER_OFFSETS);
	secondsRemainingMessagePointer = (void*)getDynamicPointer(tekkenHandle, (void*)SECONDS_REMAINING_MESSAGE_STATIC_POINTER, SECONDS_REMAINING_MESSAGE_POINTER_OFFSETS);
	opponentFoundMessagePointer = (void*)getDynamicPointer(tekkenHandle, (void*)OPPONENT_FOUND_MESSAGE_STATIC_POINTER, OPPONENT_FOUND_MESSAGE_POINTER_OFFSETS);
	screenModePointer = (void*)getDynamicPointer(tekkenHandle, (void*)SCREEN_MODE_STATIC_POINTER, SCREEN_MODE_POINTER_OFFSETS);
	steamIdPointer = (void*)getDynamicPointer(tekkenHandle, (void*)STEAM_ID_STATIC_POINTER, STEAM_ID_POINTER_OFFSETS);
	char* playerName = readStringFromMemory(tekkenHandle, opponentNamePointer);
	std::cout << "Pointers loaded.\nPlayer name (yours): " << playerName << std::endl;
	std::cout << "WARNING! If the above name is not your name, ";
	std::cout << "restart the program and when you do, make sure you are not ";
	std::cout << "in a match or you havent just declined an opponent ";
	std::cout << "or are in the process of accepting a match.\n";
	free(playerName);
}

void mainLoop() {
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
	while (true) {
		if (currentState == IN_SEARCH) {
			Sleep(delayInSearch);
		}
		else if (currentState == IN_FIGHT) {
			Sleep(delayInFight);
		}
		handleInput();
		if (isTimeToCleanMessages(playerName, currentOpponentName)) {
			cleanAllProcessMessages();
			currentOpponentName[0] = '\0';
			currentState = IN_SEARCH;
			printf("Ready to find the next opponent.\n");
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
	}
	// program never reaches this comment
}

// this function is never called... (fix it!)
void closeProgram() {
	CloseHandle(tekkenHandle);
	CloseHandle(tekkenWindowHandle);
	system("PAUSE");
}

