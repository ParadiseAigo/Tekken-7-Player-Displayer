#ifndef TEKKEN7_PLAYERDISPLAYER_H
#define TEKKEN7_PLAYERDISPLAYER_H

#include <iostream>
#include <windows.h>
#include <string>
#include <sstream>
#include <tlhelp32.h>
#include <fstream>
#include <vector>
#include <process.h> // _beginthread
#include <urlmon.h>  // to download a website
#include "resource.h" 

#pragma comment(linker,"/manifestdependency:\"type='win32' "\
                   "name='Microsoft.Windows.Common-Controls' "\
                   "version='6.0.0.0' "\
                   "processorArchitecture='*' "\
                   "publicKeyToken='6595b64144ccf1df' "\
                   "language='*' "\
                   "\"") 

#pragma comment(lib, "urlmon.lib") // to download a website

#define PLAYERLIST_PATH "Tekken Player List.txt"

#define TITLE_MAINWINDOW "Tekken 7 - Player Displayer"
#define TITLE_COMMENTWINDOW "Comment"

#define CLASSNAME_MAINWINDOW "Main Window"
#define CLASSNAME_COMMENTWINDOW "Comment Window"

#define WIDTH_MAINWINDOW 1080
#define HEIGHT_MAINWINDOW 720
#define WIDTH_COMMENTWINDOW 300
#define HEIGHT_COMMENTWINDOW 140

#define WINDOW_OPACITY 90

#define TEXT_INFORMATION " ALT + F : Set Tekken in Fullscreen Mode\n ALT + W : Set Tekken in Windowed Mode\n ALT + C : Add a Comment in the Player List\n ALT + O : Open the Player List"//\n ALT + T : Show Program Console"
#define TEXT_COMMENTWINDOW "(Last fought player not found, maybe you didn't fight a player yet)"

#define FONT_SIZE 16
#define EDITBOX_TEXT_MAX_LENGTH 256

#define COLOR_BLACK RGB(0, 0, 0)
#define COLOR_WHITE RGB(255, 255, 255)
#define COLOR_GRAY RGB(200, 200, 200)

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

typedef __int64 QWORD;

typedef struct GuiWindows {
    HWND mainWindowHandle;
    HWND outputTextHandle;
    HWND playerNameValueTextHandle;
    HWND opponentNameValueTextHandle;
    HWND opponentCharacterValueTextHandle;
    HWND commentValueTextHandle;
    HWND commentWindowHandle;
    HWND commentEditboxHandle;
    HWND commentButtonHandle;
    HWND consoleWindowHandle;
} GuiWindows;

typedef struct GuiFonts {
    HFONT informationNameTextFont;
    HFONT informationValueTextFont;
    HFONT outputTextFont;
    HFONT shortcutsTextFont;
    HFONT commentTextFont;
    HFONT commentEditboxTextFont;
    HFONT commentEditboxReadOnlyTextFont;
} GuiFonts;

typedef struct Hotkey {
    int id;
    UINT key1;
    UINT key2;

    Hotkey(int idValue, UINT key1Value, UINT key2Value)
        : id(idValue), key1(key1Value), key2(key2Value) {}
} Hotkey;

typedef struct ProgramThreads {
    HANDLE mainThread;
    HANDLE guiThread;
} ProgramThreads;

enum tekkenState {IN_FIGHT, IN_SEARCH};

extern HANDLE tekkenHandle;
extern HWND tekkenWindowHandle;
extern int tekkenPid;
extern QWORD lastFoundSteamId;
extern bool isSteamIdFound; // helps keep track of  lastFoundSteamId
extern QWORD userSteamId;
extern char* lastNameInPlayerlist;

extern void* fightThisPlayerMessagePointer;
extern void* secondsRemainingMessagePointer;
extern void* opponentFoundMessagePointer;
extern void* opponentNamePointer;
extern void* screenModePointer;
extern void* steamModulePointer;

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern GuiWindows guiWindows;
extern GuiFonts guiFonts;
extern HBRUSH solidBrush;
extern WNDPROC defaultEditProc;

extern Hotkey* hotkeys[NR_OF_HOTKEYS];

//mainThreads.cpp
void createThreads();
HANDLE beginThread(_beginthreadex_proc_type proc);
void endThread();
void waitForThreadsToTerminate();
void closeThreads();
unsigned __stdcall mainThread(void* arguments);
void initPlayerlist();
void loadTargetProcess();
void initTekkenHandle();
void initTekkenWindowHandle();
void initPointers();
void initModuleAdresses();
void editTargetProcessLoop();
void closeProgram();

//tekken.cpp
bool isGameLoaded();
bool isNewOpponentReceived();
bool isNewSteamIdReceived();                                          
bool isSteamApiLoaded(void* steamIdPointer);
void resetSteamApiBaseModuleAddress();
bool readAndIsSteamIdValid(void* steamIdPointer, QWORD* steamIdBuffer);
void handleNewReceivedOpponent();
char* updateMessagesWithoutOpponentName(char* currentOpponentName);
void updateMessagesWithoutSteamId();
void updateOpponentFoundMessage(char* message);
void updateFightThisPlayerMessage(char* message);
void updateSecondsRemainingMessage(char* message);
bool isNewFightAccepted();
bool isNewOpponentLoaded();
void cleanAllProcessMessages();
char* getNewCurrentLoadedOpponent(char* currentLoadedOpponentName);
bool isNewNameReceived(char* playerName, char* lastReceivedName); // no longer needed (now name obtained from web)
void displayOpponentName(); // no longer needed
void displayOpponentNameFromWeb();

//guiInput.cpp
void handleHotkeyInput(WPARAM hotkey);
void initHotkeys();
void registerHotKeys();
void registerHotKey(HWND windowHandle, int hotkeyId, UINT key1, UINT key2);
void unRegisterHotKeys();
void unRegisterHotKey(HWND windowHandle, int hotkeyId);

//file.cpp
void createExamplePlayerlist();
char* extractPlayerNameFromPlayerlistLine(char* line);
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
char* getLastNameInPlayerlist(char* filePath);
char* getLastCharacterInPlayerlist(char* filePath);
char* getLastLineOfFile(char* filePath);
bool bruteForceFind(char* text, char* pattern);
int bruteForceFindIndex(char* text, char* pattern);
int bruteForceFindIndexAfterIndex(char* text, char* pattern, int startIndex);
void replaceCommentInLastLineInFile(char* path, char* comment);
long writeAfterLastOccurenceOfCharInFile(char* path, char* text, char charToWriteAfter);
void setEndOfFileAtIndex(char* path, long position);

//targetMemory.cpp
HANDLE getProcessHandle(DWORD pid);
DWORD getProcessId(const std::wstring& nameProgramExe);
uintptr_t getModuleBaseAddress(DWORD pid, const wchar_t* moduleName);
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

//targetWindow.cpp
HWND getWindowHandle(const wchar_t* programWindowName);
void setScreenMode(DWORD screenMode);
void minimizeAndRestoreTekkenWindow();

//gui.cpp
unsigned __stdcall guiThread(void* arguments);
void initWindowsAndHotkeys();
void initWindows();
void registerMainWindowClass();
void registerCommentWindowClass();
void getConsoleWindowHandle();
void getScreenResolution();
void initFontsAndBrushes();
void createMainWindow();
void createCommentWindow();
void handleWindowsMessageQueueLoop();
LRESULT CALLBACK mainWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK commentWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK subEditProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
void openCommentWindow();
void setOpponentNameInCommentWindowTitle();
void disableCommentWindowEditbox();
void setFocusCommentWindow();
void saveCommentAndCloseCommentWindow();
void saveComment();
char* getTextFromCommentEditbox();
void writeCommentToFile(void* text);
void setTekkenWindowed();
void setTekkenFullscreen();
void showOrHideConsoleWindow();
void myGuiTerminalPrint(std::string text);
void printToStandardOutput(std::string text);
void printToTextboxOutput(std::string text);
void waitForWindowToBeCreated(HWND& windowHandle);
void printTextToEditControl(std::string text, HWND& editControlHandle);
wchar_t* stringToWString(std::string text);
void closeAllWindows();
void closeCommentWindow();
void deleteFontObjects();
void setPlayerNameInGui(char* playerName);
void setOpponentNameInGui(char* opponentName);
void updateAllGuiMessages(char* newOpponentName, char* characterName, char* playerlistComment);
void cleanAllGuiMessages();

//guiWindow.cpp
HWND createWindow(DWORD extendedStyle, LPCWSTR className, LPCWSTR windowName, DWORD style,
    int x, int y, int width, int height, HWND parentWindowHandle = NULL);
void registerWindowClass(WNDCLASS windowClass);
void sendMessage(HWND windowHandle, UINT msg, WPARAM wparam, LPARAM lparam);
void showWindow(HWND windowHandle, int showCommand);
bool isWindow(HWND windowHandle);
BOOL isWindowVisible(HWND windowHandle); 
void setForegroundWindow(HWND windowHandle);
void setFocus(HWND windowHandle);
void destroyWindow(HWND windowHandle);

//steamURL.cpp
std::string urlToString(LPCTSTR url);
std::wstring urlToWString(LPCTSTR url);
void urlToFile(LPCTSTR url, LPCTSTR filePath);
std::string extractNameFromSteamHtmlString(std::string htmlString);
std::wstring extractProfilePictureUrlFromSteamHtmlString(std::wstring htmlString);
std::string getOnlineNameUsingSteamId(QWORD steamId);
std::wstring getOnlineProfilePictureUrlUsingSteamId(QWORD steamId);

#endif

