#ifndef TEKKEN7_PLAYERDISPLAYER_H
#define TEKKEN7_PLAYERDISPLAYER_H

#include <iostream>
#include <windows.h>
#include <string>
#include <sstream>
#include <tlhelp32.h>
#include <fstream>
#include <vector>

#define PLAYERLIST_PATH "Tekken Player List.txt"

typedef __int64 QWORD;

enum tekkenState {IN_FIGHT, IN_SEARCH};

extern HANDLE tekkenHandle;
extern HWND tekkenWindowHandle;
extern int tekkenPid;
extern QWORD lastFoundSteamId;

extern void* fightThisPlayerMessagePointer;
extern void* secondsRemainingMessagePointer;
extern void* opponentFoundMessagePointer;
extern void* opponentNamePointer;
extern void* screenModePointer;
extern void* steamIdPointer;

//main.cpp
void init();
void initTekkenHandle();
void initTekkenWindowHandle();
void initPointers();
void mainLoop();
void closeProgram();

//tekken.cpp
bool isGameLoaded();
bool isNewOpponentReceived(char* playerName, char* currentOpponentName);
bool isOpponentSteamIdValid();
char* handleNewOpponent(char* currentOpponentName);
void updateOpponentFoundMessage(char* message);
void updateFightThisPlayerMessage(char* message);
void updateSecondsRemainingMessage(char* message);
void updateMessagesWithoutSteamId();
bool isNewFightAccepted(char* playerName, char* currentOpponentName, char* currentLoadedOpponentName);
bool isNewOpponentLoaded(char* playerName, char* currentOpponentName, char* currentLoadedOpponentName);
bool isTimeToCleanMessages(char* playerName, char* currentOpponentName);
void cleanAllProcessMessages();
char* getNewCurrentLoadedOpponent(char* currentLoadedOpponentName);

//input.cpp
void handleInput();

//file.cpp
char* extractCommentFromPlayerlistLine(char* line);
char* extractCharacterFromPlayerlistLine(char* line);
char* myStringCat(char* s1, char* s2);
char* copyString(char* s);
void openPlayerlist();
bool doesFileExist(char* filePath);
void createFile(char* filePath);
char* saveNewOpponentInPlayerlist(char* playerName, char* currentOpponentName, char* currentLoadedOpponentName);
void saveNewPlayerlistEntry(char* currentLoadedOpponentName);
char* makePlayerlistEntry(char* playerName, char* characterName, QWORD steamId);
void writeLineToFile(char* path, char* line);
char* findLineInStringVector(std::vector<std::string> v, char* pattern);
std::vector<std::string> stringToLines(char* s);
std::string fileToString(char* filePath);
bool bruteForceFind(char* text, char* pattern);

//memory.cpp
HANDLE getProcessHandle(DWORD pid);
DWORD getProcessId(const std::wstring& nameProgramExe);
QWORD getDynamicPointer(HANDLE processHandle, void* basePointer, std::vector<DWORD> offsets);
void writeDwordToMemory(HANDLE processHandle, void* address, DWORD newValue);
void writeStringLimitedToMemory(HANDLE processHandle, void* address, char* newValue);
void writeStringUnlimitedToMemory(HANDLE processHandle, void* address, char* newValue);
void writeStringSafeToMemory(HANDLE processHandle, void* address, char* newValue);
bool isMemoryReadable(HANDLE processHandle, void* address);
DWORD readDwordFromMemory(HANDLE processHandle, void* address);
QWORD readQwordFromMemory(HANDLE processHandle, void* address);
char* readStringFromMemory(HANDLE processHandle, void* address);
int getSizeStringInMemory(HANDLE processHandle, void* address);
int getMaxSizeStringInMemory(HANDLE processHandle, void* address);

//window.cpp
HWND getWindowHandle(const wchar_t* programWindowName);
void setScreenMode(DWORD screenMode);

#endif

