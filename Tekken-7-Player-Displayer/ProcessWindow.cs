using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace Tekken_7_Player_Displayer
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

        public static void MinimizeAndRestoreWindow(IntPtr windowHandle)
        {
            ShowWindow(windowHandle, SW_MINIMIZE);
            ShowWindow(windowHandle, SW_RESTORE);
        }
    }
}
