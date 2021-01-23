#include "player-displayer.h"
#include "pointers.h"

unsigned __stdcall createWindowAndHandleInput(void* arguments) {
    initWindowsAndHotkeys();
    handleWindowsMessageQueueLoop();
    closeProgram();

    endThread();
    return ERROR_SUCCESS;
}

void initWindowsAndHotkeys() {
    initWindows();
    createMainWindow();
    initHotkeys();
}

void initWindows() {
    registerMainWindowClass();
    registerCommentWindowClass();
    getConsoleWindowHandle();
    getScreenResolution();
    initFontsAndBrushes();
}

void registerMainWindowClass() {
    WNDCLASS windowClass = { 0 };
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hInstance = NULL;
    windowClass.hIcon = (HICON)LoadImageW(GetModuleHandle(NULL), MAKEINTRESOURCE(APPLICATION_ICON), IMAGE_ICON, 0, 0, LR_LOADTRANSPARENT);
    windowClass.lpfnWndProc = mainWindowProc;
    windowClass.lpszClassName = (LPCWSTR)TEXT(CLASSNAME_MAINWINDOW);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    registerWindowClass(windowClass);
}

void registerCommentWindowClass() {
    WNDCLASS windowClass = { 0 };
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hInstance = NULL;
    windowClass.lpfnWndProc = commentWindowProc;
    windowClass.lpszClassName = (LPCWSTR)TEXT(CLASSNAME_COMMENTWINDOW);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    registerWindowClass(windowClass);
}

void registerWindowClass(WNDCLASS windowClass) {
    if (!RegisterClass(&windowClass)) {
        MessageBox(NULL, TEXT("Could not register class"), TEXT("Error"), MB_OK);
    }
}

void getConsoleWindowHandle() {
    windows.consoleWindowHandle = GetConsoleWindow();
}

void getScreenResolution() {
    SCREEN_WIDTH = GetSystemMetrics(SM_CXSCREEN);
    SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN);
}

void initFontsAndBrushes() {
    fonts.welcomeTextFont = CreateFont(FONT_SIZE, 0, 0, 0, FW_NORMAL, TRUE, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Consolas"));
    fonts.outputTextFont = CreateFont(FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Consolas"));
    fonts.informationTextFont = CreateFont(FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Consolas"));
    fonts.creditsTextFont = CreateFont(13, 0, 0, 0, FW_NORMAL, TRUE, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Courier New"));
    fonts.editboxTextFont = CreateFont(FONT_SIZE, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"));
    fonts.editboxReadOnlyTextFont = CreateFont(FONT_SIZE, 0, 0, 0, FW_BOLD, TRUE, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"));

    solidBrush = CreateSolidBrush(COLOR_PINK);
}

void createMainWindow() {
    int X_MAINWINDOW = SCREEN_WIDTH / 2 - WIDTH_MAINWINDOW / 2;
    int Y_MAINWINDOW = SCREEN_HEIGHT / 2 - HEIGHT_MAINWINDOW / 2;
    int WIDTH_TEXTBOX = 600;
    int X_TEXTBOX = 50;

    windows.mainWindowHandle = createWindow(WS_EX_DLGMODALFRAME | WS_EX_LAYERED,
        TEXT(CLASSNAME_MAINWINDOW), TEXT(TITLE_MAINWINDOW),
        WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
        X_MAINWINDOW, Y_MAINWINDOW, WIDTH_MAINWINDOW, HEIGHT_MAINWINDOW);

    HWND backgroundImageHandle = createWindow(0, TEXT("STATIC"), NULL,
        WS_CHILD | WS_VISIBLE | SS_BITMAP,
        0, 0, 100, 100, windows.mainWindowHandle);

    HWND welcomeTextHandle = createWindow(0, TEXT("STATIC"), TEXT(TEXT_WELCOME),
        WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER,
        X_TEXTBOX, 45, WIDTH_TEXTBOX, FONT_SIZE * 3 + 10, windows.mainWindowHandle);

    windows.outputTextHandle = createWindow(WS_EX_PALETTEWINDOW, TEXT("Edit"), NULL,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_LEFT | ES_AUTOVSCROLL,
        X_TEXTBOX, 120, WIDTH_TEXTBOX, FONT_SIZE * 6 + 10 + 180, windows.mainWindowHandle);

    HWND informationTextHandle = createWindow(0, TEXT("STATIC"), TEXT(TEXT_INFORMATION),
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        X_TEXTBOX, 420, WIDTH_TEXTBOX, FONT_SIZE * 5 + 10, windows.mainWindowHandle);

    HWND creditsTextHandle = createWindow(0, TEXT("STATIC"), TEXT(TEXT_CREDITS),
        WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT,
        828, 540, 153, 16, windows.mainWindowHandle);

    SetLayeredWindowAttributes(windows.mainWindowHandle, 0, (255 * WINDOW_OPACITY) / 100, LWA_ALPHA);
    HBITMAP backgroundBitmapHandle = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(BACKGROUND));
    sendMessage(backgroundImageHandle, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)backgroundBitmapHandle);
    sendMessage(welcomeTextHandle, WM_SETFONT, (LPARAM)fonts.welcomeTextFont, true);
    sendMessage(windows.outputTextHandle, WM_SETFONT, (LPARAM)fonts.outputTextFont, true);
    sendMessage(windows.outputTextHandle, EM_SETREADONLY, (WPARAM)TRUE, 0);
    sendMessage(informationTextHandle, WM_SETFONT, (LPARAM)fonts.informationTextFont, true);
    sendMessage(creditsTextHandle, WM_SETFONT, (LPARAM)fonts.creditsTextFont, true);

    showWindow(windows.consoleWindowHandle, SW_HIDE);
    showWindow(windows.mainWindowHandle, SW_SHOW);
}

void createCommentWindow() {
    int X_COMMENTWINDOW = SCREEN_WIDTH - WIDTH_COMMENTWINDOW;
    int Y_COMMENTWINDOW = SCREEN_HEIGHT - HEIGHT_COMMENTWINDOW - 40;

    windows.commentWindowHandle = createWindow(WS_EX_DLGMODALFRAME | WS_EX_LAYERED,
        TEXT(CLASSNAME_COMMENTWINDOW), TEXT(TITLE_COMMENTWINDOW),
        WS_SYSMENU,
        X_COMMENTWINDOW, Y_COMMENTWINDOW, WIDTH_COMMENTWINDOW, HEIGHT_COMMENTWINDOW);

    windows.commentEditboxHandle = createWindow(WS_EX_PALETTEWINDOW, TEXT("Edit"), NULL,
        WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_LEFT | ES_AUTOVSCROLL,
        10, 10, WIDTH_COMMENTWINDOW - 40, HEIGHT_COMMENTWINDOW - 100, windows.commentWindowHandle);

    windows.commentButtonHandle = createWindow(WS_EX_PALETTEWINDOW, TEXT("BUTTON"), TEXT("Save"),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHLIKE,
        WIDTH_COMMENTWINDOW / 2 - 20, HEIGHT_COMMENTWINDOW - 75, 40, 20, windows.commentWindowHandle);

    SetLayeredWindowAttributes(windows.commentWindowHandle, 0, (255 * WINDOW_OPACITY) / 100, LWA_ALPHA);
    defaultEditProc = (WNDPROC)SetWindowLongPtr(windows.commentEditboxHandle, GWLP_WNDPROC, (LONG_PTR)subEditProc);
    sendMessage(windows.commentEditboxHandle, WM_SETFONT, (WPARAM)fonts.editboxTextFont, TRUE);
    sendMessage(windows.commentEditboxHandle, EM_SETLIMITTEXT, EDITBOX_TEXT_MAX_LENGTH, 0);
    sendMessage(windows.commentButtonHandle, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), true);

    if (lastFoughtOpponentName != NULL) {
        setOpponentNameInCommentWindowTitle();
    }
    else {
        disableCommentWindowEditbox();
    }

    setFocusCommentWindow();
    showWindow(windows.commentWindowHandle, SW_SHOW);
}

HWND createWindow(DWORD extendedStyle, LPCWSTR className, LPCWSTR windowName, DWORD style,
    int x, int y, int width, int height, HWND parentWindowHandle) {

    return CreateWindowEx(extendedStyle, className, windowName, style, x, y, width, height,
        parentWindowHandle, NULL, NULL, NULL);
}

void sendMessage(HWND windowHandle, UINT msg, WPARAM wparam, LPARAM lparam) {
    SendMessage(windowHandle, msg, wparam, lparam);
}

void showWindow(HWND windowHandle, int showCommand) {
    ShowWindow(windowHandle, showCommand);
}

void handleWindowsMessageQueueLoop() { // loop to pull messages from queue for all windows in current thread
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg); // calls the windowProc callback function
    }
}

LRESULT CALLBACK mainWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
    case WM_CTLCOLORSTATIC:
        if (lparam == (LPARAM)windows.outputTextHandle) {
            SetTextColor((HDC)wparam, COLOR_WHITE);
            SetBkColor((HDC)wparam, COLOR_BLACK);
            return (LRESULT)(HBRUSH)GetStockObject(BLACK_BRUSH);
        }
        else {
            SetBkColor((HDC)wparam, COLOR_PINK);
            return (LRESULT)solidBrush;
        }
        break;
    case WM_HOTKEY:
        handleHotkeyInput(wparam);
        break;
    case WM_CHAR:
        if (wparam == VK_ESCAPE) {
            closeAllWindows();
        }
        break;
    case WM_CLOSE:
        closeAllWindows();
        break;
    case WM_DESTROY:
        unRegisterHotKeys();
        deleteFontObjects();
        PostQuitMessage(0); // posts a WM_QUIT message that makes GetMessage return 0 and the message loop exit
        break;
    default:
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

LRESULT CALLBACK commentWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
    case WM_CHAR:
        if (wparam == VK_RETURN) {
            saveCommentAndCloseCommentWindow();
            break;
        }
        if (wparam == VK_ESCAPE) {
            destroyWindow(hwnd);
        }
        break;
    case WM_COMMAND:
        if (lparam == (LPARAM)windows.commentButtonHandle) {
            saveCommentAndCloseCommentWindow();
        }
        break;
    default:
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

LRESULT CALLBACK subEditProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
    case WM_KEYDOWN:
        if (wparam == VK_RETURN) {
            saveCommentAndCloseCommentWindow();
            break;
        }
        else if (wparam == VK_ESCAPE) {
            destroyWindow(windows.commentWindowHandle);
            break;
        }
    default:
        return CallWindowProc(defaultEditProc, wnd, msg, wparam, lparam);
    }
    return CallWindowProc(defaultEditProc, wnd, msg, wparam, lparam);;
}

void openCommentWindow() {
    if (!isWindow(windows.commentWindowHandle)) {
        createCommentWindow();
    }
    else {
        setForegroundWindow(windows.commentWindowHandle);
        setFocusCommentWindow();
    }
}

bool isWindow(HWND windowHandle) {
    return IsWindow(windowHandle);
}

void setForegroundWindow(HWND windowHandle) {
    SetForegroundWindow(windowHandle);
}

void setOpponentNameInCommentWindowTitle() {
    wchar_t* name = stringToWString(lastFoughtOpponentName);
    sendMessage(windows.commentWindowHandle, WM_SETTEXT, 0, (LPARAM)std::wstring(L"Comment").append(L" - Player: ").append(std::wstring(name)).c_str());
    delete[] name;
}

void disableCommentWindowEditbox() {
    SetClassLongPtr(windows.commentWindowHandle, GCLP_HBRBACKGROUND, (LONG)HOLLOW_BRUSH);
    LONG_PTR style = GetWindowLongPtr(windows.commentEditboxHandle, GWL_STYLE);
    SetWindowLongPtr(windows.commentEditboxHandle, GWL_STYLE, style | ES_CENTER);
    sendMessage(windows.commentEditboxHandle, WM_SETTEXT, 0, (LPARAM)TEXT(TEXT_COMMENTWINDOW));
    sendMessage(windows.commentEditboxHandle, WM_SETFONT, (WPARAM)fonts.editboxReadOnlyTextFont, TRUE);
    sendMessage(windows.commentEditboxHandle, EM_SETREADONLY, (WPARAM)TRUE, 0);
    sendMessage(windows.commentButtonHandle, WM_SETTEXT, 0, (LPARAM)L"Close");
}

void setFocusCommentWindow() {
    if (lastFoughtOpponentName != NULL) {
        setFocus(windows.commentEditboxHandle);
    }
    sendMessage(windows.commentEditboxHandle, WM_KEYDOWN, (WPARAM)VK_LBUTTON, 0);
    sendMessage(windows.commentEditboxHandle, WM_KEYUP, (WPARAM)VK_LBUTTON, 0);
}

void setFocus(HWND windowHandle) {
    SetFocus(windowHandle);
}

void saveCommentAndCloseCommentWindow() {
    saveComment();
    closeCommentWindow();
}

void saveComment() {
    if (lastFoughtOpponentName != NULL) {
        char* text = getTextFromCommentEditbox();
        if (text[0] != '\0') {
            _beginthread(writeCommentToFile, 0, (void*)text);
        }
    }
}

char* getTextFromCommentEditbox() {
    int editboxTextLength = GetWindowTextLengthA(windows.commentEditboxHandle);
    char* text = (char*)malloc((EDITBOX_TEXT_MAX_LENGTH + 1) * sizeof(CHAR));
    int copiedTextLength = GetWindowTextA(windows.commentEditboxHandle, (LPSTR)text, editboxTextLength + 1);
    return text;
}

void writeCommentToFile(void* text) {
    char* comment = (char*)text;
    replaceCommentInLastLineInFile((char*)PLAYERLIST_PATH, comment);

    print(std::string("Saved comment for player \"").append(std::string(lastFoughtOpponentName))
        .append(std::string("\": ")).append(std::string(comment)).append(std::string("\r\n")));
    free(comment);
}

void setTekkenWindowed() {
    setScreenMode(SCREEN_MODE_WINDOWED);
}

void setTekkenFullscreen() {
    setScreenMode(SCREEN_MODE_FULLSCREEN);
}

void setScreenMode(DWORD newScreenMode) {
    if (tekkenWindowHandle != NULL) {
        DWORD screenMode = readDwordFromMemory(tekkenHandle, screenModePointer);
        if (screenMode != newScreenMode) {
            writeDwordToMemory(tekkenHandle, screenModePointer, newScreenMode);
            minimizeAndRestoreTekkenWindow();
            std::map<int, const char*> screenModesText = {
                {SCREEN_MODE_FULLSCREEN, "fullscreen"},
                {SCREEN_MODE_BORDERLESS, "borderless"},
                {SCREEN_MODE_WINDOWED, "windowed"}
            };
            print(std::string("Screen mode set to ").append(screenModesText.at(newScreenMode)).append(std::string("\r\n")));
        }
    }
}

void minimizeAndRestoreTekkenWindow() {
    showWindow(tekkenWindowHandle, SW_MINIMIZE);
    showWindow(tekkenWindowHandle, SW_RESTORE);
}

void showOrHideConsoleWindow() {
    if (isWindowVisible(windows.consoleWindowHandle)) {
        showWindow(windows.consoleWindowHandle, SW_HIDE);
    }
    else {
        showWindow(windows.consoleWindowHandle, SW_SHOW);
    }
}

BOOL isWindowVisible(HWND windowHandle) {
    return IsWindowVisible(windowHandle);
}

HWND getWindowHandle(const wchar_t* programWindowName) {
    HWND windowHandle;
    DWORD errorCode;
    windowHandle = FindWindow(NULL, programWindowName);
    if (windowHandle == NULL) {
        errorCode = GetLastError();
        //        print(std::string("Error! FindWindow in getWindowHandle failed! Code: ").append(std::to_string(errorCode)).append(std::string("\r\n")));
    }
    return windowHandle;
}

void print(std::string text) {
    printToStandardOutput(text);
    printToTextboxOutput(text);
}

void printToStandardOutput(std::string text) {
    std::cout << text;
}

void printToTextboxOutput(std::string text) {
    if (!isWindow(windows.outputTextHandle)) {
        waitForWindowToBeCreated(windows.outputTextHandle);
    }
    else {
        printTextToEditControl(text, windows.outputTextHandle);
    }
}

void waitForWindowToBeCreated(HWND& windowHandle) {
    do {
        Sleep(100);
    } while (continueThreads && !isWindow(windowHandle));
}

void printTextToEditControl(std::string text, HWND& editControlHandle) {
    wchar_t* textBuffer = stringToWString(text);
    int indexEndOfText = GetWindowTextLength(editControlHandle);
    sendMessage(editControlHandle, EM_SETSEL, (WPARAM)indexEndOfText, (LPARAM)indexEndOfText);
    sendMessage(editControlHandle, EM_REPLACESEL, 0, (LPARAM)textBuffer);
    sendMessage(editControlHandle, WM_VSCROLL, (WPARAM)SB_LINEUP, NULL); // don't display last newline
    delete[] textBuffer;
}

wchar_t* stringToWString(std::string text) { // caller of this function has to do delete[] textBuffer;
    size_t nrOfCharConverted;
    wchar_t* textBuffer = new wchar_t[text.size() + 1];
    mbstowcs_s(&nrOfCharConverted, textBuffer, text.size() + 1, text.c_str(), text.size());
    return textBuffer;
}

void closeAllWindows() {
    destroyWindow(windows.mainWindowHandle);
    if (isWindow(windows.commentWindowHandle)) {
        destroyWindow(windows.commentWindowHandle);
    }
}

void closeCommentWindow() {
    destroyWindow(windows.commentWindowHandle);
}

void destroyWindow(HWND windowHandle) {
    DestroyWindow(windowHandle); // posts a WM_DESTROY message
}

void deleteFontObjects() {
    DeleteObject(fonts.welcomeTextFont);
    DeleteObject(fonts.outputTextFont);
    DeleteObject(fonts.informationTextFont);
    DeleteObject(fonts.creditsTextFont);
    DeleteObject(fonts.editboxTextFont);
    DeleteObject(fonts.editboxReadOnlyTextFont);
}
