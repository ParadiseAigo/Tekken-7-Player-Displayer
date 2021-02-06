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

bool isSteamIdValid(void* steamIdPointer, QWORD* steamIdBuffer) {
	if (! isMemoryReadable(tekkenHandle, steamIdPointer)) {
		//myGuiTerminalPrint(std::string("Steam Id (memory) not readable (probably late).\r\n"));
		return false;
	}
	*steamIdBuffer = readQwordFromMemory(tekkenHandle, steamIdPointer);
	// aigo debugging
//	myGuiTerminalPrint(std::string("Checking steam id  ")
//		.append(std::to_string(*steamIdBuffer))
//		.append(std::string("\r\n"))
//	);
	if (*steamIdBuffer < 0x0110000100000000 || *steamIdBuffer > 0x0110000200000000) {
	// aigo debugging
//		myGuiTerminalPrint(std::string("Steam Id is bad!: ")
//			.append(std::to_string(*steamIdBuffer))
//			.append("\r\n")
//		);
		// steam id is 64 bits (8 bytes) long: 0x FF FF FF FF FF FF FF FF
		// first 8 bits represent the "universe" and its always equal to 1 for normal accounts
		// next 4 bits represent the "type" and its also always equal to 1 for normal accounts
		// next 20 bits represent the "instance" and its also always equal to 1 for normal accounts
		// final 32 bits (4 bytes) represent the real steam id
		// so a full steam id in hex looks like this (with X variable)  0x 01 10 00 01 XX XX XX XX
		return false;
	}
	return true;
}

bool isOpponentSteamIdValid() {
	QWORD steamId;
	void* steamIdPointer;
	steamIdPointer = (void*)getDynamicPointer(tekkenHandle, (void*)STEAM_ID_STATIC_POINTER, STEAM_ID_POINTER_OFFSETS);
	if (isSteamIdValid(steamIdPointer, &steamId)) {
		lastFoundSteamId = steamId;  // global variable
		return true;
	}
	else {
		return false;
	}
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

bool didNameAddressFail(char* currentOpponentName) {
	char* newOpponentName = readStringFromMemory(tekkenHandle, opponentNamePointer);
	if (strcmp(newOpponentName, currentOpponentName) != 0) { // if name I found does not match the one in the address
		free(newOpponentName);
		return true;
	}
	else {
		free(newOpponentName);
		return false;
	}
}

bool isNewBetterSteamIdReceived() {
	void* steamIdPointer = (void*)getDynamicPointer(tekkenHandle, (void*) ((QWORD)steamModulePointer + STEAM_ID_BETTER_STATIC_POINTER), STEAM_ID_BETTER_POINTER_OFFSETS);
	QWORD newFoundSteamId = -1;
	if (steamIdPointer == 0) { // steam api base module not loaded correctly
		myGuiTerminalPrint(std::string("Resetting Steam api module base address. \r\n"));
		initModuleAdresses();
		return false;
	}
	bool isSteamIdOk = isSteamIdValid(steamIdPointer, &newFoundSteamId);  // gets the new steam id
	if (newFoundSteamId != lastFoundBetterSteamId) {
		// aigo delete this
		myGuiTerminalPrint(std::string("Nameless steam id changed. (set to false)\r\n"));
		isNamelessSteamIdFound = false;
	}
	if (isSteamIdOk) {
		if (lastFoundBetterSteamId == -1) {   // ignore first found steam id after starting the program
			lastFoundBetterSteamId = newFoundSteamId;
			return false;
		}
		else if (newFoundSteamId != lastFoundBetterSteamId) {
			myGuiTerminalPrint(std::string("New nameless steam id found: ")
				.append(std::to_string(newFoundSteamId))
				.append(std::string("\r\n"))
			);
			lastFoundBetterSteamId = newFoundSteamId;
			return true;
		}
	}
	return false;
}

char* updateMessagesWithBetterSteamId(char* currentOpponentName) {
	currentOpponentName = updateMessagesWithoutOpponentName(currentOpponentName);
	return currentOpponentName;
}

char* updateMessagesWithoutOpponentName(char* currentOpponentName) {
	std::string fileString;
	char* line;
	char* steamIdBuffer;
	size_t steamIdBufferSize;
	char* newOpponentNameMessage;
	char* characterName;
	char* characterNameMessage;
	char* playerlistComment;

	steamIdBufferSize = 100;
	steamIdBuffer = (char*)malloc(steamIdBufferSize * sizeof(char));
	sprintf_s(steamIdBuffer, steamIdBufferSize, "(%I64u)", lastFoundBetterSteamId);
	fileString = fileToString((char*)PLAYERLIST_PATH);
	line = findLineInStringVector(stringToLines((char*) fileString.c_str()), steamIdBuffer);

	if (line != NULL) {  // steam id found in player list!
		myGuiTerminalPrint(std::string("Nameless steam id found in player list!\r\n"));
		if (currentOpponentName != NULL) {
			free(currentOpponentName);
		}
		newOpponentNameMessage = currentOpponentName = extractPlayerNameFromPlayerlistLine(line);
		playerlistComment = extractCommentFromPlayerlistLine(line);
		characterName = extractCharacterFromPlayerlistLine(line);
		characterNameMessage = myStringCat((char*)"| Last used: ", characterName);
		myGuiTerminalPrint(std::string("New opponent found:  ").append(std::string(currentOpponentName)).append(std::string("\r\n")));
		myGuiTerminalPrint(std::string("Last used character: ").append(std::string(characterName)).append(std::string("\r\n")));
		myGuiTerminalPrint(std::string("Comment:             ").append(std::string(playerlistComment)).append(std::string("\r\n")));
	}
	else {  // steam id NOT found in player list!
		myGuiTerminalPrint(std::string("Nameless steam id NOT found in player list!\r\n"));
		newOpponentNameMessage = copyString((char*) "New (not in list)");
		playerlistComment = copyString((char*) "Brand new opponent!");
		characterName = NULL;
		characterNameMessage = copyString((char*) "| Unknown character.");
	}
	updateOpponentFoundMessage(newOpponentNameMessage);
	updateFightThisPlayerMessage(playerlistComment);
	updateSecondsRemainingMessage(characterNameMessage);
	updateAllGuiMessages(newOpponentNameMessage, characterName, playerlistComment);
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
	if ((newOpponentNameMessage != NULL) && (newOpponentNameMessage != currentOpponentName)) {
		free(newOpponentNameMessage);
	}
	free(steamIdBuffer);
	return currentOpponentName;
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
	if (newOpponentName != NULL) {
		free(newOpponentName);
	}
}

bool isNewFightAccepted(char* playerName, char* currentOpponentName, char* currentLoadedOpponentName) {
	return isNewOpponentLoaded(playerName, currentOpponentName, currentLoadedOpponentName);
}

bool isNewOpponentLoaded(char* playerName, char* currentOpponentName, char* currentLoadedOpponentName) {
	//void* opponentStructSignaturePointer =  (void*)getDynamicPointer(tekkenHandle, (void*) OPPONENT_STRUCT_SIGNATURE_STATIC_POINTER, OPPONENT_STRUCT_SIGNATURE_POINTER_OFFSETS);
	void* opponentStructNamePointer = (void*)getDynamicPointer(tekkenHandle, (void*) OPPONENT_STRUCT_NAME_STATIC_POINTER, OPPONENT_STRUCT_NAME_POINTER_OFFSETS);
	//QWORD opponentStructSignature;
	char* opponentStructName;
	char* lastLineInPlayerlist;
	char* lastNameInPlayerlist;
	if (! isMemoryReadable(tekkenHandle, opponentStructNamePointer)) {
		return false;
	}
	//if (! isMemoryReadable(tekkenHandle, opponentStructSignaturePointer)) {
	//	return false;
	//}
	//opponentStructSignature = readQwordFromMemory(tekkenHandle, opponentStructSignaturePointer);
	//if (opponentStructSignature != OPPONENT_STRUCT_SIGNATURE) { //struct not loaded yet
	//	return false;
	//}
	opponentStructName = readStringFromMemory(tekkenHandle, opponentStructNamePointer);
	// aigo debugging (delete this)
//	myGuiTerminalPrint(std::string("opponentstructname = ")
//		.append(opponentStructName)
//		.append(std::string("\r\n"))
//	);
	// empty name, so no opponent loaded
	if ((strcmp((char*)"NOT_LOGGED_IN", opponentStructName) == 0) ||
		(opponentStructName[0] == '\0')) {
		free(opponentStructName);
		return false;
	}
	// still playing against the same person
	if (strcmp(currentLoadedOpponentName, opponentStructName) == 0) {
		free(opponentStructName);
		return false;
	}
	// name before accept is now same as loaded so true
	if (strcmp(currentOpponentName, opponentStructName) == 0) {
		free(opponentStructName);
		return true;
	}
	// if last saved name (in player list) same as the one loaded: dont save again
	lastLineInPlayerlist = getLastLineOfFile((char*)PLAYERLIST_PATH);
	lastNameInPlayerlist = extractPlayerNameFromPlayerlistLine(lastLineInPlayerlist);
	if (strcmp(opponentStructName, lastNameInPlayerlist) == 0) { 
		free(opponentStructName);
		free(lastLineInPlayerlist);
		free(lastNameInPlayerlist);
		return false;
	}
	// if
	// name address didnt change (still equal to the name of the user)
	// and
	// nameless steam id not found
	// then
	// this means the program failed to get any info, maybe the opp. is loaded, but it doesnt matter
	// this program just pretends there is no new opponent even if there is
	// (user shouldnt accept if the name is not displayed)
	if ((strcmp(currentOpponentName, playerName) == 0) &&
		(isNamelessSteamIdFound == false)) {	
		free(opponentStructName);
		free(lastLineInPlayerlist);
		free(lastNameInPlayerlist);
		return false;
	}
	else {
		free(opponentStructName);								  
		free(lastLineInPlayerlist);
		free(lastNameInPlayerlist);
		return true;											   
	}
}

// this function is not used:
// improves performance and modularity
// but makes readability worse 
bool isTimeToCleanMessages(char* playerName, char* currentOpponentName) {
	opponentNamePointer = (void*)getDynamicPointer(tekkenHandle, (void*)OPPONENT_NAME_STATIC_POINTER, OPPONENT_NAME_POINTER_OFFSETS);
	char* newOpponentName = readStringFromMemory(tekkenHandle, opponentNamePointer);
	if ((strcmp(newOpponentName, playerName) == 0) &&  // dont clean if name address is not same as user
		(strcmp(currentOpponentName, newOpponentName) != 0) && // if current opponent name != one in the process
		(currentOpponentName[0] != '\0') &&  // dont clean messages if the current opponent name has already been emptied by the clean messages if statement in the main loop
		(isNamelessSteamIdFound == false)) { // if new pointer changed (but failed) or it is not needed (because the name pointer worked)
		free(newOpponentName);
		return true;
	}
	else {
		free(newOpponentName);
		return false;
	}
}

void cleanAllProcessMessages() {
	writeStringUnlimitedToMemory(tekkenHandle, fightThisPlayerMessagePointer, (char*) "Dont accept :)");
	writeStringUnlimitedToMemory(tekkenHandle, secondsRemainingMessagePointer, (char*) "...");
	writeStringUnlimitedToMemory(tekkenHandle, opponentFoundMessagePointer, (char*) "Failed to get any info.");
}

char* getNewCurrentLoadedOpponent(char* currentLoadedOpponentName) {
	void* opponentStructNamePointer;
	char* newOpponentStructName;
	opponentStructNamePointer = (void*)getDynamicPointer(tekkenHandle, (void*) OPPONENT_STRUCT_NAME_STATIC_POINTER, OPPONENT_STRUCT_NAME_POINTER_OFFSETS);
	newOpponentStructName = readStringFromMemory(tekkenHandle, opponentStructNamePointer);
	free(currentLoadedOpponentName);
	return newOpponentStructName;
}
