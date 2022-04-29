#include "player-displayer.h"
#include "pointers.h"

void createExamplePlayerlist() {
	char* newPlayerlistLine;
	std::string allCharacters[] = ALL_CHARACTERS;
	int paul = 0;
	int hwo = 4;
	createFile((char*) PLAYERLIST_PATH);
	newPlayerlistLine = makePlayerlistEntry((char*)"narcissist_city", (char*) allCharacters[paul].c_str(), 76561197960265729);
	writeLineToFile((char*)PLAYERLIST_PATH, newPlayerlistLine);
	free(newPlayerlistLine);
	replaceCommentInLastLineInFile((char*)PLAYERLIST_PATH, (char*)"toxic plugger stay away");
	newPlayerlistLine = makePlayerlistEntry((char*)"bigboss99", (char*) allCharacters[hwo].c_str(), 76561197960265730);
	writeLineToFile((char*)PLAYERLIST_PATH, newPlayerlistLine);
	free(newPlayerlistLine);
}

char* extractPlayerNameFromPlayerlistLine(char* line) {
	int lineIndexBegin = 0;  // player name is the first thing in the line so its always = 0
	int lineIndexEnd = 0;
	int lineSize = (int) strlen(line);
	int tabsCount = 0;
	std::string character(line);
	if ((line != NULL) && (strcmp(line, (char*) "")) != 0) {
		while ((lineIndexEnd + 2) < lineSize) {
			if (line[lineIndexEnd] != '\t') {
				lineIndexEnd++;
			}
			else {
				break;
			}
		}
		std::string result = character.substr(lineIndexBegin, (lineIndexEnd - lineIndexBegin));
		return copyString((char*)result.c_str());
	}
	return NULL;
}

char* extractCommentFromPlayerlistLine(char* line) {
	int lineIndex = 0;
	int lineSize = (int) strlen(line);
	int tabsCount = 0;
	std::string comment(line);
	if (line != NULL) {
		while (tabsCount < 3 && (lineIndex + 2) < lineSize) { // getting the start of the comment in the playerlist line
			if (line[lineIndex] == '\t') {
				if (line[lineIndex + 1] != '\t') {
					tabsCount++;
				}
				else {
					// ignore mutliple tabs
				}
			}
			lineIndex++;
		}
		return copyString((char*)comment.substr(lineIndex, lineSize).c_str());
	}
	return NULL;
}

char* extractCharacterFromPlayerlistLine(char* line) {
	int lineIndexBegin = 0;
	int lineIndexEnd = 0;
	int lineSize = (int) strlen(line);
	int tabsCount = 0;
	std::string character(line);
	if (line != NULL) {
		while (tabsCount < 1 && (lineIndexBegin + 2) < lineSize) {
			if (line[lineIndexBegin] == '\t') {
				if (line[lineIndexBegin + 1] != '\t') {
					tabsCount++;
				}
				else {
					// ignore mutliple tabs
				}
			}
			lineIndexBegin++;
		}
		lineIndexEnd = lineIndexBegin;
		while ((lineIndexEnd + 2) < lineSize) {
			if (line[lineIndexEnd] != '\t') {
				lineIndexEnd++;
			}
			else {
				break;
			}
		}
		std::string result = character.substr(lineIndexBegin, (lineIndexEnd - lineIndexBegin));
		return copyString((char*)result.c_str());
	}
	return NULL;
}

char* myStringCat(char* s1, char* s2) {
	char* result = (char*)calloc(strlen(s1) + strlen(s2) + 2, sizeof(char));
	strcpy_s(result, strlen(s1) + 1, s1);
	strcat_s(result, strlen(result) + strlen(s2) + 1, s2);
	return result;
}

char* copyString(char* s) {
	char* result = (char*) malloc((strlen(s) + 1) * sizeof(char));
	strcpy_s(result, strlen(s) + 1, s);
	return result;
}

void openPlayerlist() {
	myGuiTerminalPrint(std::string("Opening playerlist...\r\n"));
	if (doesFileExist((char*) PLAYERLIST_PATH)) {
		setScreenMode(SCREEN_MODE_WINDOWED);
		ShellExecute(0, 0, TEXT(PLAYERLIST_PATH), 0, 0 , SW_SHOW ); //(const wchar_t*)
	}
	else {
		myGuiTerminalPrint(std::string("Playerlist is not found, creating a new one...\r\n"));
		createFile((char*) PLAYERLIST_PATH);
	}
}

bool doesFileExist(char* filePath) {
	FILE* file;
	errno_t errorCode;
	if (0 != (errorCode = fopen_s(&file, filePath, "r"))) {
		return false;
	}
	else {
		fclose(file);
		return true;
	}
}

void createFile(char* filePath) {
	FILE* file;
	errno_t errorCode;
	if (0 != (errorCode = fopen_s(&file, filePath, "a"))) {
		myGuiTerminalPrint(std::string("Error in createFile: Error creating the file ").append(std::string(filePath)).append(std::string("\r\n")));
	}
	else {
		myGuiTerminalPrint(std::string("File \"").append(std::string(filePath)).append(std::string("\" created.\r\n")));
		fclose(file);
	}
}

char* saveNewOpponentInPlayerlist(char* currentLoadedOpponentName) {
	currentLoadedOpponentName = getNewCurrentLoadedOpponent(currentLoadedOpponentName);
	saveNewPlayerlistEntry(currentLoadedOpponentName);
	return currentLoadedOpponentName;
}

void saveNewPlayerlistEntry(char* currentLoadedOpponentName) {
	void* opponentStructCharacterPointer;
	DWORD newOpponentStructCharacter;
	char* newPlayerlistLine;
	QWORD steamId;
	std::string allCharacters[] = ALL_CHARACTERS;
	opponentStructCharacterPointer = (void*)getDynamicPointer(tekkenHandle, (void*) ((QWORD)tekkenModulePointer + OPPONENT_STRUCT_CHARACTER_STATIC_POINTER), OPPONENT_STRUCT_CHARACTER_POINTER_OFFSETS);
	newOpponentStructCharacter = readDwordFromMemory(tekkenHandle, opponentStructCharacterPointer);
	steamId = lastFoundSteamId;
	myGuiTerminalPrint(std::string("New opponent    : ").append(std::string(currentLoadedOpponentName)).append(std::string("\r\n")));
	myGuiTerminalPrint(std::string("Character id    : ").append(std::to_string(newOpponentStructCharacter)).append(std::string("\r\n")));
	myGuiTerminalPrint(std::string("Steam id        : ").append(std::to_string(steamId)).append(std::string("\r\n")));
	if (newOpponentStructCharacter != 255) {
		myGuiTerminalPrint(std::string("Character name:   ").append(allCharacters[newOpponentStructCharacter]).append(std::string("\r\n")));
		newPlayerlistLine = makePlayerlistEntry(currentLoadedOpponentName, (char*) allCharacters[newOpponentStructCharacter].c_str(), steamId);
		myGuiTerminalPrint(std::string("Playerlist entry: ").append(std::string(newPlayerlistLine)).append(std::string("\r\n")));
		writeLineToFile((char*)PLAYERLIST_PATH, newPlayerlistLine);
		free(newPlayerlistLine);
	}
}

char* makePlayerlistEntry(char* playerName, char* characterName, QWORD steamId) {
	std::string result = "";
	int steamIdBufferSize = 100;
	char* steamIdBuffer = (char*) malloc(steamIdBufferSize * sizeof(char));
	sprintf_s(steamIdBuffer, steamIdBufferSize, "(%I64u)", steamId);
	result.append(playerName);
	result.append("\t\t\t");
	result.append(characterName);
	result.append("\t\t\t");
	result.append(steamIdBuffer);
	result.append("\t\t\t");
	result.append("no comment yet");
	free(steamIdBuffer);
	return copyString((char*) result.c_str());
}

void writeLineToFile(char* path, char* line) {
	FILE* file;
	errno_t errorCode;
	if (0 != (errorCode = fopen_s(&file, path, "r+"))) {
		myGuiTerminalPrint(std::string("Error opening a file in writeLineToFile, error code = ")
			.append(std::to_string(errorCode))
			.append(std::string("\r\n"))
		);
		if (!doesFileExist((char*)PLAYERLIST_PATH)) {
			myGuiTerminalPrint(std::string("Playerlist is not found, creating a new one...\r\n"));
			createFile((char*) PLAYERLIST_PATH);
		}
		return;
	}
	fseek(file, 0L, SEEK_END);
	fprintf(file, "%s\n", line);
	fclose(file);
}

char* findLineInStringVector(std::vector<std::string> v, char* pattern) {
	size_t sizeVector = v.size();
	size_t indexVector = 0;
	while (indexVector < sizeVector) {
		if (bruteForceFind((char*) v[indexVector].c_str(), pattern)) {
			return copyString((char*) v[indexVector].c_str());
		}
		indexVector++;
	}
	return NULL;
}

std::vector<std::string> stringToLines(char* s) {
	std::vector<std::string> result = {};
	std::stringstream ss(s);
	std::string line;
	std::vector<std::string>::iterator i_vector;
	if (strcmp(s, (char*)"") == 0) { // if empty
		return {""};
	}
	i_vector = result.begin();
	while(std::getline(ss,line,'\n')){
		i_vector = result.insert(i_vector, line);
	}
	return result;
}

std::string fileToString(char* filePath) {
	FILE* file;
	std::string result = "";
	char c;
	if (0 != fopen_s(&file, filePath, "r")) {
		myGuiTerminalPrint(std::string("Error opening the file ").append(std::string(filePath)).append(std::string(" in fileToString\r\n")));
		return "";
	}
	while ((c = getc(file)) != EOF) {
		result.append(std::string(1, c));
	}
	fclose(file);
	return result;
}

char* getLastNameInPlayerlist(char* filePath) {
	char* lastLineInPlayerlist;
	char* lastNameInPlayerlist;
	lastLineInPlayerlist = getLastLineOfFile(filePath);
	lastNameInPlayerlist = extractPlayerNameFromPlayerlistLine(lastLineInPlayerlist);
	free(lastLineInPlayerlist);
	return lastNameInPlayerlist;
}

char* getLastCharacterInPlayerlist(char* filePath) {
	char* lastLineInPlayerlist;
	char* lastCharacterInPlayerlist;
	lastLineInPlayerlist = getLastLineOfFile(filePath);
	lastCharacterInPlayerlist = extractCharacterFromPlayerlistLine(lastLineInPlayerlist);
	free(lastLineInPlayerlist);
	return lastCharacterInPlayerlist;
}

char* getLastLineOfFile(char* filePath) {
	std::string fileString = fileToString((char*)PLAYERLIST_PATH);
	std::string line = stringToLines((char*)fileString.c_str())[0]; // first line is last line...
	return copyString((char*) line.c_str());
}

bool bruteForceFind(char* text, char* pattern) {
	int i_t = 0, i_p = 0; //index
	int n_t = (int)strlen(text);
	int n_p = (int)strlen(pattern);
	while (true) {
		if (i_p == n_p) { //pattern found
			return true;
		}
		else if (i_t == n_t) { //pattern not found
			return false;
		}
		else if (text[i_t] == pattern[i_p]) { //match
			i_t++;
			i_p++;
		}
		else { // mismatch
			i_t = i_t - i_p + 1;
			i_p = 0;
		}
	}
}

int bruteForceFindIndex(char* text, char* pattern) {
	int i_t = 0, i_p = 0; //index
	int n_t = (int)strlen(text);
	int n_p = (int)strlen(pattern);
	while (true) {
		if (i_p == n_p) { //pattern found
			return i_t - i_p;
		}
		else if (i_t == n_t) { //pattern not found
			return -1;
		}
		else if (text[i_t] == pattern[i_p]) { //match
			i_t++;
			i_p++;
		}
		else { // mismatch
			i_t = i_t - i_p + 1;
			i_p = 0;
		}
	}
}

int bruteForceFindIndexAfterIndex(char* text, char* pattern, int startIndex) {
	int i_t = startIndex, i_p = 0; //index
	int n_t = (int)strlen(text);
	int n_p = (int)strlen(pattern);
	while (true) {
		if (i_p == n_p) { //pattern found
			return i_t - i_p;
		}
		else if (i_t == n_t) { //pattern not found
			return -1;
		}
		else if (text[i_t] == pattern[i_p]) { //match
			i_t++;
			i_p++;
		}
		else { // mismatch
			i_t = i_t - i_p + 1;
			i_p = 0;
		}
	}
}

void replaceCommentInLastLineInFile(char* path, char* comment) {
	long indexCommentBegin = writeAfterLastOccurenceOfCharInFile(path, comment, '\t');
	if (indexCommentBegin == -1) {
		return;
	}
	long indexCommentEnd = indexCommentBegin + (long) (strlen(comment) + strlen("\n\r"));
	setEndOfFileAtIndex(path, indexCommentEnd);
}

long writeAfterLastOccurenceOfCharInFile(char* path, char* text, char charToWriteAfter) {
	FILE* file;
	errno_t errorCode;
	if (0 != (errorCode = fopen_s(&file, path, "r+"))) {
		myGuiTerminalPrint(std::string("Error opening a file in writeAfterLastOccurenceOfCharInFile, error code = ").append(std::to_string(errorCode)).append(std::string("\r\n")));
		return -1;
	}
	char c;
	fseek(file, 0, SEEK_END);
	long i = ftell(file);
	while (c = getc(file) != charToWriteAfter && i > 0) {
		i--;
		fseek(file, i, SEEK_SET);
	}
	i++;
	fseek(file, i, SEEK_SET);
	fprintf(file, "%s\n", text);
	fclose(file);
	return i;
}

void setEndOfFileAtIndex(char* path, long index) {
	HANDLE fileHandle = CreateFile((LPCTSTR)TEXT(PLAYERLIST_PATH), GENERIC_WRITE,
		FILE_SHARE_READ, NULL, CREATE_NEW | OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	errno_t errorCode = GetLastError();
	if (errorCode > 0) {
		myGuiTerminalPrint(std::string("Error opening a file in setEndOfFileAtIndex, error code = ").append(std::to_string(errorCode)).append(std::string("\r\n")));
	}
	SetFilePointer(fileHandle, index, 0, FILE_BEGIN);
	SetEndOfFile(fileHandle);
	CloseHandle(fileHandle);
}

LPPICTURE loadImageFromFile(LPCTSTR filePath) {
	// open file
	HANDLE fileHandle = CreateFile(filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	errno_t errorCode = GetLastError();
	if (errorCode > 0) {
		myGuiTerminalPrint(std::string("Error opening a file in drawPictureFromFile, error code = ").append(std::to_string(errorCode)).append(std::string("\r\n")));
		return NULL;
	}

	// get file size
	DWORD fileSize = GetFileSize(fileHandle, NULL);
	if (fileSize <= 0) {
		myGuiTerminalPrint(std::string("Error in drawPictureFromFile, file size is = ").append(std::to_string(fileSize)).append(std::string("\r\n")));
		return NULL;
	}

	// alloc memory based on file size
	HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, fileSize);
	if (memory == NULL) {
		myGuiTerminalPrint(std::string("Error: GlobalAlloc failed in drawPictureFromFile.\r\n"));
		return NULL;
	}

	// get a pointer to the first byte of the allocated memory
	LPVOID data = NULL;
	data = GlobalLock(memory);
	if (data == NULL) {
		myGuiTerminalPrint(std::string("Error: GlobalLock failed in drawPictureFromFile.\r\n"));
		return NULL;
	}

	// read file and store in memory
	DWORD bytesRead = 0;
	BOOL isReadSuccess = ReadFile(fileHandle, data, fileSize, &bytesRead, NULL);
	if (isReadSuccess == FALSE) {
		myGuiTerminalPrint(std::string("Error: ReadFile failed in drawPictureFromFile.\r\n"));
		return NULL;
	}
	GlobalUnlock(memory);
	CloseHandle(fileHandle);

	// create stream from memory
	LPSTREAM stream = NULL;
	HRESULT result = CreateStreamOnHGlobal(memory, TRUE, &stream);
	if (FAILED(result) || stream == NULL) {
		myGuiTerminalPrint(std::string("Error: CreateStreamOnHGlobal failed in drawPictureFromFile.\r\n"));
		return NULL;
	}

	// create picture from stream
	LPPICTURE picture = NULL;
	result = OleLoadPicture(stream, fileSize, FALSE, IID_IPicture, (LPVOID*)&picture);
	if (FAILED(result) || picture == NULL) {
		myGuiTerminalPrint(std::string("Error: OleLoadPicture failed in drawPictureFromFile.\r\n"));
		return NULL;
	}
	stream->Release();
	return picture;
}

bool doesWindowPositionFileExist() {
    char* appdataPath = (char*) malloc(sizeof(char) * 200);
    char* windowPositionFilePath;
	_dupenv_s(&appdataPath, NULL, "APPDATA");
	windowPositionFilePath = myStringCat(appdataPath, (char*) "\\Player-Displayer-Window-Position.txt");
	if (doesFileExist(windowPositionFilePath)) {
		free(windowPositionFilePath);
		free(appdataPath);
		return true;
	}
	else {
		free(windowPositionFilePath);
		free(appdataPath);
		return false;
	}
}

void getWindowSavedPosition(int* xOut, int* yOut) {
    char* appdataPath = (char*) malloc(sizeof(char) * 200);
    char* windowPositionFilePath;
	std::string data;
	_dupenv_s(&appdataPath, NULL, "APPDATA");
	windowPositionFilePath = myStringCat(appdataPath, (char*) "\\Player-Displayer-Window-Position.txt");
	data = fileToString(windowPositionFilePath);
	if (data != "") {
		int firstNewlinePosition = bruteForceFindIndex((char*) data.c_str(), (char*) "\n");
		int secondNewlinePosition = bruteForceFindIndexAfterIndex((char*) data.c_str(), (char*) "\n", firstNewlinePosition);
		*xOut = stoi(data.substr(0, firstNewlinePosition));
		*yOut = stoi(data.substr(firstNewlinePosition, secondNewlinePosition + 1));
	}
	free(windowPositionFilePath);
	free(appdataPath);
}

void createWindowPositionFile() {
    char* appdataPath = (char*) malloc(sizeof(char) * 200);
    char* windowPositionFilePath;
	std::string data;
	_dupenv_s(&appdataPath, NULL, "APPDATA");
	windowPositionFilePath = myStringCat(appdataPath, (char*) "\\Player-Displayer-Window-Position.txt");
	createFile(windowPositionFilePath);
	free(windowPositionFilePath);
	free(appdataPath);
}

void saveWindowPositionInFile() {
	FILE* file;
	errno_t errorCode;
    RECT rect;
    char* appdataPath = (char*) malloc(sizeof(char) * 200);
    char* windowPositionFilePath;
	_dupenv_s(&appdataPath, NULL, "APPDATA");
	windowPositionFilePath = myStringCat(appdataPath, (char*) "\\Player-Displayer-Window-Position.txt");
	if (0 != (errorCode = fopen_s(&file, windowPositionFilePath, "r+"))) {
		/*
		myGuiTerminalPrint(std::string("Error opening the window position file, error code = ")
			.append(std::to_string(errorCode))
			.append(std::string("\r\n"))
		);
		*/
		free(windowPositionFilePath);
		free(appdataPath);
		return;
	}
	if (GetWindowRect(guiWindows.mainWindowHandle, &rect)) {
		fseek(file, 0L, SEEK_SET);
		fprintf(file, "%d\n%d\n", rect.left, rect.top);
	}
	fclose(file);
	free(windowPositionFilePath);
	free(appdataPath);
}


