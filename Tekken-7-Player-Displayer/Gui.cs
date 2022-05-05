using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media.Imaging;
using System.Windows.Threading;

namespace Tekken_7_Player_Displayer_csharp
{
    class Gui
    {
        public static void PrintToGuiConsole(string text)
        {
            MainWindow.main.Dispatcher.BeginInvoke(new Action(() =>
            {
                MainWindow.main.guiConsole.Text += text;
                MainWindow.main.guiConsole.ScrollToEnd();
            }));
            Console.Write(text);
        }

        public static void UpdateAllGuiMessages(string newOpponentName, string characterName, string playerlistComment)
        {
            MainWindow.main.Dispatcher.BeginInvoke(new Action(() =>
            {
                MainWindow.main.guiOpponentName.Text = newOpponentName;
                MainWindow.main.guiLastCharacter.Text = characterName;
                MainWindow.main.guiComment.Text = playerlistComment;
            }));
        }

        public static void CleanAllGuiMessages()
        {
            MainWindow.main.Dispatcher.BeginInvoke(new Action(() =>
            {
                MainWindow.main.guiOpponentName.Text = "";
                MainWindow.main.guiLastCharacter.Text = "";
                MainWindow.main.guiComment.Text = "";
                MainWindow.main.guiProfilePicture.Source = null;
            }));
        }

        public static void SetOpponentNameInGui(string opponentName)
        {
            MainWindow.main.Dispatcher.BeginInvoke(new Action(() =>
            {
                MainWindow.main.guiOpponentName.Text = opponentName;
            }));
        }

        public static void SetProfilePictureInGui(string picturePath)
        {
            MainWindow.main.Dispatcher.BeginInvoke(new Action(() =>
            {
                MainWindow.main.guiProfilePicture.Source = InitImage(picturePath);
            }));
        }

        /// <summary>
        /// Allows different processes or threads to read the same picture
        /// </summary>
        /// <param name="filePath"></param>
        /// <returns></returns>
        private static BitmapImage InitImage(string filePath)
        {
            BitmapImage bitmapImage;
            using (BinaryReader reader = new BinaryReader(File.Open(filePath, FileMode.Open)))
            {
                FileInfo fi = new FileInfo(filePath);
                byte[] bytes = reader.ReadBytes((int)fi.Length);
                reader.Close();

                //image = new Image();
                bitmapImage = new BitmapImage();
                bitmapImage.BeginInit();
                bitmapImage.StreamSource = new MemoryStream(bytes);
                bitmapImage.EndInit();
                bitmapImage.CacheOption = BitmapCacheOption.OnLoad;
                //image.Source = bitmapImage;
                reader.Dispose();
            }
            return bitmapImage;
        }

        public static void SetTekkenWindowed()
        {
            ProcessWindow.SetScreenMode(Pointers.SCREEN_MODE_WINDOWED);
        }

        public static void SetTekkenFullscreen()
        {
            ProcessWindow.SetScreenMode(Pointers.SCREEN_MODE_FULLSCREEN);
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
    }
}
