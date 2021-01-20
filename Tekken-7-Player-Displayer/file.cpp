#include "player-displayer.h"
#include "pointers.h"

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
    print(std::string("Opening playerlist...\r\n"));
    if (doesFileExist((char*) PLAYERLIST_PATH)) {
		setScreenMode(SCREEN_MODE_WINDOWED);
		ShellExecute(0, 0, TEXT(PLAYERLIST_PATH), 0, 0 , SW_SHOW ); //(const wchar_t*)
    }
    else {
        print(std::string("Playerlist is not found, creating a new one...\r\n"));
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
        print(std::string("Error in createFile: Error creating the file ").append(std::string(filePath)).append(std::string("\r\n")));
    }
    else {
        print(std::string("File \"").append(std::string(filePath)).append(std::string("\" created.\r\n")));
        fclose(file);
    }
}

char* saveNewOpponentInPlayerlist(char* playerName, char* currentOpponentName, char* currentLoadedOpponentName) {
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
	opponentStructCharacterPointer = (void*)getDynamicPointer(tekkenHandle, (void*) OPPONENT_STRUCT_CHARACTER_STATIC_POINTER, OPPONENT_STRUCT_CHARACTER_POINTER_OFFSETS);
	newOpponentStructCharacter = readDwordFromMemory(tekkenHandle, opponentStructCharacterPointer);
    steamId = lastFoundSteamId;
    print(std::string("New opponent    : ").append(std::string(currentLoadedOpponentName)).append(std::string("\r\n")));
    print(std::string("Character id    : ").append(std::to_string(newOpponentStructCharacter)).append(std::string("\r\n")));
    print(std::string("Steam id        : ").append(std::to_string(steamId)).append(std::string("\r\n")));
    if (newOpponentStructCharacter != 255) {
        print(std::string("Character name:   ").append(allCharacters[newOpponentStructCharacter]).append(std::string("\r\n")));
        newPlayerlistLine = makePlayerlistEntry(currentLoadedOpponentName, (char*) allCharacters[newOpponentStructCharacter].c_str(), steamId);
        print(std::string("Playerlist entry: ").append(std::string(newPlayerlistLine)).append(std::string("\r\n")));
        writeLineToFile((char*)PLAYERLIST_PATH, newPlayerlistLine);
		free(newPlayerlistLine);
    }
}

char* makePlayerlistEntry(char* playerName, char* characterName, QWORD steamId) {
    // aigo this entire function needs testing
    std::string result = "";
    int steamIdBufferSize = 100;
    char* steamIdBuffer = (char*) malloc(steamIdBufferSize * sizeof(char));
    sprintf_s(steamIdBuffer, steamIdBufferSize, "(%I64u)", steamId);
    result.append(playerName);
    result.append(PLAYERLIST_COLUMN_DELIMITER);
    result.append(characterName);
    result.append(PLAYERLIST_COLUMN_DELIMITER);
    result.append(steamIdBuffer);
    result.append(PLAYERLIST_COLUMN_DELIMITER);
    result.append(DEFAULT_COMMENT);
    free(steamIdBuffer);
    return copyString((char*) result.c_str());

    /* old extremely ugly (but error free) code
    char* temp1;
    char* temp2;
    int steamIdBufferSize = 40;
    char* steamIdBuffer = (char*) malloc(steamIdBufferSize * sizeof(char));
    sprintf_s(steamIdBuffer, steamIdBufferSize + 1, "(%I64u)", steamId);
    temp1 = myStringCat(playerName, (char*)PLAYERLIST_COLUMN_DELIMITER);
    temp2 = myStringCat(temp1, characterName);
    free(temp1);
    temp1 = myStringCat(temp2, (char*)PLAYERLIST_COLUMN_DELIMITER);
    free(temp2);
    temp2 = myStringCat(temp1, steamIdBuffer);
    free(temp1);
    temp1 = myStringCat(temp2, (char*)"\t\t\tno comment yet");
    free(temp2);
    free(steamIdBuffer);
    return temp1;
    */
}

void writeLineToFile(char* path, char* line) {
    FILE* file;
    errno_t errorCode;
    if (0 != (errorCode = fopen_s(&file, path, "a+"))) {
        print(std::string("Error opening a file in writeLineToFile, error code = ").append(std::to_string(errorCode)).append(std::string("\r\n")));
        return;
    }
	fseek(file, 0L, SEEK_END);
    fprintf(file, "%s\n", line);
    fclose(file);
}

void replaceCommentInLastLineInFile(char* path, char* comment) {
    FILE* file;
    errno_t errorCode;
    if (0 != (errorCode = fopen_s(&file, path, "r+"))) {
        print(std::string("Error opening a file in replaceCommentInLastLineInFile, error code = ").append(std::to_string(errorCode)).append(std::string("\r\n")));
        return;
    }
    char c;
    fseek(file, 0, SEEK_END);
    long i = ftell(file);
    while (c = getc(file) != '\t') {
        i--;
        fseek(file, i, SEEK_SET);
    }
    i++;
    fseek(file, i, SEEK_SET);
    fprintf(file, "%s\n", comment);
    fclose(file);

    long indexEndOfComment = i + strlen(comment) + strlen("\n\r");
    setEndOfFileAtIndex(path, indexEndOfComment);
}

void setEndOfFileAtIndex(char* path, long index) {
    HANDLE fileHandle = CreateFile((LPCTSTR)TEXT(PLAYERLIST_PATH), GENERIC_WRITE,
        FILE_SHARE_READ, NULL, CREATE_NEW | OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    errno_t errorCode = GetLastError();
    if (errorCode > 0) {
        print(std::string("Error opening a file in setEndOfFileAtIndex, error code = ").append(std::to_string(errorCode)).append(std::string("\r\n")));
    }
    SetFilePointer(fileHandle, index, 0, FILE_BEGIN);
    SetEndOfFile(fileHandle);
    CloseHandle(fileHandle);
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
        print(std::string("Error opening the file ").append(std::string(filePath)).append(std::string(" in fileToString\r\n")));
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

