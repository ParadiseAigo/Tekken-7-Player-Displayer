#include "player-displayer.h"
#include "pointers.h"

bool isGameLoaded() {
	// will assume game is loaded if the opponent name address contains a (non-empty) string
	opponentNamePointer = (void*)getDynamicPointer(tekkenHandle, (void*)OPPONENT_NAME_STATIC_POINTER, OPPONENT_NAME_POINTER_OFFSETS);
	char * playerName = readStringFromMemory(tekkenHandle, opponentNamePointer);
	if (playerName[0] == '\0') {
		free(playerName);
		return false;
	}
	else {
		free(playerName);
		return true;
	}
}

bool isNewOpponentReceived(char* playerName, char* currentOpponentName) {
	char* newOpponentName = readStringFromMemory(tekkenHandle, opponentNamePointer);
	if (strcmp(newOpponentName, currentOpponentName) == 0) { // if equal; still same opponent
		free(newOpponentName);
		return false;
	}
	if (strcmp(newOpponentName, playerName) == 0) { // if equal; failed to retreive data, or there is no new opponent
		free(newOpponentName);
		return false;
	}
	free(newOpponentName);
	return true;
}

bool isOpponentSteamIdValid() {
	QWORD steamId;
	steamIdPointer = (void*)getDynamicPointer(tekkenHandle, (void*)STEAM_ID_STATIC_POINTER, STEAM_ID_POINTER_OFFSETS);
	if (! isMemoryReadable(tekkenHandle, steamIdPointer)) {
		myGuiTerminalPrint(std::string("Steam Id (memory) not readable (probably late).\r\n"));
		return false;
	}
	steamId = readQwordFromMemory(tekkenHandle, steamIdPointer);
	if (steamId < 0x0110000100000000 || steamId > 0x0110000200000000) {
		myGuiTerminalPrint(std::string("Steam Id is bad!\r\n"));
		// steam id is 64 bits (8 bytes) long: 0x FF FF FF FF FF FF FF FF
		// first 8 bits represent the "universe" and its always equal to 1 for normal accounts
		// next 4 bits represent the "type" and its also always equal to 1 for normal accounts
		// next 20 bits represent the "instance" and its also always equal to 1 for normal accounts
		// final 32 bits (4 bytes) represent the real steam id
		// so a full steam id in hex looks like this (with X variable)  0x 01 10 00 01 XX XX XX XX
		return false;
	}
	lastFoundSteamId = steamId;  // global variable
	return true;
}

char* handleNewOpponent(char* currentOpponentName) {
	char* newOpponentName;
	QWORD steamId;
	size_t steamIdBufferSize;
	char* steamIdBuffer;
	std::string fileString;
	char* line;
	char* playerlistComment;
	char* characterNameMessage;
	char* characterName;
	newOpponentName = readStringFromMemory(tekkenHandle, opponentNamePointer);
	myGuiTerminalPrint(std::string("New opponent found:  ").append(std::string(newOpponentName)).append(std::string("\r\n")));
	steamIdBufferSize = 100;
	steamIdBuffer = (char*)malloc(steamIdBufferSize * sizeof(char));
	steamId = lastFoundSteamId;
	sprintf_s(steamIdBuffer, steamIdBufferSize, "(%I64u)", steamId);
	fileString = fileToString((char*)PLAYERLIST_PATH);
	line = findLineInStringVector(stringToLines((char*) fileString.c_str()), steamIdBuffer);
	if (line != NULL) {
		playerlistComment = extractCommentFromPlayerlistLine(line);
		characterName = extractCharacterFromPlayerlistLine(line);
		characterNameMessage = myStringCat((char*)"| Last used: ", characterName);
	}
	else {
		playerlistComment = copyString((char*) "Brand new opponent!");
		characterName = NULL;
		characterNameMessage = copyString((char*) "| Unknown character.");
	}
	updateOpponentFoundMessage(newOpponentName);
	updateFightThisPlayerMessage(playerlistComment);
	updateSecondsRemainingMessage(characterNameMessage);
	updateAllGuiMessages(newOpponentName, characterName, playerlistComment);
	if (characterName == NULL) {
		myGuiTerminalPrint(std::string("Last used character: /\r\n"));
	}
	else {
		myGuiTerminalPrint(std::string("Last used character: ").append(std::string(characterName)).append(std::string("\r\n")));
	}
	myGuiTerminalPrint(std::string("Comment:             ").append(std::string(playerlistComment)).append(std::string("\r\n")));
	if (characterName != NULL) {
		free(characterName);
	}
	if (characterNameMessage != NULL) {
		free(characterNameMessage);
	}
	if (playerlistComment != NULL) {
		free(playerlistComment);
	}
	if (line != NULL) {
		free(line);
	}
	free(steamIdBuffer);
	free(currentOpponentName);
	return newOpponentName; // calling function should set currentOpponentName equal to newOpponentName
}

void updateOpponentFoundMessage(char* message) {
	writeStringUnlimitedToMemory(tekkenHandle, opponentFoundMessagePointer, message);
}

void updateFightThisPlayerMessage(char* message) {
	writeStringUnlimitedToMemory(tekkenHandle, fightThisPlayerMessagePointer, message);
}

void updateSecondsRemainingMessage(char* message) {
	writeStringUnlimitedToMemory(tekkenHandle, secondsRemainingMessagePointer, message);
}

void updateMessagesWithoutSteamId() {
	char* newOpponentName;
	std::string fileString;
	char* line;
	char* playerlistComment;
	char* characterNameMessage;
	char* characterName;
	newOpponentName = readStringFromMemory(tekkenHandle, opponentNamePointer);
	fileString = fileToString((char*)PLAYERLIST_PATH);
	// without the steam id: search the playerlist by name instead of by steam id
	line = findLineInStringVector(stringToLines((char*) fileString.c_str()), newOpponentName);
	if (line != NULL) {
		playerlistComment = extractCommentFromPlayerlistLine(line);
		characterName = extractCharacterFromPlayerlistLine(line);
		characterNameMessage = myStringCat((char*)"| Last used: ", characterName);
	}
	else {
		playerlistComment = copyString((char*) "Brand new opponent!");
		characterName = NULL;
		characterNameMessage = copyString((char*) "| Unknown character.");
	}
	updateOpponentFoundMessage(newOpponentName);
	updateFightThisPlayerMessage(playerlistComment);
	updateSecondsRemainingMessage(characterNameMessage);
	updateAllGuiMessages(newOpponentName, characterName, playerlistComment);
	if (characterName != NULL) {
		free(characterName);
	}
	if (characterNameMessage != NULL) {
		free(characterNameMessage);
	}
	if (playerlistComment != NULL) {
		free(playerlistComment);
	}
	if (line != NULL) {
		free(line);
	}
}

bool isNewFightAccepted(char* playerName, char* currentOpponentName, char* currentLoadedOpponentName) {
	return isNewOpponentLoaded(playerName, currentOpponentName, currentLoadedOpponentName);
}

bool isNewOpponentLoaded(char* playerName, char* currentOpponentName, char* currentLoadedOpponentName) {
	void* opponentStructSignaturePointer =  (void*)getDynamicPointer(tekkenHandle, (void*) OPPONENT_STRUCT_SIGNATURE_STATIC_POINTER, OPPONENT_STRUCT_SIGNATURE_POINTER_OFFSETS);
	void* opponentStructNamePointer = (void*)getDynamicPointer(tekkenHandle, (void*) OPPONENT_STRUCT_NAME_STATIC_POINTER, OPPONENT_STRUCT_NAME_POINTER_OFFSETS);
	char* opponentStructName;
	QWORD opponentStructSignature;
	if (! isMemoryReadable(tekkenHandle, opponentStructNamePointer)) {
		return false;
	}
	if (! isMemoryReadable(tekkenHandle, opponentStructSignaturePointer)) {
		return false;
	}
	opponentStructSignature = readQwordFromMemory(tekkenHandle, opponentStructSignaturePointer);
	if (opponentStructSignature != OPPONENT_STRUCT_SIGNATURE) { //struct not loaded yet
		return false;
	}
	opponentStructName = readStringFromMemory(tekkenHandle, opponentStructNamePointer);
	if (opponentStructName[0] == '\0') { // empty name, so no opponent loaded
		free(opponentStructName);
		return false;
	}
	if (strcmp(currentLoadedOpponentName, opponentStructName) == 0) { // still playing against the same person
		free(opponentStructName);
		return false;
	}
	if (strcmp((char*)"NOT_LOGGED_IN", opponentStructName) == 0) {
		free(opponentStructName);
		return false;
	}
	if (strcmp(currentOpponentName, playerName) == 0) { // if equal; this means opp. loaded, but failed to get steam
														// info; this problem cant be helped, so this program just
														// pretends there is no new opponent (user shouldnt accept
														// if the name is not displayed)
		free(opponentStructName);
		return false;
	}
	if (strcmp(currentOpponentName, opponentStructName) != 0) { // if loaded name doesnt match it probably means
		free(opponentStructName);							   // this is the name of the previous loaded person
		return false;
	}
	free(opponentStructName);								  
	return true;											   
}

bool isTimeToCleanMessages(char* playerName, char* currentOpponentName) {
	char* newOpponentName = readStringFromMemory(tekkenHandle, opponentNamePointer);
	if (strcmp(newOpponentName, playerName) == 0 && strcmp(currentOpponentName, newOpponentName) != 0 && currentOpponentName[0] != '\0') {
		free(newOpponentName);
		return true;
	}
	free(newOpponentName);
	return false;
}

void cleanAllProcessMessages() {
	writeStringUnlimitedToMemory(tekkenHandle, fightThisPlayerMessagePointer, (char*) "Dont accept :)");
	writeStringUnlimitedToMemory(tekkenHandle, secondsRemainingMessagePointer, (char*) "...");
	writeStringUnlimitedToMemory(tekkenHandle, opponentFoundMessagePointer, (char*) "Steam didnt give us the name.");
}

char* getNewCurrentLoadedOpponent(char* currentLoadedOpponentName) {
	void* opponentStructNamePointer;
	char* newOpponentStructName;
	opponentStructNamePointer = (void*)getDynamicPointer(tekkenHandle, (void*) OPPONENT_STRUCT_NAME_STATIC_POINTER, OPPONENT_STRUCT_NAME_POINTER_OFFSETS);
	newOpponentStructName = readStringFromMemory(tekkenHandle, opponentStructNamePointer);
	free(currentLoadedOpponentName);
	return newOpponentStructName;
}
