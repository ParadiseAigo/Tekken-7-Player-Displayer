using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading;
using System.Windows;
using System.Windows.Media.Imaging;

namespace Tekken_7_Player_Displayer
{
    class Gui
    {
        public static void PrintLineToGuiConsole(string text)
        {
            PrintToGuiConsole($"{text}\r\n");
        }

        private static void PrintToGuiConsole(string text)
        {
            MainWindow.mainWindow.Dispatcher.BeginInvoke(new Action(() =>
            {
                MainWindow.mainWindow.guiConsole.Text += text;
                MainWindow.mainWindow.guiConsole.CaretIndex = MainWindow.mainWindow.guiConsole.Text.Length;
                MainWindow.mainWindow.guiConsole.ScrollToEnd();
            }));
            Console.Write(text);
        }

        public static void PrintToGuiPlayerList(String text)
        {
            MainWindow.mainWindow.Dispatcher.BeginInvoke(new Action(() =>
            {
                MainWindow.mainWindow.guiPlayerList.Text = "============ Found searching players: \r\n" + text;
                MainWindow.mainWindow.guiPlayerList.CaretIndex = MainWindow.mainWindow.guiPlayerList.Text.Length;
            }));
            Console.Write(text);
        }

        public static void PrintToGuiNextOpponent(String text)
        {
            MainWindow.mainWindow.Dispatcher.BeginInvoke(new Action(() =>
            {
                MainWindow.mainWindow.guiNextOpponent.Text = "============ Your next opponent was using:\r\n" + text;
                MainWindow.mainWindow.guiNextOpponent.CaretIndex = MainWindow.mainWindow.guiNextOpponent.Text.Length;
            }));
            Console.Write(text);

        }

        public static void UpdateGuiNextOpponent()
        {
            String newText = "";
            int charactersPadding = 1 + Pointers.ALL_CHARACTERS.OrderByDescending(s => s.Length).First().Length;
            int rankPadding = 1 + Pointers.ALL_RANKS.OrderByDescending(s => s.Length).First().Length;
            foreach (PlayerLobbyInfo player in MainWindow.ListOfPlayerLobbies)
            {
                if (player.SteamId == MainWindow.lastFoundSteamId)
                {
                    String steamId = player.SteamId.ToString() + " ";
                    String character = player.Character;
                    String rank = player.Rank;
                    String name = player.Name;
                    newText = (steamId + character.PadRight(charactersPadding) + rank.PadRight(rankPadding) + name + "\r\n");
                }
            }
            PrintToGuiNextOpponent(newText);
        }

        static public void PrintPlayerLobbyInfoList(List<PlayerLobbyInfo> theList)
        {
            String toBePrinted = "";
            int charactersPadding = 1 + Pointers.ALL_CHARACTERS.OrderByDescending(s => s.Length).First().Length;
            int rankPadding = 1 + Pointers.ALL_RANKS.OrderByDescending(s => s.Length).First().Length;
            theList = theList.OrderByDescending(x => Array.FindIndex(Pointers.ALL_RANKS, rank => rank == x.Rank)).ThenBy(x => x.Name).ToList();
            foreach (PlayerLobbyInfo player in theList)
            {
                String steamId = player.SteamId.ToString() + " ";
                String character = player.Character;
                String rank = player.Rank;
                String name = player.Name;
                String line = (steamId + character.PadRight(charactersPadding) + rank.PadRight(rankPadding) + name + "\r\n");
                toBePrinted += line;
            }
            Gui.PrintToGuiPlayerList(toBePrinted);
        }

        public static void InitOnlineModeComboBox()
        {
            MainWindow.mainWindow.Dispatcher.BeginInvoke(new Action(() =>
            {
                MainWindow.mainWindow.onlineModeComboBox.SelectedIndex = 0;
                MainWindow.mainWindow.onlineModeComboBox.Items.Add(new KeyValuePair<String, int>("Ranked", LobbyListFilters.Ranked));
                MainWindow.mainWindow.onlineModeComboBox.Items.Add(new KeyValuePair<String, int>("Quick Match", LobbyListFilters.QuickMatch));
                MainWindow.mainWindow.onlineModeComboBox.Items.Add(new KeyValuePair<String, int>("Any", LobbyListFilters.None));
            }));
        }

        /*
        public static void RefreshPlayerLobbyInfoDropDownMenu()
        {
            MainWindow.mainWindow.Dispatcher.BeginInvoke(new Action (() =>
            {
                MainWindow.mainWindow.listOfLobbies.Items.Clear();
                foreach (PlayerLobbyInfo player in MainWindow.ListOfPlayerLobbies)
                {
                    Gui.AddPlayerLobbyInfoToDropDownMenu(player);
                }
            }));
        }

        public static void AddPlayerLobbyInfoToDropDownMenu(PlayerLobbyInfo player)
        {
            //MainWindow.mainWindow.listOfLobbies.Items.Add(player.Name + " (" + player.SteamId.ToString() + ")");
            MainWindow.mainWindow.listOfLobbies.Items.Add(new KeyValuePair<String, PlayerLobbyInfo>(player.Name, player));
        }
        */

        public static void UpdateAllGuiMessages(string newOpponentName, string characterName, string playerlistComment)
        {
            MainWindow.mainWindow.Dispatcher.BeginInvoke(new Action(() =>
            {
                MainWindow.mainWindow.guiOpponentName.Text = newOpponentName;
                MainWindow.mainWindow.guiLastCharacter.Text = characterName;
                MainWindow.mainWindow.guiComment.Text = playerlistComment;
            }));
        }

        public static void CleanAllGuiMessages()
        {
            MainWindow.mainWindow.Dispatcher.BeginInvoke(new Action(() =>
            {
                MainWindow.mainWindow.guiOpponentName.Text = "";
                MainWindow.mainWindow.guiLocation.Text = "";
                MainWindow.mainWindow.guiLastCharacter.Text = "";
                MainWindow.mainWindow.guiComment.Text = "";
                MainWindow.mainWindow.guiProfilePicture.Source = null;
            }));
        }

        public static void SetOpponentNameInGui(string opponentName)
        {
            MainWindow.mainWindow.Dispatcher.BeginInvoke(new Action(() =>
            {
                MainWindow.mainWindow.guiOpponentName.Text = opponentName;
            }));
        }

        public static void SetLocationInGui(string location)
        {
            MainWindow.mainWindow.Dispatcher.BeginInvoke(new Action(() =>
            {
                MainWindow.mainWindow.guiLocation.Text = location;
            }));
        }

        public static void SetProfilePictureInGui(string picturePath)
        {
            MainWindow.mainWindow.Dispatcher.BeginInvoke(new Action(() =>
            {
                MainWindow.mainWindow.guiProfilePicture.Source = InitImage(picturePath);
            }));
        }

        /// <summary>
        /// Reads an image and allows different processes or threads to read the same picture
        /// </summary>
        private static BitmapImage InitImage(string filePath)
        {
            BitmapImage bitmapImage;
            using (BinaryReader reader = new BinaryReader(File.Open(filePath, FileMode.Open)))
            {
                FileInfo fi = new FileInfo(filePath);
                byte[] bytes = reader.ReadBytes((int)fi.Length);
                reader.Close();

                bitmapImage = new BitmapImage();
                bitmapImage.BeginInit();
                bitmapImage.StreamSource = new MemoryStream(bytes);
                bitmapImage.EndInit();
                bitmapImage.CacheOption = BitmapCacheOption.OnLoad;
                reader.Dispose();
            }
            return bitmapImage;
        }

        public static void SetTekkenWindowed()
        {
            Tekken.SetScreenMode(Pointers.SCREEN_MODE_WINDOWED);
        }

        public static void SetTekkenFullscreen()
        {
            Tekken.SetScreenMode(Pointers.SCREEN_MODE_FULLSCREEN);
        }

        public static void OpenCommentWindow()
        {
            if (!IsWindowInstantiated<CommentWindow>())
            {
                MainWindow.commentWindow = new CommentWindow();
                MainWindow.commentWindow.Show();
            }
            BringWindowToForeground(MainWindow.commentWindow);
        }

        public static bool IsWindowInstantiated<T>() where T : Window
        {
            var windows = Application.Current.Windows.Cast<Window>();
            var any = windows.Any(s => s is T);
            return any;
        }

        private static void BringWindowToForeground(Window window)
        {
            if (!window.IsVisible) window.Show();
            if (window.WindowState == WindowState.Minimized) window.WindowState = WindowState.Normal;
            window.Activate();
            window.Topmost = true;
            window.Topmost = false;
        }

        public static void PrintCannotContinueAndSleepForever()
        {
            PrintLineToGuiConsole("Impossible to continue.... (Please restart the program.)");
            SleepForever();
        }

        public static void PrintCannotContinueAndCloseProgram()
        {
            PrintLineToGuiConsole("Impossible to continue.... (Closing the program.)");
            MainWindow.mainWindow.Dispatcher.BeginInvoke(new Action(() =>
            {
                Application.Current.Shutdown();
            }));
        }

        public static void SleepForever()
        {
            while (true)
            { // let the program sleep.... forever
                Thread.Sleep(10000);
            }
        }
    }
}
