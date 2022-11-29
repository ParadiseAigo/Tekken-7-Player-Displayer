using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace Tekken_7_Player_Displayer
{
    /// <summary>
    /// Interaction logic for CommentWindow.xaml
    /// </summary>
    public partial class CommentWindow : Window
    {
        public CommentWindow()
        {
            InitializeComponent();

            SetWindowPositionBottomRight();
            if (MainWindow.lastNameInPlayerlist != null)
            {
                SetOpponentNameInCommentWindowTitle();
                guiCommentText.Focus();
            }
            else
            {
                DisableCommentWindowEditbox();
            }
            PreviewKeyDown += (s, e) => { if (e.Key == Key.Escape) Close(); };
        }

        private void SetWindowPositionBottomRight()
        {
            this.Top = SystemParameters.WorkArea.Height - this.Height;
            this.Left = SystemParameters.WorkArea.Width - this.Width;
        }

        private void SetOpponentNameInCommentWindowTitle()
        {
            string lastCharacter = PlayerList.GetLastCharacterInPlayerlist(Pointers.PLAYERLIST_PATH);
            this.Title = $"Comment - {MainWindow.lastNameInPlayerlist} - {lastCharacter}";
        }

        private void DisableCommentWindowEditbox()
        {
            guiCommentText.FontStyle = FontStyles.Italic;
            guiCommentText.Text = "(Last fought player not found, maybe you didn't fight a player yet)";
            guiCommentText.IsReadOnly = true;
            guiButton.Content = "  Close  ";
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            SaveCommentAndCloseCommentWindow();
        }

        private void SaveCommentAndCloseCommentWindow()
        {
            SaveComment();
            CloseCommentWindow();
        }

        private void SaveComment()
        {
            if (MainWindow.lastNameInPlayerlist != null && guiCommentText.Text != "")
            {
                string comment = guiCommentText.Text;
                Thread writeCommentThread = new Thread(() => WriteCommentToFile(comment));
                writeCommentThread.Start();
            }
        }

        private void WriteCommentToFile(string comment)
        {
            PlayerList.ReplaceCommentInLastLineInFile(Pointers.PLAYERLIST_PATH, comment);
            Gui.PrintLineToGuiConsole($"Saved comment for player \"{MainWindow.lastNameInPlayerlist}\": {comment}");
        }

        private void CloseCommentWindow()
        {
            this.Close();
        }
    }
}
