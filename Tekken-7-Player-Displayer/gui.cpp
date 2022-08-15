#include "player-displayer.h"
#include "pointers.h"

int SCREEN_WIDTH;
int SCREEN_HEIGHT;

GuiWindows guiWindows;
GuiFonts guiFonts;

HBRUSH solidBrush;
WNDPROC defaultEditProc;

LPPICTURE opponentProfilePicture;

unsigned __stdcall guiThread(void* arguments) {
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

void getConsoleWindowHandle() {
    guiWindows.consoleWindowHandle = GetConsoleWindow();
}

void getScreenResolution() {
    SCREEN_WIDTH = GetSystemMetrics(SM_CXSCREEN);
    SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN);
}

void initFontsAndBrushes() {
    guiFonts.informationNameTextFont = CreateFont(FONT_SIZE, 0, 0, 0, FW_NORMAL, TRUE, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Consolas"));
    guiFonts.informationValueTextFont = CreateFont(FONT_SIZE_INFO, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Consolas"));
    guiFonts.outputTextFont = CreateFont(FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Consolas"));
    guiFonts.shortcutsTextFont = CreateFont(18, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Consolas"));
    guiFonts.commentTextFont = CreateFont(20, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Consolas"));
    guiFonts.commentEditboxTextFont = CreateFont(FONT_SIZE, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"));
    guiFonts.commentEditboxReadOnlyTextFont = CreateFont(FONT_SIZE, 0, 0, 0, FW_BOLD, TRUE, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"));

    solidBrush = CreateSolidBrush(COLOR_GRAY);
}

void createMainWindow() {
    int X_MAINWINDOW = SCREEN_WIDTH / 2 - WIDTH_MAINWINDOW / 2;
    int Y_MAINWINDOW = SCREEN_HEIGHT / 2 - HEIGHT_MAINWINDOW / 2;
    int X_TEXTBOX = 650;

    if (doesWindowPositionFileExist()) {
        getWindowSavedPosition(&X_MAINWINDOW, &Y_MAINWINDOW);
    }

    guiWindows.mainWindowHandle = createWindow(WS_EX_DLGMODALFRAME,
        TEXT(CLASSNAME_MAINWINDOW), TEXT(TITLE_MAINWINDOW),
        WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
        X_MAINWINDOW, Y_MAINWINDOW, WIDTH_MAINWINDOW, HEIGHT_MAINWINDOW);

    HWND backgroundImageHandle = createWindow(0, TEXT("STATIC"), NULL,
        WS_CHILD | WS_VISIBLE | SS_BITMAP,
        0, 0, 0, 0, guiWindows.mainWindowHandle);

    guiWindows.opponentProfilePictureHandle = createWindow(0, TEXT("STATIC"), NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | SS_BITMAP | SS_REALSIZECONTROL,
        910, 545, 100, 100, guiWindows.mainWindowHandle);

    guiWindows.outputTextHandle = createWindow(WS_EX_PALETTEWINDOW, TEXT("Edit"), NULL,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_LEFT | ES_AUTOVSCROLL | ES_READONLY,
        40, 40, 580, FONT_SIZE * 30 + 10, guiWindows.mainWindowHandle);

    HWND shortcutsTextHandle = createWindow(0, TEXT("STATIC"), TEXT(TEXT_INFORMATION),
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        X_TEXTBOX, 50, 380, 18 * 5 + 5, guiWindows.mainWindowHandle);

    HWND opponentNameTextHandle = createWindow(0, TEXT("STATIC"), TEXT("Opponent Name"),
        WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER,
        X_TEXTBOX, 170, 150, FONT_SIZE * 1 + 5, guiWindows.mainWindowHandle);
    HWND opponentLocationTextHandle = createWindow(0, TEXT("STATIC"), TEXT("IP Location"),
        WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER,
        X_TEXTBOX, 300, 150, FONT_SIZE * 1 + 5, guiWindows.mainWindowHandle);
    HWND opponentCharacterTextHandle = createWindow(0, TEXT("STATIC"), TEXT("Last Character"),
        WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER,
        X_TEXTBOX, 430, 150, FONT_SIZE * 1 + 5, guiWindows.mainWindowHandle);
    HWND commentNameTextHandle = createWindow(0, TEXT("STATIC"), TEXT("Comment"),
        WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER,
        40, 550, 100, FONT_SIZE * 1 + 5, guiWindows.mainWindowHandle);

    guiWindows.opponentNameValueTextHandle = createWindow(0, TEXT("Edit"), TEXT(""),
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_AUTOVSCROLL | ES_READONLY,
        X_TEXTBOX, 200, 380, FONT_SIZE * 1 + 40, guiWindows.mainWindowHandle);
    guiWindows.opponentLocationValueTextHandle = createWindow(0, TEXT("Edit"), TEXT(""),
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_AUTOVSCROLL | ES_READONLY,
        X_TEXTBOX, 330, 380, FONT_SIZE * 1 + 40, guiWindows.mainWindowHandle);
    guiWindows.opponentCharacterValueTextHandle = createWindow(0, TEXT("Edit"), TEXT(""),
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_CENTER | ES_AUTOVSCROLL | ES_READONLY,
        X_TEXTBOX, 460, 380, FONT_SIZE * 1 + 40, guiWindows.mainWindowHandle);
    guiWindows.commentValueTextHandle = createWindow(0, TEXT("Edit"), TEXT(""),
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_LEFT | ES_AUTOVSCROLL | ES_READONLY,
        40, 580, 845, 20 * 2 + 10, guiWindows.mainWindowHandle);

    HBITMAP backgroundBitmapHandle = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(BACKGROUND));
    sendMessage(backgroundImageHandle, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)backgroundBitmapHandle);
    sendMessage(guiWindows.outputTextHandle, WM_SETFONT, (LPARAM)guiFonts.outputTextFont, true);
    sendMessage(shortcutsTextHandle, WM_SETFONT, (LPARAM)guiFonts.shortcutsTextFont, true);
    sendMessage(opponentNameTextHandle, WM_SETFONT, (LPARAM)guiFonts.informationNameTextFont, true);
    sendMessage(opponentLocationTextHandle, WM_SETFONT, (LPARAM)guiFonts.informationNameTextFont, true);
    sendMessage(opponentCharacterTextHandle, WM_SETFONT, (LPARAM)guiFonts.informationNameTextFont, true);
    sendMessage(commentNameTextHandle, WM_SETFONT, (LPARAM)guiFonts.informationNameTextFont, true);
    sendMessage(guiWindows.opponentNameValueTextHandle, WM_SETFONT, (LPARAM)guiFonts.informationValueTextFont, true);
    sendMessage(guiWindows.opponentLocationValueTextHandle, WM_SETFONT, (LPARAM)guiFonts.informationValueTextFont, true);
    sendMessage(guiWindows.opponentCharacterValueTextHandle, WM_SETFONT, (LPARAM)guiFonts.informationValueTextFont, true);
    sendMessage(guiWindows.commentValueTextHandle, WM_SETFONT, (LPARAM)guiFonts.commentTextFont, true);
    
    showWindow(guiWindows.consoleWindowHandle, SW_HIDE);
    showWindow(guiWindows.mainWindowHandle, SW_SHOW);
}

void createCommentWindow() {
    int X_COMMENTWINDOW = SCREEN_WIDTH - WIDTH_COMMENTWINDOW;
    int Y_COMMENTWINDOW = SCREEN_HEIGHT - HEIGHT_COMMENTWINDOW - 40;

    guiWindows.commentWindowHandle = createWindow(WS_EX_DLGMODALFRAME | WS_EX_LAYERED,
        TEXT(CLASSNAME_COMMENTWINDOW), TEXT(TITLE_COMMENTWINDOW),
        WS_SYSMENU,
        X_COMMENTWINDOW, Y_COMMENTWINDOW, WIDTH_COMMENTWINDOW, HEIGHT_COMMENTWINDOW);

    guiWindows.commentEditboxHandle = createWindow(WS_EX_PALETTEWINDOW, TEXT("Edit"), NULL,
        WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_LEFT | ES_AUTOVSCROLL,
        10, 10, WIDTH_COMMENTWINDOW - 40, HEIGHT_COMMENTWINDOW - 100, guiWindows.commentWindowHandle);

    guiWindows.commentButtonHandle = createWindow(WS_EX_PALETTEWINDOW, TEXT("BUTTON"), TEXT("Save"),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHLIKE,
        WIDTH_COMMENTWINDOW / 2 - 20, HEIGHT_COMMENTWINDOW - 75, 40, 20, guiWindows.commentWindowHandle);

    SetLayeredWindowAttributes(guiWindows.commentWindowHandle, 0, (255 * WINDOW_OPACITY) / 100, LWA_ALPHA);
    defaultEditProc = (WNDPROC)SetWindowLongPtr(guiWindows.commentEditboxHandle, GWLP_WNDPROC, (LONG_PTR)subEditProc);
    sendMessage(guiWindows.commentEditboxHandle, WM_SETFONT, (WPARAM)guiFonts.commentEditboxTextFont, TRUE);
    sendMessage(guiWindows.commentEditboxHandle, EM_SETLIMITTEXT, EDITBOX_TEXT_MAX_LENGTH, 0);
    sendMessage(guiWindows.commentButtonHandle, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), true);

    if (lastNameInPlayerlist != NULL) {
        setOpponentNameInCommentWindowTitle();
    } else {
        disableCommentWindowEditbox();
    }

    setFocusCommentWindow();
    showWindow(guiWindows.commentWindowHandle, SW_SHOW);
}

void handleWindowsMessageQueueLoop() { // loop to pull messages from queue for all windows in current thread
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg); // calls the mainWindowProc callback function
    }
}

LRESULT CALLBACK mainWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
    case WM_CTLCOLORSTATIC:
        if (lparam == (LPARAM)guiWindows.outputTextHandle) {
            SetTextColor((HDC)wparam, COLOR_WHITE);
            SetBkColor((HDC)wparam, COLOR_BLACK);
            return (LRESULT)(HBRUSH)GetStockObject(BLACK_BRUSH);
        } else if (lparam == (LPARAM)guiWindows.opponentProfilePictureHandle) {
            drawPictureOnWindow(opponentProfilePicture, guiWindows.opponentProfilePictureHandle);
            return (LRESULT)(HBRUSH)GetStockObject(HOLLOW_BRUSH);
        } else {
            SetBkColor((HDC)wparam, COLOR_GRAY);
            return (LRESULT)solidBrush;
        }
        break;
    case WM_HOTKEY:
        handleHotkeyInput(wparam);
        break;
    case WM_CHAR:
        if (wparam == VK_ESCAPE) {
            //closeAllWindows();
        }
        break;
    case WM_CLOSE:  // when the [X] is pressed of the gui window
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
        if (lparam == (LPARAM)guiWindows.commentButtonHandle) {
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
        } else if (wparam == VK_ESCAPE) {
            destroyWindow(guiWindows.commentWindowHandle);
            break;
        }
    default:
        return CallWindowProc(defaultEditProc, wnd, msg, wparam, lparam);
    }
    return CallWindowProc(defaultEditProc, wnd, msg, wparam, lparam);;
}

void openCommentWindow() {
    if (!isWindow(guiWindows.commentWindowHandle)) {
        createCommentWindow();
    } else {
        setForegroundWindow(guiWindows.commentWindowHandle);
        setFocusCommentWindow();
    }
}

void setOpponentNameInCommentWindowTitle() {
    char* lastCharacter;
    lastCharacter = getLastCharacterInPlayerlist((char*)PLAYERLIST_PATH);
    sendMessage(guiWindows.commentWindowHandle, WM_SETTEXT, 0, (LPARAM)std::wstring(L"Comment - Player: ")
        .append(charPtrToWString(lastNameInPlayerlist)).append(L" - ")
        .append(charPtrToWString(lastCharacter)).c_str()
    );
    if (lastCharacter != NULL) {
		free(lastCharacter);
    }
}

void disableCommentWindowEditbox() {
    SetClassLongPtr(guiWindows.commentWindowHandle, GCLP_HBRBACKGROUND, (LONG)HOLLOW_BRUSH);
    LONG_PTR style = GetWindowLongPtr(guiWindows.commentEditboxHandle, GWL_STYLE);
    SetWindowLongPtr(guiWindows.commentEditboxHandle, GWL_STYLE, style | ES_CENTER);
    sendMessage(guiWindows.commentEditboxHandle, WM_SETTEXT, 0, (LPARAM)TEXT(TEXT_COMMENTWINDOW));
    sendMessage(guiWindows.commentEditboxHandle, WM_SETFONT, (WPARAM)guiFonts.commentEditboxReadOnlyTextFont, TRUE);
    sendMessage(guiWindows.commentEditboxHandle, EM_SETREADONLY, (WPARAM)TRUE, 0);
    sendMessage(guiWindows.commentButtonHandle, WM_SETTEXT, 0, (LPARAM)L"Close");
}

void setFocusCommentWindow() {
    if (lastNameInPlayerlist != NULL) {
        setFocus(guiWindows.commentEditboxHandle);
    }
    sendMessage(guiWindows.commentEditboxHandle, WM_KEYDOWN, (WPARAM)VK_LBUTTON, 0);
    sendMessage(guiWindows.commentEditboxHandle, WM_KEYUP, (WPARAM)VK_LBUTTON, 0);
}

void saveCommentAndCloseCommentWindow() {
    saveComment();
    closeCommentWindow();
}

void saveComment() {
    if (lastNameInPlayerlist != NULL) {
        char* text = getTextFromCommentEditbox();
        if (text[0] != '\0') {
            _beginthread(writeCommentToFile, 0, (void*)text);
        }
    }
}

char* getTextFromCommentEditbox() {
    int editboxTextLength = GetWindowTextLengthA(guiWindows.commentEditboxHandle);
    char* text = (char*)malloc((EDITBOX_TEXT_MAX_LENGTH + 1) * sizeof(CHAR));
    int copiedTextLength = GetWindowTextA(guiWindows.commentEditboxHandle, (LPSTR)text, editboxTextLength + 1);
    return text;
}

void writeCommentToFile(void* text) {
    char* comment = (char*)text;
    replaceCommentInLastLineInFile((char*)PLAYERLIST_PATH, comment);

    myGuiTerminalPrint(std::string("Saved comment for player \"").append(std::string(lastNameInPlayerlist))
        .append(std::string("\": ")).append(std::string(comment)).append(std::string("\r\n")));
    free(comment);
}

void setTekkenWindowed() {
    setScreenMode(SCREEN_MODE_WINDOWED);
}

void setTekkenFullscreen() {
    setScreenMode(SCREEN_MODE_FULLSCREEN);
}

void myGuiTerminalPrint(std::string text) {
    //if (guiWindows.mainWindowHandle != NULL) {
        printToStandardOutput(text);
        printToTextboxOutput(text);
    //}
}

void printToStandardOutput(std::string text) {
    std::cout << text;
}

void printToTextboxOutput(std::string text) {
    if (!isWindow(guiWindows.outputTextHandle)) {
        waitForWindowToBeCreated(guiWindows.outputTextHandle);
    }
    printTextToEditControl(text, guiWindows.outputTextHandle);
}

void waitForWindowToBeCreated(HWND& windowHandle) {
    do {
        Sleep(100);
    } while (!isWindow(windowHandle));
}

void printTextToEditControl(std::string text, HWND& editControlHandle) {
    QWORD indexEndOfText = GetWindowTextLength(editControlHandle);
    QWORD sizeLimitText = SendMessage(editControlHandle, EM_GETLIMITTEXT, 0, 0);
    if (indexEndOfText + (QWORD) text.size() >= sizeLimitText) {
        sendMessage(editControlHandle, EM_SETLIMITTEXT, (WPARAM) (sizeLimitText + (QWORD)text.size() + (QWORD)EDITBOX_TEXT_MAX_LENGTH), 0);
    }
    sendMessage(editControlHandle, EM_SETSEL, (WPARAM)indexEndOfText, (LPARAM)indexEndOfText);
    sendMessage(editControlHandle, EM_REPLACESEL, 0, (LPARAM)(wchar_t*)charPtrToWString((char*)text.c_str()).c_str());
}

std::wstring charPtrToWString(char* text) {
    size_t textSize = strlen(text) + 1;
    wchar_t* buffer = (wchar_t*)malloc(textSize * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, text, -1, buffer, (int) textSize);
    std::wstring wstr(buffer);
    free(buffer);
    return wstr;
}

std::string wcharPtrToString(wchar_t* text) {
    size_t textSize = wcslen(text) + 1;
    char* buffer = (char*)malloc(textSize * sizeof(char));
    size_t nrOfCharConverted;
    wcstombs_s(&nrOfCharConverted, buffer, textSize, (const wchar_t*)text, textSize);
    std::string str(buffer);
    free(buffer);
    return str;
}

void closeAllWindows() {
    if (!doesWindowPositionFileExist()) {
		createWindowPositionFile();
    }
	saveWindowPositionInFile();
    destroyWindow(guiWindows.mainWindowHandle);
    if (isWindow(guiWindows.commentWindowHandle)) {
        destroyWindow(guiWindows.commentWindowHandle);
    }
}

void closeCommentWindow() {
    destroyWindow(guiWindows.commentWindowHandle);
}

void deleteFontObjects() {
    DeleteObject(guiFonts.informationNameTextFont);
    DeleteObject(guiFonts.informationValueTextFont);
    DeleteObject(guiFonts.outputTextFont);
    DeleteObject(guiFonts.shortcutsTextFont);
    DeleteObject(guiFonts.commentTextFont);
    DeleteObject(guiFonts.commentEditboxTextFont);
    DeleteObject(guiFonts.commentEditboxReadOnlyTextFont);
}

void showOrHideConsoleWindow() {
    if (isWindowVisible(guiWindows.consoleWindowHandle)) {
        showWindow(guiWindows.consoleWindowHandle, SW_HIDE);
    } else {
        showWindow(guiWindows.consoleWindowHandle, SW_SHOW);
    }
}

void setOpponentNameInGui(char* opponentName) {
    setTextAndResizeToFitInWindow(opponentName, guiWindows.opponentNameValueTextHandle);
}

void setOpponentLocationInGui(char* location) {
    setTextAndResizeToFitInWindow(location, guiWindows.opponentLocationValueTextHandle);
}

void setTextAndResizeToFitInWindow(char* text, HWND hwnd) {
    int fontSize = FONT_SIZE_INFO;
    HFONT fontHandle = guiFonts.informationValueTextFont;
    HDC hdc = GetDC(hwnd);

    sendMessage(hwnd, WM_SETFONT, (LPARAM)fontHandle, true);
    while (isTextLargerThanWindow(text, hwnd) && fontSize > 0) {
        fontSize--;
        fontHandle = CreateFont(fontSize, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                     CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Consolas"));
        sendMessage(hwnd, WM_SETFONT, (LPARAM)fontHandle, true);

        // clean up previous font object (the first font in guiFonts is not cleaned up here)
        HFONT oldFontHandle = (HFONT)SelectObject(hdc, fontHandle);
        DeleteObject(oldFontHandle);
    }
    ReleaseDC(hwnd, hdc);
    sendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)(wchar_t*)charPtrToWString(text).c_str());
}

bool isTextLargerThanWindow(char* text, HWND hwnd) {
    RECT windowRect = { 0, 0, 0, 0 };
    GetClientRect(hwnd, &windowRect);

    RECT textRect = { 0, 0, 0, 0 };
    HDC hdc = GetDC(hwnd);
    HFONT currentFont = (HFONT)SendMessage(hwnd, WM_GETFONT, 0, 0);
    HFONT oldFont = (HFONT)SelectObject(hdc, currentFont);
    DrawTextA(hdc, (const char*)text, strlen(text), &textRect, DT_CALCRECT | DT_NOPREFIX | DT_SINGLELINE);
    // clean up
    SelectObject(hdc, oldFont);
    ReleaseDC(hwnd, hdc);

    long windowMargin = 10;
    long windowWidth = abs(windowRect.right - windowRect.left - windowMargin);
    long windowHeight = abs(windowRect.bottom - windowRect.top);
    long textWidth = abs(textRect.right - textRect.left);
    long textHeight = abs(textRect.bottom - textRect.top);

    return textWidth > windowWidth || textHeight > windowHeight;
}

void loadOpponentProfilePictureFromFileAndRedraw(LPCTSTR filePath) { // this function only works for jpg, gif
    opponentProfilePicture = loadImageFromFile(filePath);            // the global variable is used to draw the picture in WM_PAINT 
    RedrawWindow(guiWindows.opponentProfilePictureHandle, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
}

void loadOpponentProfilePictureFromPNGFileAndRedraw(LPCTSTR filePath) { // this function works for png, jpg and gif
    CImage image;                                                       // but the quality is sometimes bad for jpg (because of the conversion to bmp)
    image.Load(filePath);
    HBITMAP bitmapHandle = image.Detach();
    sendMessage(guiWindows.opponentProfilePictureHandle, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bitmapHandle);
}

void clearOpponentProfilePicture() {
    if (opponentProfilePicture != NULL) {
        opponentProfilePicture->Release();
        opponentProfilePicture = NULL;
    }
    sendMessage(guiWindows.opponentProfilePictureHandle, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)NULL);
    RECT rect;
    GetClientRect(guiWindows.opponentProfilePictureHandle, &rect);
    MapWindowPoints(guiWindows.opponentProfilePictureHandle, guiWindows.mainWindowHandle, (LPPOINT)&rect, 2);
    RedrawWindow(guiWindows.mainWindowHandle, &rect, NULL, RDW_ERASE | RDW_INVALIDATE);
}

void updateAllGuiMessages(char* newOpponentName, char* characterName, char* playerlistComment) {
    setOpponentNameInGui(newOpponentName);
	if (characterName == NULL) {
		sendMessage(guiWindows.opponentCharacterValueTextHandle, WM_SETTEXT, 0, (LPARAM)TEXT(""));
	} else {
		sendMessage(guiWindows.opponentCharacterValueTextHandle, WM_SETTEXT, 0, (LPARAM)(wchar_t*)charPtrToWString(characterName).c_str());
	}
	sendMessage(guiWindows.commentValueTextHandle, WM_SETTEXT, 0, (LPARAM)(wchar_t*)charPtrToWString(playerlistComment).c_str());
}

void cleanAllGuiMessages() {
    sendMessage(guiWindows.opponentNameValueTextHandle, WM_SETTEXT, 0, (LPARAM)TEXT(""));
    sendMessage(guiWindows.opponentLocationValueTextHandle, WM_SETTEXT, 0, (LPARAM)TEXT(""));
	sendMessage(guiWindows.opponentCharacterValueTextHandle, WM_SETTEXT, 0, (LPARAM)TEXT(""));
	sendMessage(guiWindows.commentValueTextHandle, WM_SETTEXT, 0, (LPARAM)TEXT(""));
    clearOpponentProfilePicture();
}
