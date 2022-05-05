using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace Tekken_7_Player_Displayer_csharp
{
    class ProcessWindow
    {
        [DllImport("user32")]
        public static extern IntPtr FindWindow(string lpClassName, string lpWindowName);
        
        [DllImport("user32")]
        public static extern bool IsWindow(IntPtr hWnd);

        [DllImport("user32")]
        private static extern int ShowWindow(IntPtr hWnd, int nCmdShow);

        public const int SW_SHOW = 5;
        public const int SW_MINIMIZE = 6;
        public const int SW_RESTORE = 9;


        public static IntPtr GetWindowHandle(string windowName)
        {
            return FindWindow(null, windowName);
        }
        public static void SetScreenMode(int newScreenMode)
        {
            if (MainWindow.tekkenWindowHandle != IntPtr.Zero)
            {
                int screenMode;
                string screenModeInEnglish;
                switch (newScreenMode)
                {
                    case var value when value == Pointers.SCREEN_MODE_FULLSCREEN:
                        screenModeInEnglish = "fullscreen";
                        break;
                    case var value when value == Pointers.SCREEN_MODE_BORDERLESS:
                        screenModeInEnglish = "borderless";
                        break;
                    case var value when value == Pointers.SCREEN_MODE_WINDOWED:
                        screenModeInEnglish = "windowed";
                        break;
                    default:
                        screenModeInEnglish = "unknown";
                        break;
                }
                screenMode = ProcessMemory.ReadMemory<int>(MainWindow.screenModePointer);
                if (screenMode != newScreenMode)
                {
                    ProcessMemory.WriteMemory<int>(MainWindow.screenModePointer, newScreenMode);
                    MinimizeAndRestoreTekkenWindow();
                    Gui.PrintToGuiConsole($"Screen mode set to {screenModeInEnglish}\r\n");
                }
            }
        }

        private static void MinimizeAndRestoreTekkenWindow()
        {
            ShowWindow(MainWindow.tekkenWindowHandle, SW_MINIMIZE);
            ShowWindow(MainWindow.tekkenWindowHandle, SW_RESTORE);
        }
    }
}
