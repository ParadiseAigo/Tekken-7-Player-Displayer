#include "player-displayer.h"
#include "pointers.h"

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
		lastFoundSteamId = -1;
		return false;
	}
	else if ((!readAndIsSteamIdValid(steamIdPointer, &newFoundSteamId))) { 	// if steam id not valid
		isSteamIdFound = false;
		lastFoundSteamId = -1;
		return false;
	}
	else if (userSteamId == newFoundSteamId) {								// if equal to user
		isSteamIdFound = false;
		lastFoundSteamId = -1;
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
		characterName = copyString((char*) "?");
		characterNameMessage = copyString((char*) "| Unknown character.");
	}
	updateAllGuiMessages(newOpponentNameMessage, characterName, playerlistComment);
	updateAllInGameMessages(newOpponentNameMessage, characterName, playerlistComment);
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

void updateAllInGameMessages(char* newOpponentNameMessage, char* characterName, char* playerlistComment) {
	updateSecondsRemainingMessage(newOpponentNameMessage);
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
	opponentStructNamePointer = (void*)getDynamicPointer(tekkenHandle, (void*) ((QWORD)tekkenModulePointer + OPPONENT_STRUCT_NAME_STATIC_POINTER), OPPONENT_STRUCT_NAME_POINTER_OFFSETS);
	opponentStructCharacterPointer = (void*)getDynamicPointer(tekkenHandle, (void*) ((QWORD)tekkenModulePointer + OPPONENT_STRUCT_CHARACTER_STATIC_POINTER), OPPONENT_STRUCT_CHARACTER_POINTER_OFFSETS);
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
	//updateOpponentFoundMessage((char*)"Failed to get any info.");
	//updateFightThisPlayerMessage((char*)"Dont accept :)");
	//updateSecondsRemainingMessage((char*)"...");
	updateSecondsRemainingMessage((char*)"Failed to get the name.");
}

char* getNewCurrentLoadedOpponent(char* currentLoadedOpponentName) {
	void* opponentStructNamePointer;
	char* newOpponentStructName;
	opponentStructNamePointer = (void*)getDynamicPointer(tekkenHandle, (void*) ((QWORD)tekkenModulePointer + OPPONENT_STRUCT_NAME_STATIC_POINTER), OPPONENT_STRUCT_NAME_POINTER_OFFSETS);
	newOpponentStructName = readStringFromMemory(tekkenHandle, opponentStructNamePointer);
	free(currentLoadedOpponentName);
	return newOpponentStructName;
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
	updateSecondsRemainingMessage((char*)name.c_str());
}

void displayOpponentNameFromWeb(std::string name) {
	char* opponentName = (char*)name.c_str();
	myGuiTerminalPrint(std::string("Steam id's name:  ").append(std::string(opponentName)).append(std::string("\r\n")));
	setOpponentNameOneInGui(opponentName);
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

void updateOpponentNameTwo() {
	opponentNamePointer = (void*)getDynamicPointer(tekkenHandle, (void*) ((QWORD)tekkenModulePointer + OPPONENT_NAME_STATIC_POINTER), OPPONENT_NAME_POINTER_OFFSETS);
	char* opponentName = readStringFromMemory(tekkenHandle, opponentNamePointer);
	if (strcmp(lastFoundName, opponentName) != 0) {
		setOpponentNameTwoInGui(opponentName);
		free(lastFoundName);
		lastFoundName = copyString(opponentName);
	}
	free(opponentName);
}

void turnOffSilentMode() {
	if (silentMode == false) { // if already turned off
		myGuiTerminalPrint(std::string("Silent mode is already off. Restart Tekken 7 to turn it on.\r\n"));
	}
	else {
		silentMode = false;
		myGuiTerminalPrint(std::string("Silent mode turned off. Now feedback will also be given in-game.\r\n"));
		cleanAllProcessMessages();
	}
}

