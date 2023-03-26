using Steamworks;
using System;
using System.IO;
using System.Linq;
using System.Net;
using System.Threading;
using System.Windows;
using System.Collections.Generic;

namespace Tekken_7_Player_Displayer
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private const string mainWindowTitle = "Tekken 7 - Player Displayer  |  v1.4.6";
        public static MainWindow mainWindow;
        public static CommentWindow commentWindow;
        private static Thread mainThread;

        public static IntPtr tekkenWindowHandle;
        public static long userSteamId;
        public static long lastFoundSteamId;
        public static bool isSteamIdFound; // helps keep track of  lastFoundSteamId
        public static string lastNameInPlayerlist;
        public static string lastFoundName;
        public static bool silentMode = true;
        public static bool fullLocation = false;

        public static long steamModulePointer;
        public static long tekkenModulePointer;
        public static long screenModePointer;
        public static long secondsRemainingMessagePointer;

        public static CallResult<LobbyMatchList_t> CallResultLobbyMatchList;
        public static List<PlayerLobbyInfo> ListOfPlayerLobbies;
        //public static PlayerLobbyInfo SelectedPlayer; //JoinLobby()
        public static int OnlineModeFilter;

        public MainWindow()
        {
            InitializeComponent();

            InitMainWindow();
            StartMainThread();
        }

        private void InitMainWindow()
        {
            AppDomain currentDomain = AppDomain.CurrentDomain;
            currentDomain.UnhandledException += new UnhandledExceptionEventHandler(MyUnhandledExceptionHandler);
            mainWindow = this;
            mainWindow.Title = mainWindowTitle;
            SetWindowPosition();
            Closed += MainWindow_Closed;
            InitHotkeys();
        }

        static void MyUnhandledExceptionHandler(object sender, UnhandledExceptionEventArgs args)
        {
            Exception e = (Exception)args.ExceptionObject;
            string errorMessage = "An unhandled exception occurred: " + e.Message + "\n"
                                + e.StackTrace + "\n"
                                + "Runtime terminating: " + args.IsTerminating;
            string title = "Exception - " + mainWindowTitle;
            MessageBox.Show(errorMessage, title, MessageBoxButton.OK, MessageBoxImage.Error);
            Console.WriteLine(errorMessage);
        }

        private void SetWindowPosition()
        {
            if (Double.TryParse(Settings.Default.WindowPositionX, out double savedWindowPositionX) &&
                Double.TryParse(Settings.Default.WindowPositionY, out double savedWindowPositionY))
            {
                Rect savedPositionRect = new Rect(savedWindowPositionX, savedWindowPositionY, this.Width, this.Height);
                Rect virtualScreenRect = new Rect(SystemParameters.VirtualScreenLeft, SystemParameters.VirtualScreenTop, SystemParameters.VirtualScreenWidth, SystemParameters.VirtualScreenHeight);
                if (savedPositionRect.IntersectsWith(virtualScreenRect))
                {
                    this.Left = savedWindowPositionX;
                    this.Top = savedWindowPositionY;
                }
            }
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
            InitGlobalVariables();
            LoadTargetProcess();
            StartLobbyInfoThread();
            EditTargetProcessLoop();
        }

        private void InitPlayerlist()
        {
            Gui.PrintLineToGuiConsole("Checking your list  \"Tekken Player List.txt\"  ...");
            if (!File.Exists(Pointers.PLAYERLIST_PATH))
            {
                Gui.PrintLineToGuiConsole("\"Tekken Player List.txt\" not found.\r\nAttempting to create a new one.");
                PlayerList.CreateExamplePlayerlist();
            }
            else
            {
                Gui.PrintLineToGuiConsole("Good  \"Tekken Player List.txt\"  is OK.");
            }
        }

        private void InitGlobalVariables()
        {
            MainWindow.lastFoundSteamId = -1;
            MainWindow.isSteamIdFound = false;
            MainWindow.lastFoundName = "";
            MainWindow.lastNameInPlayerlist = PlayerList.GetLastNameInPlayerlist(Pointers.PLAYERLIST_PATH); //global variable  // set equal to NULL if player list is empty
            MainWindow.CallResultLobbyMatchList = CallResult<LobbyMatchList_t>.Create(SteamworksAPI.MyCallbackLobbyMatchList);
            MainWindow.ListOfPlayerLobbies = new List<PlayerLobbyInfo>();
            //MainWindow.SelectedPlayer = null; // JoinLobby()
            MainWindow.OnlineModeFilter = LobbyListFilters.Ranked;
        }

        private void LoadTargetProcess()
        {
            InitTekkenHandle();
            InitTekkenWindowHandle();
            InitPointers();
            InitSteamworksAPI();
            Tekken.CleanAllProcessMessages();
            Gui.PrintLineToGuiConsole("Program ready!");
        }

        private void InitTekkenHandle()
        {
            int secondsDelay = 2;
            while (!ProcessMemory.Attach(Pointers.TEKKEN_EXE_NAME))
            {
                Gui.PrintLineToGuiConsole($"Tekken not found. Trying again in {secondsDelay} seconds...");
                Thread.Sleep(secondsDelay * 1000);
            }
            Gui.PrintLineToGuiConsole($"Tekken found! (pid = {ProcessMemory.Process.Id})");
        }

        private void InitTekkenWindowHandle()
        {
            int secondsDelay = 2;
            while (null == (tekkenWindowHandle = ProcessWindow.GetWindowHandle(Pointers.TEKKEN_WINDOW_NAME))
                    || !ProcessWindow.IsWindow(tekkenWindowHandle))
            {
                Gui.PrintLineToGuiConsole($"Searching for Tekken window. Trying again in {secondsDelay} seconds...");
                Thread.Sleep(secondsDelay * 1000); // milliseconds
            }
            Gui.PrintLineToGuiConsole($"Tekken window found! ({tekkenWindowHandle})");
        }

        private void InitPointers()
        {
            InitModuleAdresses();
            Gui.PrintLineToGuiConsole("Pointers loaded.");
        }

        public void InitModuleAdresses()
        {
            int secondsDelay = 2;
            steamModulePointer = 0;
            while (steamModulePointer == 0)
            {
                steamModulePointer = ProcessMemory.GetModuleAddress(Pointers.STEAM_API_MODULE_EDITED_NAME);
                if (steamModulePointer != 0)
                    continue;
                steamModulePointer = ProcessMemory.GetModuleAddress(Pointers.STEAM_API_MODULE_NAME);
                if (steamModulePointer != 0)
                    continue;
                Gui.PrintLineToGuiConsole($"Getting the Tekken Steam module base address. Trying again in {secondsDelay} seconds...");
                Thread.Sleep(secondsDelay * 1000); // milliseconds
            }
            tekkenModulePointer = 0;
            while (tekkenModulePointer == 0)
            {
                tekkenModulePointer = ProcessMemory.GetModuleAddress(Pointers.TEKKEN_MODULE_NAME);
                if (tekkenModulePointer != 0)
                    continue;
                Gui.PrintLineToGuiConsole($"Getting the Tekken (inside the Tekken process) module base address. Trying again in {secondsDelay} seconds...");
                Thread.Sleep(secondsDelay * 1000); // milliseconds
            }
            long userSteamIdAddress = ProcessMemory.GetDynamicAddress(steamModulePointer + Pointers.STEAM_ID_USER_STATIC_POINTER, Pointers.STEAM_ID_USER_POINTER_OFFSETS);
            userSteamId = ProcessMemory.ReadMemory<long>(userSteamIdAddress);
            screenModePointer = ProcessMemory.GetDynamicAddress(tekkenModulePointer + Pointers.SCREEN_MODE_STATIC_POINTER, Pointers.SCREEN_MODE_POINTER_OFFSETS);
        }

        private void InitSteamworksAPI()
        {
            if (!SteamworksAPI.Init(Pointers.TEKKEN_STEAM_APP_ID))
            {
                Gui.PrintLineToGuiConsole("Error: failed to initialize steam api.");
                Gui.PrintCannotContinueAndSleepForever();
            }
        }

        private void StartLobbyInfoThread()
        {
            Gui.InitOnlineModeComboBox();
            Gui.PrintToGuiPlayerList("");
            Gui.PrintToGuiNextOpponent("");
            Thread lobbyInfoThread = new Thread(() =>
            {
                while (true) {
                    Thread.Sleep(3000);
                    if (MainWindow.OnlineModeFilter == LobbyListFilters.Off) continue;
                    SteamworksAPI.SavePlayerLobbies(MainWindow.CallResultLobbyMatchList);
                    Gui.PrintPlayerLobbyInfoList(MainWindow.ListOfPlayerLobbies);
                    //Gui.RefreshPlayerLobbyInfoDropDownMenu(); // JoinLobby()
                }
            }
            );
            lobbyInfoThread.IsBackground = true; // this makes sure the thread will be stopped after the gui is closed
            lobbyInfoThread.Start();
        }

        private void EditTargetProcessLoop()
        {
            bool areMessagesClean = true;
            int delayWhileSearching = 1000 / 10; // in milliseconds, "10fps" (updates 10 times a sec)
            int delayWhileFighting = 2000; // 2 seconds, unused variable

            while (true)
            {
                Thread.Sleep(delayWhileSearching);
                Gui.UpdateGuiNextOpponent();
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
                    Gui.PrintLineToGuiConsole("Tekken window closed. (Can't find it anymore.)");
                    Gui.PrintCannotContinueAndCloseProgram();
                }
            }
        }

        private bool DidSomeoneCloseTekkenWindow()
        {
            int delay = 2;
            if (!ProcessWindow.IsWindow(tekkenWindowHandle)) // has the window been closed?
            {
                Gui.PrintLineToGuiConsole($"Has the Tekken window been closed? Waiting {delay} seconds before checking again.");
                Thread.Sleep(delay * 1000); // wait then try again to make sure
            }
            return !ProcessWindow.IsWindow(tekkenWindowHandle);
        }

        void MainWindow_Closed(object sender, EventArgs e)
        {
            SaveWindowPosition();
            Hotkeys.Disable();
            if (Gui.IsWindowInstantiated<CommentWindow>()) commentWindow.Close();
            SteamworksAPI.Shutdown();
        }

        private void SaveWindowPosition()
        {
            Settings.Default.WindowPositionX = this.Left.ToString();
            Settings.Default.WindowPositionY = this.Top.ToString();
            Settings.Default.Save();
        }

        private void onlineModeComboBox_SelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
           if (MainWindow.mainWindow == null) return;
            int newFilter = ((KeyValuePair<String, int>) MainWindow.mainWindow.onlineModeComboBox.SelectedItem).Value;
            MainWindow.OnlineModeFilter = newFilter;
            MainWindow.ListOfPlayerLobbies.Clear();
            if (MainWindow.OnlineModeFilter != LobbyListFilters.Off)
            {
                Gui.PrintLineToGuiConsole("Showing players in "
                    + ((KeyValuePair<String, int>) MainWindow.mainWindow.onlineModeComboBox.SelectedItem).Key
                    + "." );
            }
            Gui.PrintPlayerLobbyInfoList(MainWindow.ListOfPlayerLobbies);
            //Gui.RefreshPlayerLobbyInfoDropDownMenu(); //JoinLobby()
        }

        // JoinLobby()
        /*
        private void joinButton_Click(object sender, RoutedEventArgs e)
        {
            if (MainWindow.SelectedPlayer == null)
            {
                Gui.PrintLineToGuiConsole("Can't join. No player selected.");
                return;
            }
            Gui.PrintLineToGuiConsole("Joining: " + MainWindow.SelectedPlayer.Name);
            if (false == SteamMatchmaking.RequestLobbyData(MainWindow.SelectedPlayer.LobbyId))
            {
                Gui.PrintLineToGuiConsole("Joining is no longer possible.");
            }
            else
            {
                SteamMatchmaking.JoinLobby(MainWindow.SelectedPlayer.LobbyId);
            }
        }

        private void listOfLobbies_SelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
            if (mainWindow.listOfLobbies.SelectedItem == null) return;
            MainWindow.SelectedPlayer = ((KeyValuePair<String, PlayerLobbyInfo>) mainWindow.listOfLobbies.SelectedItem).Value;
            Gui.PrintLineToGuiConsole("Selected player: " + MainWindow.SelectedPlayer.Name);
        }
        */


        // JoinLobby() xaml
        /*
        <ComboBox x:Name="listOfLobbies" Grid.Column="1" Grid.Row="9" Width="150" Height="20" HorizontalAlignment="Left" VerticalAlignment="Top" Margin="0,20,0,0" SelectionChanged="listOfLobbies_SelectionChanged" DisplayMemberPath="Key" SelectedValuePath="Value"></ComboBox>
        <Button x:Name="joinButton" Grid.Column="1" Grid.Row="9" Width="100" Height="20" HorizontalAlignment="Right" VerticalAlignment="Top" Margin="0,20,40,0" Click="joinButton_Click">Join</Button>
        */
    }
}
