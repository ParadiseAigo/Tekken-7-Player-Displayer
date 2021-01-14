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
    // aigo this entire function needs testing
    std::string result = "";
    int steamIdBufferSize = 40;
    char* steamIdBuffer = (char*) malloc(steamIdBufferSize * sizeof(char));
    sprintf_s(steamIdBuffer, steamIdBufferSize + 1, "(%I64u)", steamId);
    result.append(playerName);
    result.append("\t\t\t");
    result.append(characterName);
    result.append("\t\t\t");
    result.append(steamIdBuffer);
    result.append("\t\t\t");
    result.append("no comment yet");
    free(steamIdBuffer);
    return copyString((char*) result.c_str());

    /* old extremely ugly (but error free) code
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
    */
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

