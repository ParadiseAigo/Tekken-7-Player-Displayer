#include "player-displayer.h"

Hotkey* hotkeys[NR_OF_HOTKEYS];

void handleHotkeyInput(WPARAM hotkey) {
    switch (hotkey) {
    case ALT_W:
        setTekkenWindowed();
        break;
    case ALT_F:
        setTekkenFullscreen();
        break;
    case ALT_O:
        setTekkenWindowed();
        openPlayerlist();
        break;
    case ALT_C:
        setTekkenWindowed();
        openCommentWindow();
        break;
    case ALT_T:
        showOrHideConsoleWindow();
        break;
    case ALT_S:
        turnOffSilentMode();
        break;
    }
}

void initHotkeys() {
    hotkeys[0] = new Hotkey(ALT_W, MOD_ALT, KEY_W);
    hotkeys[1] = new Hotkey(ALT_F, MOD_ALT, KEY_F);
    hotkeys[2] = new Hotkey(ALT_C, MOD_ALT, KEY_C);
    hotkeys[3] = new Hotkey(ALT_O, MOD_ALT, KEY_O);
    hotkeys[4] = new Hotkey(ALT_T, MOD_ALT, KEY_T);
    hotkeys[5] = new Hotkey(ALT_S, MOD_ALT, KEY_S);
    registerHotKeys();
}

void registerHotKeys() {
    for (int i = 0; i < NR_OF_HOTKEYS; i++) {
        registerHotKey(guiWindows.mainWindowHandle, hotkeys[i]->id, hotkeys[i]->key1, hotkeys[i]->key2);
    }
}

void registerHotKey(HWND windowHandle, int hotkeyId, UINT key1, UINT key2) {
    RegisterHotKey(windowHandle, hotkeyId, key1, key2);
}

void unRegisterHotKeys() {
    for (int i = 0; i < NR_OF_HOTKEYS; i++) {
        unRegisterHotKey(guiWindows.mainWindowHandle, hotkeys[i]->id);
    }
}

void unRegisterHotKey(HWND windowHandle, int hotkeyId) {
    UnregisterHotKey(windowHandle, hotkeyId);
}