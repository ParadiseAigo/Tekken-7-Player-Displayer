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

bool isNewOpponentReceived() {
	return isNewSteamIdReceived();
}

bool isNewSteamIdReceived() {
	QWORD newFoundSteamId;
	void* steamIdPointer;
	newFoundSteamId = -1;
	steamIdPointer = (void*)getDynamicPointer(tekkenHandle,(void*) ((QWORD)steamModulePointer + STEAM_ID_BETTER_STATIC_POINTER), STEAM_ID_BETTER_POINTER_OFFSETS);
	if (!isSteamApiLoaded(steamIdPointer)) {								// if steam api not loaded
		resetSteamApiBaseModuleAddress();
		isSteamIdFound = false;
		return false;
	}
	else if ((!readAndIsSteamIdValid(steamIdPointer, &newFoundSteamId))) { 	// if steam id not valid
		isSteamIdFound = false;
		return false;
	}
	else if (userSteamId == newFoundSteamId) {								// if equal to user
		isSteamIdFound = false;
		return false;
	}
	else if (newFoundSteamId == lastFoundSteamId) {							// if unchanged
		isSteamIdFound = true;
		return false;
	}
	else {																	// new steam id received!
		myGuiTerminalPrint(std::string("New Steam id found: ")
			.append(std::to_string(newFoundSteamId))
			.append(std::string("\r\n"))
		);
		isSteamIdFound = true;
		lastFoundSteamId = newFoundSteamId;
		return true;
	}
}

bool isSteamApiLoaded(void* steamIdPointer) {
	if (steamIdPointer == 0) {
		return false;
	}
	else {
		return true;
	}
}

void resetSteamApiBaseModuleAddress() {
	myGuiTerminalPrint(std::string("Resetting Steam api module base address.\r\n"));
	initModuleAdresses();
}

bool readAndIsSteamIdValid(void* steamIdPointer, QWORD* steamIdBuffer) {
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

void handleNewReceivedOpponent() {
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
	sprintf_s(steamIdBuffer, steamIdBufferSize, "(%I64u)", lastFoundSteamId);
	fileString = fileToString((char*)PLAYERLIST_PATH);
	line = findLineInStringVector(stringToLines((char*) fileString.c_str()), steamIdBuffer);

	if (line != NULL) {  // steam id found in player list!
		myGuiTerminalPrint(std::string("Steam id found in player list!\r\n"));
		newOpponentNameMessage = extractPlayerNameFromPlayerlistLine(line);
		playerlistComment = extractCommentFromPlayerlistLine(line);
		characterName = extractCharacterFromPlayerlistLine(line);
		characterNameMessage = myStringCat((char*)"| Last used: ", characterName);
		myGuiTerminalPrint(std::string("New opponent found:  ").append(std::string(newOpponentNameMessage)).append(std::string("\r\n")));
		myGuiTerminalPrint(std::string("Last used character: ").append(std::string(characterName)).append(std::string("\r\n")));
		myGuiTerminalPrint(std::string("Comment:             ").append(std::string(playerlistComment)).append(std::string("\r\n")));
	}
	else {  // steam id NOT found in player list!
		myGuiTerminalPrint(std::string("Steam id NOT found in player list!\r\n"));
		newOpponentNameMessage = copyString((char*) "New (not in list)");
		playerlistComment = copyString((char*) "Brand new opponent!");
		characterName = NULL;
		characterNameMessage = copyString((char*) "| Unknown character.");
	}
	//aigo make a function called  updateAllProcessMessages(.., .., ..);  // and inside it put an "if" silent mode ...
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
	if ((newOpponentNameMessage != NULL)) {
		free(newOpponentNameMessage);
	}
	free(steamIdBuffer);
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
	sprintf_s(steamIdBuffer, steamIdBufferSize, "(%I64u)", lastFoundSteamId);
	fileString = fileToString((char*)PLAYERLIST_PATH);
	line = findLineInStringVector(stringToLines((char*) fileString.c_str()), steamIdBuffer);

	if (line != NULL) {  // steam id found in player list!
		myGuiTerminalPrint(std::string("Steam id found in player list!\r\n"));
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
		myGuiTerminalPrint(std::string("Steam id NOT found in player list!\r\n"));
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

void updateOpponentFoundMessage(char* message) {
	if (silentMode == false) {
		writeStringUnlimitedToMemory(tekkenHandle, opponentFoundMessagePointer, message);
	}
}

void updateFightThisPlayerMessage(char* message) {
	if (silentMode == false) {
		writeStringUnlimitedToMemory(tekkenHandle, fightThisPlayerMessagePointer, message);
	}
}

void updateSecondsRemainingMessage(char* message) {
	if (silentMode == false) {
		writeStringUnlimitedToMemory(tekkenHandle, secondsRemainingMessagePointer, message);
	}
}

bool isNewFightAccepted() {
	return isNewOpponentLoaded();
}

bool isNewOpponentLoaded() {
	void* opponentStructNamePointer;
	char* opponentStructName;
	void* opponentStructCharacterPointer;
	QWORD opponentStructCharacter;
	QWORD numberOfCharacters;
	std::vector<std::string> allCharacters ALL_CHARACTERS;
	numberOfCharacters = allCharacters.size();
	opponentStructNamePointer = (void*)getDynamicPointer(tekkenHandle, (void*) OPPONENT_STRUCT_NAME_STATIC_POINTER, OPPONENT_STRUCT_NAME_POINTER_OFFSETS);
	opponentStructCharacterPointer = (void*)getDynamicPointer(tekkenHandle, (void*) OPPONENT_STRUCT_CHARACTER_STATIC_POINTER, OPPONENT_STRUCT_CHARACTER_POINTER_OFFSETS);
	if ((!isMemoryReadable(tekkenHandle, opponentStructNamePointer)) ||
		(!isMemoryReadable(tekkenHandle, opponentStructCharacterPointer))) {
		return false;
	}
	opponentStructName = readStringFromMemory(tekkenHandle, opponentStructNamePointer);
	opponentStructCharacter = readDwordFromMemory(tekkenHandle, opponentStructCharacterPointer);
	// aigo debugging (delete this)
//	myGuiTerminalPrint(std::string("opponentstructcharacter = ")
//		.append(std::to_string(opponentStructCharacter))
//		.append(std::string(" , opponentstructname = "))
//		.append(opponentStructName)
//		.append(std::string("\r\n"))
//	);
	if ((strcmp((char*)"NOT_LOGGED_IN", opponentStructName) != 0) &&
		(opponentStructName[0] != '\0') &&
		(isSteamIdFound == true) &&
		(opponentStructCharacter < numberOfCharacters) &&
		(opponentStructCharacter >= 0) && // 0 is paul ^_^
		(strcmp(lastNameInPlayerlist, opponentStructName) != 0)) {
		free(opponentStructName);
		return true;
	}
	else {
		free(opponentStructName);
		return false;
	}
}

void cleanAllProcessMessages() {
	updateOpponentFoundMessage((char*)"Failed to get any info.");
	updateFightThisPlayerMessage((char*)"Dont accept :)");
	updateSecondsRemainingMessage((char*)"...");
}

char* getNewCurrentLoadedOpponent(char* currentLoadedOpponentName) {
	void* opponentStructNamePointer;
	char* newOpponentStructName;
	opponentStructNamePointer = (void*)getDynamicPointer(tekkenHandle, (void*) OPPONENT_STRUCT_NAME_STATIC_POINTER, OPPONENT_STRUCT_NAME_POINTER_OFFSETS);
	newOpponentStructName = readStringFromMemory(tekkenHandle, opponentStructNamePointer);
	free(currentLoadedOpponentName);
	return newOpponentStructName;
}

bool isNewNameReceived(char* playerName, char* lastReceivedName) {
	char* newOpponentName = readStringFromMemory(tekkenHandle, opponentNamePointer);
	if (strcmp(newOpponentName, lastReceivedName) == 0) { // if equal; still same opponent
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

void displayOpponentName() {
	char* newOpponentName = readStringFromMemory(tekkenHandle, opponentNamePointer);
	myGuiTerminalPrint(std::string("New opponent found:  ").append(std::string(newOpponentName)).append(std::string("\r\n")));
	updateOpponentFoundMessage(newOpponentName);
	setOpponentNameInGui(newOpponentName);
	free(newOpponentName);
}

void displayOpponentInfoFromWeb(QWORD steamId) {
    std::string htmlString;
	std::string name;
	std::string pictureLink;
    htmlString = getSteamPageHtml(steamId);
	name = extractNameFromSteamHtmlString(htmlString);
	pictureLink = extractProfilePictureUrlFromSteamHtmlString(htmlString);
	displayOpponentNameFromWeb(name);
	displayOpponentProfilePictureFromWeb(pictureLink);
}

void displayOpponentNameFromWeb(std::string name) {
	char* opponentName = (char*)name.c_str();
	myGuiTerminalPrint(std::string("Steam id's name:  ").append(std::string(opponentName)).append(std::string("\r\n")));
	updateOpponentFoundMessage(opponentName);
	setOpponentNameInGui(opponentName);
}

void displayOpponentProfilePictureFromWeb(std::string pictureLink) {
	TCHAR picturePath[MAX_PATH];
	GetTempPathW(MAX_PATH, picturePath);
	wsprintf(picturePath, TEXT("%s\\opponent.jpg"), picturePath);

	urlToFile(pictureLink, picturePath);
	
	size_t posExtension = pictureLink.find_last_of('.');
	std::string fileExtension = pictureLink.substr(posExtension + 1);
	if (fileExtension.compare("png") == 0) {
		loadOpponentProfilePictureFromPNGFileAndRedraw(picturePath);
	} else {
		loadOpponentProfilePictureFromFileAndRedraw(picturePath);
	}
}

void turnOffSilentMode() {
	if (isTekkenLoaded == false) {
		myGuiTerminalPrint(std::string("Can't turn off silent mode: Tekken 7 not loaded yet.\r\n"));
	}
	else if (silentMode == false) { // if already turned off
		myGuiTerminalPrint(std::string("Silent mode is already off. Restart Tekken 7 to turn it off.\r\n"));
	}
	else {
		silentMode = false;
		myGuiTerminalPrint(std::string("Silent mode turned off. Now feedback will be given in-game.\r\n"));
		cleanAllProcessMessages();
	}
}

