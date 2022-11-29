using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows;
using System.Windows.Interop;

namespace Tekken_7_Player_Displayer
{
    public static class Hotkeys
    {
        public delegate IntPtr HookProc(int nCode, IntPtr wParam, IntPtr lParam);

        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern IntPtr SetWindowsHookEx(int idHook, HookProc lpfn, IntPtr hmod, int dwThreadId);

        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern IntPtr CallNextHookEx(IntPtr hhk, int nCode, IntPtr wParam, IntPtr lParam);

        [DllImport("user32.dll")]
        public static extern ushort GetAsyncKeyState(int nVirtKey);

        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern bool UnhookWindowsHookEx(IntPtr hhk);

        [DllImport("user32.dll")]
        private static extern bool RegisterHotKey(IntPtr hWnd, int id, int fsModifiers, int vk);

        [DllImport("user32.dll")]
        private static extern bool UnregisterHotKey(IntPtr hWnd, int id);

        public struct KBDLLHOOKSTRUCT
        {
            public int vkCode;
            public int scanCode;
            public uint flags;
            public uint time;
            public IntPtr dwExtraInfo;
        }

        private class Hotkey
        {
            public int Modifier;
            public int Key;
            public Action handler;
        }

        private const int WH_KEYBOARD_LL = 13;
        private const int ALT = 0x0001;
        private const int KEY_W = 0x57;
        private const int KEY_F = 0x46;
        private const int KEY_C = 0x43;
        private const int KEY_O = 0x4F;
        private const int KEY_S = 0x53;
        private const int KEY_L = 0x4C;

        private static IntPtr hHook;
        private static HookProc hookProc;
        private static IntPtr windowHandle;
        private static Dictionary<int, Hotkey> hotkeys;

        public static bool Enabled => hHook != IntPtr.Zero;

        static Hotkeys()
        {
            hHook = IntPtr.Zero;
            hotkeys = new Dictionary<int, Hotkey>();
            hookProc = new HookProc(EvtDispatcher);
            windowHandle = new WindowInteropHelper(MainWindow.mainWindow).Handle;
        }

        public static void Init()
        {
            AddHotkey(ALT, KEY_W, () => HandleKeysPressed_ALT_W());
            AddHotkey(ALT, KEY_F, () => HandleKeysPressed_ALT_F());
            AddHotkey(ALT, KEY_C, () => HandleKeysPressed_ALT_C());
            AddHotkey(ALT, KEY_O, () => HandleKeysPressed_ALT_O());
            AddHotkey(ALT, KEY_S, () => HandleKeysPressed_ALT_S());
            AddHotkey(ALT, KEY_L, () => HandleKeysPressed_ALT_L());
        }

        public static bool Enable()
        {
            if (hHook != IntPtr.Zero) return true;
            hHook = SetWindowsHookEx(WH_KEYBOARD_LL, hookProc, IntPtr.Zero, 0);
            return Enabled;
        }

        public static void Disable()
        {
            if (Enabled)
            {
                foreach (int hotkeyId in hotkeys.Keys) UnregisterHotKey(windowHandle, hotkeyId);
                UnhookWindowsHookEx(hHook);
                hHook = IntPtr.Zero;
            }
        }

        public static void AddHotkey(int modifier, int key, Action handler)
        {
            RegisterHotKey(windowHandle, hotkeys.Count, modifier, key);
            Hotkey hotkey = new Hotkey { Modifier = modifier, Key = key, handler = handler };
            hotkeys[hotkeys.Count + 1] = hotkey;
        }

        public static bool RemoveHotkey(int id)
        {
            UnregisterHotKey(windowHandle, id);
            return hotkeys.Remove(id);
        }

        private static IntPtr EvtDispatcher(int nCode, IntPtr wParam, IntPtr lParam)
        {
            int msg = wParam.ToInt32();
            if (nCode >= 0 && msg == 0x100 || msg == 0x104) // Keydown message
            {
                KBDLLHOOKSTRUCT kbInfo = Marshal.PtrToStructure<KBDLLHOOKSTRUCT>(lParam);

                int kState = kbInfo.vkCode; // Build current key state (with modifiers)
                kState |= (GetAsyncKeyState(0x5B) & 0x8000) >> 4; // LWIN
                kState |= (GetAsyncKeyState(0x5c) & 0x8000) >> 4; // RWIN
                kState |= (GetAsyncKeyState(0x10) & 0x8000) >> 5; // SHIFT
                kState |= (GetAsyncKeyState(0x11) & 0x8000) >> 6; // CTRL
                kState |= (GetAsyncKeyState(0x12) & 0x8000) >> 7; // ALT

                foreach (Hotkey hk in hotkeys.Values)
                {
                    if ((hk.Modifier << 8 | hk.Key) == kState)
                        hk.handler();
                }
            }
            return CallNextHookEx(hHook, nCode, wParam, lParam);
        }

        public static void HandleKeysPressed_ALT_W()
        {
            Gui.SetTekkenWindowed();
        }

        public static void HandleKeysPressed_ALT_F()
        {
            Gui.SetTekkenFullscreen();
        }

        public static void HandleKeysPressed_ALT_C()
        {
            Gui.SetTekkenWindowed();
            Gui.OpenCommentWindow();
        }

        public static void HandleKeysPressed_ALT_O()
        {
            Gui.SetTekkenWindowed();
            PlayerList.OpenPlayerlist();
        }

        public static void HandleKeysPressed_ALT_S()
        {
            Tekken.TurnOffSilentMode();
        }

        public static void HandleKeysPressed_ALT_L()
        {
            MainWindow.fullLocation = true;
        }
    }
}
