using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Tekken_7_Player_Displayer_csharp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        internal static MainWindow main;
        public static CommentWindow commentWindow;
        private static Thread mainThread;

        public static IntPtr tekkenWindowHandle;
        public static long userSteamId;
        public static long lastFoundSteamId;
        public static bool isSteamIdFound; // helps keep track of  lastFoundSteamId
        public static string lastNameInPlayerlist;
        public static string lastFoundName;
        public static bool silentMode = true;

        public static long steamModulePointer;
        public static long tekkenModulePointer;
        public static long screenModePointer;
        public static long secondsRemainingMessagePointer;

        public MainWindow()
        {
            InitializeComponent();

            InitWindows();
            InitHotkeys();
            StartMainThread();
        }

        private void InitWindows()
        {
            main = this;
            Closed += MainWindow_Closed;
        }

        private void InitHotkeys()
        {
            Hotkeys.Init();
            Hotkeys.Enable();
        }

        private void StartMainThread()
        {
            mainThread = new Thread(() => RunMainThread());
            mainThread.IsBackground = true; // this makes sure the thread will be stopped after the gui is closed
            mainThread.Start();
        }

        private void RunMainThread()
        {
            InitPlayerlist();
            LoadTargetProcess();
            EditTargetProcessLoop();
        }

        private void InitPlayerlist()
        {
            Gui.PrintToGuiConsole("Checking your list  \"Tekken Player List.txt\"  ...\r\n");
            if (!File.Exists(Pointers.PLAYERLIST_PATH))
            {
                Gui.PrintToGuiConsole("\"Tekken Player List.txt\" not found.\r\nAttempting to create a new one.\r\n");
                PlayerList.CreateExamplePlayerlist();
            }
            else
            {
                Gui.PrintToGuiConsole("Good  \"Tekken Player List.txt\"  is OK.\r\n");
            }
        }

        private void LoadTargetProcess()
        {
            InitTekkenHandle();
            InitTekkenWindowHandle();
            InitPointers();
            Tekken.CleanAllProcessMessages();
            Gui.PrintToGuiConsole("Program ready!\r\n");
        }

        private void InitTekkenHandle()
        {
            int secondsDelay = 2;
            while (!ProcessMemory.Attach(Pointers.TEKKEN_EXE_NAME))
            {
                Gui.PrintToGuiConsole($"Tekken not found. Trying again in {secondsDelay} seconds...\r\n");
                Thread.Sleep(secondsDelay * 1000);
            }
            Gui.PrintToGuiConsole($"Tekken found! pid = ({ProcessMemory.Process.Id})\r\n");
        }

        private void InitTekkenWindowHandle()
        {
            int secondsDelay = 2;
            while (null == (tekkenWindowHandle = ProcessWindow.GetWindowHandle(Pointers.TEKKEN_WINDOW_NAME)))
            {
                Gui.PrintToGuiConsole($"Searching for Tekken window. Trying again in {secondsDelay} seconds...\r\n");
                Thread.Sleep(secondsDelay * 1000); // milliseconds
            }
            Gui.PrintToGuiConsole("Tekken window found!\r\n");
        }

        private void InitPointers()
        {
            InitModuleAdresses();
            Gui.PrintToGuiConsole("Pointers loaded.\r\n");
        }

        public void InitModuleAdresses()
        {
            steamModulePointer = ProcessMemory.GetModuleAddress(Pointers.STEAM_API_MODULE_EDITED_NAME);
            if (steamModulePointer == 0)
            {
                steamModulePointer = ProcessMemory.GetModuleAddress(Pointers.STEAM_API_MODULE_NAME);
            }
            tekkenModulePointer = ProcessMemory.GetModuleAddress(Pointers.TEKKEN_MODULE_NAME);
            if (steamModulePointer == 0)
            {
                Gui.PrintToGuiConsole("Error: failed to get the module base address of the steam api.\r\n");
            }
            if (tekkenModulePointer == 0)
            {
                Gui.PrintToGuiConsole("Error: failed to get the module base address of tekken.\r\n");
            }
            if (tekkenModulePointer == 0 || steamModulePointer == 0)
            {
                Gui.PrintToGuiConsole("Impossible to continue... please restart this program.\r\n");
                while (true)
                { // let the program sleep.... forever
                    Thread.Sleep(10000);
                }
            }
            long userSteamIdAddress = ProcessMemory.GetDynamicAddress(steamModulePointer + Pointers.STEAM_ID_USER_STATIC_POINTER, Pointers.STEAM_ID_USER_POINTER_OFFSETS);
            userSteamId = ProcessMemory.ReadMemory<long>(userSteamIdAddress);
            screenModePointer = ProcessMemory.GetDynamicAddress(tekkenModulePointer + Pointers.SCREEN_MODE_STATIC_POINTER, Pointers.SCREEN_MODE_POINTER_OFFSETS);
        }
        
        private void EditTargetProcessLoop()
        {
            bool areMessagesClean;
            int delayWhileSearching = 1000 / 10; // "10fps" (updates 10 times a sec)
            int delayWhileFighting = 2000; // 2 seconds, unused variable :(
            lastFoundSteamId = -1;  // global variable
            isSteamIdFound = false; // global variable
            lastFoundName = ""; // global variable
            areMessagesClean = true;
            lastNameInPlayerlist = PlayerList.GetLastNameInPlayerlist(Pointers.PLAYERLIST_PATH); //global variable  // set equal to NULL if player list is empty
            while (true)
            {
                Thread.Sleep(delayWhileSearching);
                //UpdateOpponentNameTwo(); // todo remove
                if (Tekken.IsNewOpponentReceived())
                {
                    Tekken.CleanAllProcessMessages();
                    Gui.CleanAllGuiMessages();
                    Tekken.HandleNewReceivedOpponent();
                    Tekken.DisplayOpponentInfoFromWeb(lastFoundSteamId);
                    areMessagesClean = false;
                }
                else if ((!areMessagesClean) && (!isSteamIdFound))
                {
                    Tekken.CleanAllProcessMessages();
                    Gui.CleanAllGuiMessages();
                    areMessagesClean = true;
                }
                if (Tekken.IsNewFightAccepted())
                {
                    lastNameInPlayerlist = PlayerList.SaveNewOpponentInPlayerlist();
                }
                if (DidSomeoneCloseTekkenWindow())
                {
                    RestartProgram();
                }
            }
        }

        private bool DidSomeoneCloseTekkenWindow()
        {
            return !ProcessWindow.IsWindow(tekkenWindowHandle);
        }

        private void RestartProgram()
        {
            Thread.Sleep(3000); // wait to make sure the tekken process has closed after the window was closed
            LoadTargetProcess();
        }

        void MainWindow_Closed(object sender, EventArgs e)
        {
            Hotkeys.Disable();
            if (Gui.IsWindowInstantiated<CommentWindow>()) commentWindow.Close();
        }
    }
}

