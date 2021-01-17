#ifndef TEKKEN7_PLAYERDISPLAYER_H
#define TEKKEN7_PLAYERDISPLAYER_H

#include <iostream>
#include <windows.h>
#include <string>
#include <sstream>
#include <tlhelp32.h>
#include <fstream>
#include <vector>
#include <map>
#include <process.h> // _beginthread
#include "resource.h" 

#define PLAYERLIST_PATH "Tekken Player List.txt"

#define TITLE_MAINWINDOW "Tekken 7 - Player Displayer"
#define TITLE_COMMENTWINDOW "Comment"

#define CLASSNAME_MAINWINDOW "Main Window"
#define CLASSNAME_COMMENTWINDOW "Comment Window"

#define WIDTH_MAINWINDOW 640
#define HEIGHT_MAINWINDOW 438
#define WIDTH_COMMENTWINDOW 300
#define HEIGHT_COMMENTWINDOW 140

#define WINDOW_OPACITY 90

#define TEXT_WELCOME "Hello! ^^ / This program will display your opponent's name in-game before accepting the match. It also keeps a list of your opponent's name, steamId and character. Have fun! :)"
#define TEXT_INFORMATION " Extra Shortcuts : \n         ALT + F : Set Tekken in Fullscreen Mode\n         ALT + W : Set Tekken in Windowed Mode\n         ALT + C : Add a Comment in the Player List\n         ALT + O : Open the Player List"//\n         ALT + T : Show Program Console"
#define TEXT_CREDITS " github.com/ParadiseAigo"
#define TEXT_COMMENTWINDOW "(Last fought player not found, maybe you didn't fight a player yet)"

#define FONT_SIZE 16
#define EDITBOX_TEXT_MAX_LENGTH 256
#define PLAYERLIST_COLUMN_DELIMITER "\t\t\t"

#define COLOR_BLACK RGB(0, 0, 0)
#define COLOR_WHITE RGB(255, 255, 255)
#define COLOR_PINK RGB(255, 228, 225)

#define ALT_W 1
#define ALT_F 2
#define ALT_C 3
#define ALT_O 4
#define ALT_T 5
#define NR_OF_HOTKEYS 5

#define KEY_W 0x57
#define KEY_F 0x46
#define KEY_C 0x43
#define KEY_O 0x4F
#define KEY_T 0x54

#define NR_OF_THREADS 2

typedef __int64 QWORD;

typedef struct Windows {
    HWND mainWindowHandle;
    HWND outputTextHandle;
    HWND commentWindowHandle;
    HWND commentEditboxHandle;
    HWND commentButtonHandle;
    HWND consoleWindowHandle;
} Windows;

typedef struct Fonts {
    HFONT welcomeTextFont;
    HFONT outputTextFont;    
    HFONT informationTextFont;
    HFONT creditsTextFont;
    HFONT editboxTextFont;
    HFONT editboxReadOnlyTextFont;
} Fonts;

typedef struct Hotkey {
    int id;
    UINT key1;
    UINT key2;

    Hotkey(int idValue, UINT key1Value, UINT key2Value)
        : id(idValue), key1(key1Value), key2(key2Value) {}
} Hotkey;

enum tekkenState {IN_FIGHT, IN_SEARCH};

extern HANDLE tekkenHandle;
extern HWND tekkenWindowHandle;
extern int tekkenPid;
extern QWORD lastFoundSteamId;
extern char* lastFoughtOpponentName;
extern char* lastFoughtOpponentLineInFile;

extern void* fightThisPlayerMessagePointer;
extern void* secondsRemainingMessagePointer;
extern void* opponentFoundMessagePointer;
extern void* opponentNamePointer;
extern void* screenModePointer;
extern void* steamIdPointer;

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

extern Windows windows;
extern Fonts fonts;

extern HBRUSH solidBrush;
extern WNDPROC defaultEditProc;

extern Hotkey* hotkeys[NR_OF_HOTKEYS];
extern HANDLE threadHandles[NR_OF_THREADS];
extern bool continueThreads;

//main.cpp
void createThreads();
HANDLE beginThread(_beginthreadex_proc_type proc);
void endThread();
void waitForThreadsToTerminate();
void closeThreads();

unsigned __stdcall readAndUpdateTekkenMemory(void* arguments);
unsigned __stdcall createWindowAndHandleInput(void* arguments);

//update.cpp
void init();
void initTekkenHandle();
void initTekkenWindowHandle();
void initPointers();
void mainLoop();

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
void handleHotkeyInput(WPARAM hotkey);
void initHotkeys();
void registerHotKeys();
void registerHotKey(HWND windowHandle, int hotkeyId, UINT key1, UINT key2);
void unRegisterHotKeys();
void unRegisterHotKey(HWND windowHandle, int hotkeyId);

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
void initWindowsAndHotkeys();
void initWindows();
void registerMainWindowClass();
void registerCommentWindowClass();
void registerWindowClass(WNDCLASS windowClass);
void getConsoleWindowHandle();
HWND getWindowHandle(const wchar_t* programWindowName);
void getScreenResolution();
void initFontsAndBrushes();
void createMainWindow();
void createCommentWindow();
HWND createWindow(DWORD extendedStyle, LPCWSTR className, LPCWSTR windowName, DWORD style,
    int x, int y, int width, int height, HWND parentWindowHandle = NULL);
void showWindow(HWND windowHandle, int showCommand);
void setFocus(HWND windowHandle);
void sendMessage(HWND windowHandle, UINT msg, WPARAM wparam, LPARAM lparam);
void handleWindowsMessageQueueLoop();
LRESULT CALLBACK mainWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK commentWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK subEditProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
void openCommentWindow();
bool isWindow(HWND windowHandle);
boolean isLastFoughtOpponentFound();
void setOpponentNameInCommentWindowTitle();
void disableCommentWindowEditbox();
void saveCommentAndCloseCommentWindow();
char* getTextFromCommentEditbox();
void writeCommentToFile(void* text);
std::string setCommentInLine(char* line, char* comment);
void setTekkenWindowed();
void setTekkenFullscreen();
void setScreenMode(DWORD screenMode);
void minimizeAndRestoreTekkenWindow();
void showOrHideConsoleWindow();
BOOL isWindowVisible(HWND windowHandle);
void print(std::string text);
void printToStandardOutput(std::string text);
void printToTextboxOutput(std::string text);
wchar_t* stringToWString(std::string text);
void closeAllWindows();
void closeCommentWindow();
void destroyWindow(HWND windowHandle);
void deleteFontObjects();
void closeProgram();

#endif

