#include "main.h"
#include "pointers.h"

//TODO:
//-> mode inFight (low fps) inSearch (high fps)
//-> display new found name before trying to check the steam id (because of the read mem error)

HANDLE tekkenHandle;
HWND tekkenWindowHandle;
int tekkenPid;
QWORD lastFoundSteamId;

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
    tekkenPid = pid;
    std::cout << "Tekken found! pid = (" << pid << ")" << std::endl;
    std::cout << "Opening Tekken process..." << std::endl;
    tekkenHandle = getProcessHandle(pid);
    std::cout << "Opening Tekken process success!" << std::endl;
}

void initTekkenWindowHandle() {
    DWORD secondsDelay;
	secondsDelay = 2;
    while ( (tekkenWindowHandle = getWindowHandle(TEXT(TEKKEN_WINDOW_NAME))) == NULL ) {
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

bool isGameLoaded() {
    // will assume game is loaded if the opponent name address contains a string
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

HANDLE getProcessHandle(DWORD pid) {
    HANDLE processHandle;
    DWORD errorCode;
    processHandle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, pid);
    if (processHandle == NULL) {
        errorCode = GetLastError();
        printf("Error! OpenProcess failed! Code: %d. Closing program.\n", errorCode);
		system("PAUSE");
        exit(0);
    }
    return processHandle;
}

DWORD getProcessId(const std::wstring& programNameExe) {
    HANDLE processesSnapshot;   //will hold a snapshot of all processes...
    PROCESSENTRY32 processInfo; //this struct will contain the process id
    DWORD errorCode;
    ZeroMemory(&processInfo, sizeof(PROCESSENTRY32)); // set all the struct's members equal to zero
    processInfo.dwSize = sizeof(processInfo); // must set the size before using it / filling it
    processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL); // get snapshot of all processes
    if (processesSnapshot == INVALID_HANDLE_VALUE) {
        errorCode = GetLastError();
        printf("Error! CreateToolhelp32Snapshot failed! Code: %d. Closing program.\n", errorCode);
		system("PAUSE");
        exit(0);
    }
    Process32First(processesSnapshot, &processInfo);
    if (!programNameExe.compare(processInfo.szExeFile)) { //if names are equal
        CloseHandle(processesSnapshot);
        return processInfo.th32ProcessID;
    }
    while (Process32Next(processesSnapshot, &processInfo)) {
        if (!programNameExe.compare(processInfo.szExeFile)) {
            CloseHandle(processesSnapshot);
            return processInfo.th32ProcessID;
        }
    }
    CloseHandle(processesSnapshot);
    return 0;
}

HWND getWindowHandle(const wchar_t* programWindowName) {  //change the argument type to  (const wchar_t*)
    HWND windowHandle;
    DWORD errorCode;
    windowHandle = FindWindow(NULL, programWindowName);
    if (windowHandle == NULL) {
        errorCode = GetLastError();
		//printf("Error! FindWindow in getWindowHandle failed! Code: %d.\n", errorCode);
    }
    return windowHandle;
}

void mainLoop() {
	char* playerName = readStringFromMemory(tekkenHandle, opponentNamePointer);
	char* currentOpponentName = readStringFromMemory(tekkenHandle, opponentNamePointer);
	char* currentLoadedOpponentName = (char*) malloc(10 * sizeof(char)); // dummy value
    currentLoadedOpponentName[0] = '\0'; // empty string
    while (true) {
        Sleep(1000/20); // ("20fps")
        handleInput();
		if (isTimeToCleanMessages(playerName, currentOpponentName)) {
			cleanAllProcessMessages();
            currentOpponentName[0] = '\0';
			printf("Ready to find the next opponent.\n");
		}
        currentOpponentName = updateOpponentFoundMessage(playerName, currentOpponentName);
        currentLoadedOpponentName = saveNewOpponentInPlayerlist(playerName, currentOpponentName, currentLoadedOpponentName);
    }
    // program never reaches this comment
}

void handleInput() {
	DWORD currentPid = GetCurrentProcessId();
	int KEY_W = 0x57;
	int KEY_F = 0x46;
	int KEY_O = 0x4F;
    int delay = 1500;
	AttachThreadInput(currentPid, tekkenPid, true); //attach
	if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(KEY_W) ) {  //if CTRL+W pressed
        setScreenMode(SCREEN_MODE_WINDOWED);
		Sleep(delay); //avoid unintended multiple presses
	}
	if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(KEY_F) ) {  //if CTRL+F pressed
        setScreenMode(SCREEN_MODE_FULLSCREEN);
		Sleep(delay); //avoid unintended multiple presses
	}
    if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(KEY_O)) {  //if CTRL+O pressed
        setScreenMode(SCREEN_MODE_WINDOWED);
        openPlayerlist();
		Sleep(delay); //avoid unintended multiple presses
    }
	AttachThreadInput(currentPid, tekkenPid, false); //detach
}

char* updateOpponentFoundMessage(char* playerName, char* currentOpponentName) {
    char* newOpponentName;
    if (isNewOpponentReceived(playerName, currentOpponentName)) {
		newOpponentName = readStringFromMemory(tekkenHandle, opponentNamePointer);
        printf("New opponent found:  %s\n", newOpponentName);
		//system("pause");
		writeStringUnlimitedToMemory(tekkenHandle, opponentFoundMessagePointer, newOpponentName);
        updateFightThisPlayerMessage();
		free(currentOpponentName);
        return newOpponentName; // calling function should set currentOpponentName equal to newOpponentName
    }
    return currentOpponentName; // currentOpponentName remains unchanged
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
    if (!isSteamIdValid()) {
		free(newOpponentName);
        return false;
    }
    free(newOpponentName);
    return true;
}

bool isSteamIdValid() {
    QWORD steamId;
	steamIdPointer = (void*)getDynamicPointer(tekkenHandle, (void*)STEAM_ID_STATIC_POINTER, STEAM_ID_POINTER_OFFSETS);
    if (! isMemoryReadable(tekkenHandle, steamIdPointer)) {
        printf("Steam Id (memory) not readable, trying again...\n");
        return false;
    }
    steamId = readQwordFromMemory(tekkenHandle, steamIdPointer);
    if (steamId < 0x0110000100000000 || steamId > 0x0110000200000000) {
        printf("Steam Id is bad!\n");
        // steam id is 64 bits (8 bytes) long: 0x FF FF FF FF FF FF FF FF
        // first 8 bits represent the "universe" and its always equal to 1 for normal accounts
        // next 4 bits represent the "type" and its also always equal to 1 for normal accounts
        // next 20 bits represent the "instance" and its also always equal to 1 for normal accounts
        // final 32 bits (4 bytes) represent the real steam id
        // so a full steam id in hex looks like this (with X variable)  0x 01 10 00 01 XX XX XX XX
        return false;
    }
    lastFoundSteamId = steamId;
    return true;
}

void updateFightThisPlayerMessage() {
    QWORD steamId;
    size_t steamIdBufferSize;
    char* steamIdBuffer;
    std::string fileString;
    char* line;
    char* playerlistComment;
    char* characterNameMessage;
    char* characterName;
    steamIdBufferSize = 100;
    steamIdBuffer = (char*)malloc(steamIdBufferSize * sizeof(char));
    steamId = lastFoundSteamId;
    sprintf_s(steamIdBuffer, steamIdBufferSize + 1, "(%I64u)", steamId);
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
	writeStringUnlimitedToMemory(tekkenHandle, fightThisPlayerMessagePointer, playerlistComment);
    updateSecondsRemainingMessage(characterNameMessage);
    if (characterName == NULL) {
        printf("Last used character: /\n");
    }
    else {
		printf("Last used character: %s\n", characterName);
    }
    printf("Comment:             %s\n", playerlistComment);
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
}

void updateSecondsRemainingMessage(char* message) {
	writeStringUnlimitedToMemory(tekkenHandle, secondsRemainingMessagePointer, message);
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

void setScreenMode(DWORD newScreenMode) {
    DWORD screenMode;
    std::string screenModeInEnglish;
	switch (newScreenMode) {
	case SCREEN_MODE_FULLSCREEN:
		screenModeInEnglish = "fullscreen";
		break;
	case SCREEN_MODE_BORDERLESS:
		screenModeInEnglish = "borderless";
		break;
	case SCREEN_MODE_WINDOWED:
		screenModeInEnglish = "windowed";
		break;
	default:
		screenModeInEnglish = "unknown";
		break;
	}
    screenMode = readDwordFromMemory(tekkenHandle, screenModePointer);
    if (screenMode != newScreenMode) {
        writeDwordToMemory(tekkenHandle, screenModePointer, newScreenMode);
		ShowWindow(tekkenWindowHandle, SW_MINIMIZE);
        ShowWindow(tekkenWindowHandle, SW_RESTORE);
        std::cout << "Screen mode set to = " << screenModeInEnglish << std::endl;
    }
}

void openPlayerlist() {
    std::cout << "Opening playerlist..." << std::endl;
    if (doesFileExist((char*) PLAYERLIST_PATH)) {
		setScreenMode(SCREEN_MODE_WINDOWED);
		ShellExecute(0, 0, TEXT(PLAYERLIST_PATH), 0, 0 , SW_SHOW ); //(const wchar_t*)
    }
    else {
        std::cout << "Playerlist is not found, creating a new one..." << std::endl;
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
        printf("Error in createFile: Error creating the file %s\n", filePath);
    }
    else {
        printf("File \"%s\" created.\n", filePath);
        fclose(file);
    }
}

char* saveNewOpponentInPlayerlist(char* playerName, char* currentOpponentName, char* currentLoadedOpponentName) {
    if (isNewOpponentLoaded(playerName, currentOpponentName, currentLoadedOpponentName)) {
        currentLoadedOpponentName = getNewCurrentLoadedOpponent(currentLoadedOpponentName);
        saveNewPlayerlistEntry(currentLoadedOpponentName);
    }
    return currentLoadedOpponentName;
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
        free(opponentStructName);                               // this is the name of the previous loaded person
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

void saveNewPlayerlistEntry(char* currentLoadedOpponentName) {
    void* opponentStructCharacterPointer;
    DWORD newOpponentStructCharacter;
    char* newPlayerlistLine;
    QWORD steamId;
    std::string allCharacters[] = ALL_CHARACTERS;
	opponentStructCharacterPointer = (void*)getDynamicPointer(tekkenHandle, (void*) OPPONENT_STRUCT_CHARACTER_STATIC_POINTER, OPPONENT_STRUCT_CHARACTER_POINTER_OFFSETS);
	newOpponentStructCharacter = readDwordFromMemory(tekkenHandle, opponentStructCharacterPointer);
    steamId = lastFoundSteamId;
	std::cout << "New opponent    : " << currentLoadedOpponentName << std::endl;
    std::cout << "Character id    : " << newOpponentStructCharacter << std::endl;
    std::cout << "Steam id        : " << steamId << std::endl;
    if (newOpponentStructCharacter != 255) {
        std::cout << "Character name:   " << allCharacters[newOpponentStructCharacter].c_str() << std::endl;
        newPlayerlistLine = makePlayerlistEntry(currentLoadedOpponentName, (char*) allCharacters[newOpponentStructCharacter].c_str(), steamId);
        std::cout << "Playerlist entry: " << newPlayerlistLine << std::endl;
        writeLineToFile((char*)PLAYERLIST_PATH, newPlayerlistLine);
		free(newPlayerlistLine);
    }
}

char* makePlayerlistEntry(char* playerName, char* characterName, QWORD steamId) {
    char* temp1;
    char* temp2;
    int steamIdBufferSize = 40;
    char* steamIdBuffer = (char*) malloc(steamIdBufferSize * sizeof(char));
    sprintf_s(steamIdBuffer, steamIdBufferSize + 1, "(%I64u)", steamId);
    temp1 = myStringCat(playerName, (char*)"\t\t\t");
    temp2 = myStringCat(temp1, characterName);
    free(temp1);
    temp1 = myStringCat(temp2, (char*)"\t\t\t");
    free(temp2);
    temp2 = myStringCat(temp1, steamIdBuffer);
    free(temp1);
    temp1 = myStringCat(temp2, (char*)"\t\t\tno comment yet");
    free(temp2);
    free(steamIdBuffer);
    return temp1;
}

void writeLineToFile(char* path, char* line) {
    FILE* file;
    errno_t errorCode;
    if (0 != (errorCode = fopen_s(&file, path, "r+"))) {
        printf("Error opening a file in writeLineToFile, error code = %d\n", errorCode);
        //system("PAUSE");
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
        printf("Error opening the file %s in fileToString\n", filePath);
        return "";
    }
    while ((c = getc(file)) != EOF) {
		result.append(std::string(1, c));
    }
    fclose(file);
    return result;
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

void closeProgram() {
	CloseHandle(tekkenHandle);
    CloseHandle(tekkenWindowHandle);
    system("PAUSE");
}

QWORD getDynamicPointer(HANDLE processHandle, void* basePointer, std::vector<DWORD> offsets) {
    QWORD resultPointer = 0, prevPointer;
    DWORD errorCode;
    if (offsets.size() == 0) {
        return (QWORD) basePointer;
    }
    prevPointer = (QWORD) basePointer;
    for (int i = 0; i < offsets.size(); i++) {
        if (ReadProcessMemory(processHandle, (void*) prevPointer, &resultPointer, sizeof(resultPointer), NULL) == 0) {
			errorCode = GetLastError();
			//printf("Error! ReadProcessMemory in getDynamicPointer failed! Code: %d.\n", errorCode);
			//system("PAUSE");
            return 0;
        }
        prevPointer = resultPointer + offsets[i];
    }
    resultPointer += offsets[offsets.size() - 1];
    return resultPointer;
}

void writeDwordToMemory(HANDLE processHandle, void* address, DWORD newValue) {
    // void* address  holds values that look like this:  (void*) 0x03AFFE76
    DWORD errorCode;
    errorCode = WriteProcessMemory(processHandle, address, &newValue, sizeof(newValue), NULL);
    if (errorCode == 0) {
		errorCode = GetLastError();
		printf("Error! WriteProcessMemory failed in WriteDwordToMemory. Code: %d .\n", errorCode);
        system("PAUSE");
    }
}

void writeStringLimitedToMemory(HANDLE processHandle, void* address, char* newValue) {
    DWORD errorCode;
    int sizeAddress = getMaxSizeStringInMemory(processHandle, address) + 1; //to "undo" the -1 in strncpy_s
    int sizeNewValue = (int) strlen(newValue);
    int sizeNewValuePadded = sizeAddress;
    char* newValuePadded = (char*) malloc(sizeNewValuePadded * sizeof(char));
    memset(newValuePadded, 0, sizeNewValuePadded);
    strncpy_s(newValuePadded, sizeNewValuePadded, newValue, sizeNewValuePadded - 1); // 4th argument must be smaller than the 2nd
    errorCode = WriteProcessMemory(processHandle, address, newValuePadded, sizeNewValuePadded, NULL);
    if (errorCode == 0) {
		errorCode = GetLastError();
		printf("Error! WriteProcessMemory failed in writeStringLimitedToMemory. Code: %d .\n", errorCode);
        system("PAUSE");
    }
    free(newValuePadded);
}

void writeStringUnlimitedToMemory(HANDLE processHandle, void* address, char* newValue) {
    DWORD errorCode;
    int sizeNewValue = (int) strlen(newValue);
    errorCode = WriteProcessMemory(processHandle, address, newValue, sizeNewValue + 1, NULL);
    if (errorCode == 0) {
		errorCode = GetLastError();
		printf("Error! WriteProcessMemory failed in writeStringUnlimitedToMemory. Code: %d .\n", errorCode);
        system("PAUSE");
    }
}

void writeStringSafeToMemory(HANDLE processHandle, void* address, char* newValue) {
    DWORD errorCode;
    int sizeAddress = getSizeStringInMemory(processHandle, address);
    int sizeNewValue = (int) strlen(newValue);
    int sizeNewValuePadded = sizeAddress;
    char* newValuePadded = (char*) malloc(sizeNewValuePadded * sizeof(char));
    memset(newValuePadded, 0, sizeNewValuePadded);
    strncpy_s(newValuePadded, sizeNewValuePadded, newValue, sizeNewValuePadded - 1); // 4th argument must be smaller than the 2nd
    errorCode = WriteProcessMemory(processHandle, address, newValuePadded, sizeNewValuePadded, NULL);
    if (errorCode == 0) {
		errorCode = GetLastError();
		printf("Error! WriteProcessMemory failed in writeStringSafeToMemory. Code: %d .\n", errorCode);
        system("PAUSE");
    }
    free(newValuePadded);
}

bool isMemoryReadable(HANDLE processHandle, void* address) {
    DWORD buffer = 0, errorCode;
    errorCode = ReadProcessMemory(processHandle, address, &buffer, sizeof(buffer), NULL);
    if (errorCode == 0) {
        return false;
    }
    else {
        return true;
    }
}

DWORD readDwordFromMemory(HANDLE processHandle, void* address) {
    DWORD result, errorCode;
    errorCode = ReadProcessMemory(processHandle, address, &result, sizeof(result), NULL);
    if (errorCode == 0) {
		errorCode = GetLastError();
		// make sure this is compiled into 64bit or you'll get the 299 error: trying to read x64 with x86 exe
		printf("Error! ReadProcessMemory failed in readDwordFromMemory. Code: %d .\n", errorCode);
        system("PAUSE");
    }
    return result;
}

QWORD readQwordFromMemory(HANDLE processHandle, void* address) {
    QWORD result;
	DWORD errorCode;
    errorCode = ReadProcessMemory(processHandle, address, &result, sizeof(result), NULL);
    if (errorCode == 0) {
		errorCode = GetLastError();
		// make sure this is compiled into 64bit or you'll get the 299 error: trying to read x64 with x86 exe
		printf("Error! ReadProcessMemory failed in readQwordFromMemory. Code: %d .\n", errorCode);
        system("PAUSE");
    }
    return result;
}

char* readStringFromMemory(HANDLE processHandle, void* address) {
    char* result;
    int sizeResult;
    DWORD errorCode;
    sizeResult = 50;
    result = (char*) malloc(sizeResult * sizeof(char));
    errorCode = ReadProcessMemory(processHandle, address, result, sizeResult, NULL);
    if (errorCode == 0) {
		errorCode = GetLastError();
		printf("Error! ReadProcessMemory failed in readStringFromMemory. Code: %d .\n", errorCode);
        system("PAUSE");
    }
    return result;
}

int getSizeStringInMemory(HANDLE processHandle, void* address) {
    int count;
    BYTE byteBuffer;
    DWORD errorCode;
    QWORD addressPlusOffset;
    count = 0;
    do {
        addressPlusOffset = (QWORD)address + (QWORD)(count * sizeof(BYTE));
        errorCode = ReadProcessMemory(processHandle, (void*)addressPlusOffset, &byteBuffer, sizeof(BYTE), NULL);
        if (errorCode == 0) {
            errorCode = GetLastError();
            printf("Error! ReadProcessMemory failed in getSizeStringInMemory. Code: %d .\n", errorCode);
            system("PAUSE");
        }
        count++;

    } while (byteBuffer != '\0');
    return count;
}

int getMaxSizeStringInMemory(HANDLE processHandle, void* address) {
    int count;
    BYTE byteBuffer;
    DWORD errorCode;
    QWORD addressPlusOffset;
    bool pastZero = false;
    count = 0;
    do {
        addressPlusOffset = (QWORD)address + (QWORD)(count * sizeof(BYTE));
        errorCode = ReadProcessMemory(processHandle, (void*)addressPlusOffset, &byteBuffer, sizeof(BYTE), NULL);
        if (errorCode == 0) {
            errorCode = GetLastError();
            printf("Error! ReadProcessMemory failed in getSizeStringInMemory. Code: %d .\n", errorCode);
            system("PAUSE");
        }
        count++;
        if (byteBuffer == '\0') {
            pastZero = true;
        }
    } while (pastZero == false || byteBuffer == '\0');
	return count - 2;
}
