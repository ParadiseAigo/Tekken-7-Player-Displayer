#ifndef TEKKEN7_PLAYERDISPLAYER_GUI_H
#define TEKKEN7_PLAYERDISPLAYER_GUI_H

//gui.cpp
unsigned __stdcall createWindowAndHandleInput(void* arguments);
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
void print(std::string text);
void printToStandardOutput(std::string text);
void printToTextboxOutput(std::string text);
void waitForWindowToBeCreated(HWND& windowHandle);
void printTextToEditControl(std::string text, HWND& editControlHandle);
wchar_t* stringToWString(std::string text);
void closeAllWindows();
void closeCommentWindow();
void deleteFontObjects();
void setPlayerNameInGui(char* playerName);
void updateAllGuiMessages(char* newOpponentName, char* characterName, char* playerlistComment);
void cleanAllGuiMessages();

#endif