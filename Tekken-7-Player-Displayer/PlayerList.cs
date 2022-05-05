using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;

namespace Tekken_7_Player_Displayer_csharp
{
    class PlayerList
    {
        public static void CreateExamplePlayerlist()
        {
            int paul = 0;
            int hwo = 4;
            string line1 = MakePlayerlistEntry("narcissist_city", Pointers.ALL_CHARACTERS[paul], 76561197960265729);
            WriteLineToFile(Pointers.PLAYERLIST_PATH, line1);
            ReplaceCommentInLastLineInFile(Pointers.PLAYERLIST_PATH, "toxic plugger stay away");
            string line2 = MakePlayerlistEntry("bigboss99", Pointers.ALL_CHARACTERS[hwo], 76561197960265730);
            WriteLineToFile(Pointers.PLAYERLIST_PATH, line2);
        }

        public static string ExtractPlayerNameFromPlayerlistLine(string line)
        {
            int lineIndexBegin = 0;  // player name is the first thing in the line so its always = 0
            int lineIndexEnd = 0;
            int lineSize = line.Length;
            string result = "";
            if (line != null && line != "")
            {
                while ((lineIndexEnd + 2) < lineSize)
                {
                    if (line[lineIndexEnd] != '\t')
                    {
                        lineIndexEnd++;
                    }
                    else
                    {
                        break;
                    }
                }
                result = line.Substring(lineIndexBegin, lineIndexEnd - lineIndexBegin);
                return result;
            }
            return result;
        }

        public static string ExtractCommentFromPlayerlistLine(string line)
        {
            int lineIndex = 0;
            int lineSize = line.Length;
            int tabsCount = 0;
            if (line != null)
            {
                while (tabsCount < 3 && (lineIndex + 2) < lineSize)
                {
                    if (line[lineIndex] == '\t')
                    {
                        if (line[lineIndex + 1] != '\t')
                        {
                            tabsCount++;
                        }
                    }
                    else
                    {
                        // ignore mutliple tabs
                    }
                }
                lineIndex++;
            }
            return line.Substring(lineIndex, lineSize);
        }

        public static string ExtractCharacterFromPlayerlistLine(string line)
        {
            int lineIndexBegin = 0;
            int lineIndexEnd = 0;
            int lineSize = line.Length;
            int tabsCount = 0;
            if (line != null)
            {
                while (tabsCount < 1 && (lineIndexBegin + 2) < lineSize)
                {
                    if (line[lineIndexBegin] == '\t')
                    {
                        if (line[lineIndexBegin + 1] != '\t')
                        {
                            tabsCount++;
                        }
                        else
                        {
                            // ignore mutliple tabs
                        }
                    }
                    lineIndexBegin++;
                }
                lineIndexEnd = lineIndexBegin;
                while ((lineIndexEnd + 2) < lineSize)
                {
                    if (line[lineIndexEnd] != '\t')
                    {
                        lineIndexEnd++;
                    }
                    else
                    {
                        break;
                    }
                }
                return line.Substring(lineIndexBegin, lineIndexEnd - lineIndexBegin);
            }
            return null;
        }

        public static string SaveNewOpponentInPlayerlist()
        {
            string currentLoadedOpponentName = Tekken.GetNewCurrentLoadedOpponent();
            SaveNewPlayerlistEntry(currentLoadedOpponentName);
            return currentLoadedOpponentName;
        }

        public static void SaveNewPlayerlistEntry(string currentLoadedOpponentName)
        {
            long opponentStructCharacterAddress = ProcessMemory.GetDynamicAddress(MainWindow.tekkenModulePointer + Pointers.OPPONENT_STRUCT_CHARACTER_STATIC_POINTER, Pointers.OPPONENT_STRUCT_CHARACTER_POINTER_OFFSETS);
            long newOpponentStructCharacter = ProcessMemory.ReadMemory<long>(opponentStructCharacterAddress);
            long steamId = MainWindow.lastFoundSteamId;
            Gui.PrintToGuiConsole($"New opponent    : {currentLoadedOpponentName}\r\n");
            Gui.PrintToGuiConsole($"Character id    : {newOpponentStructCharacter}\r\n");
            Gui.PrintToGuiConsole($"Steam id        : {steamId}\r\n");
            if (newOpponentStructCharacter != 255)
            {
                Gui.PrintToGuiConsole($"Character name:   {Pointers.ALL_CHARACTERS[newOpponentStructCharacter]}\r\n");
                string newPlayerlistLine = PlayerList.MakePlayerlistEntry(currentLoadedOpponentName, Pointers.ALL_CHARACTERS[newOpponentStructCharacter], steamId);
                Gui.PrintToGuiConsole($"Playerlist entry: {newPlayerlistLine}\r\n");
                PlayerList.WriteLineToFile(Pointers.PLAYERLIST_PATH, newPlayerlistLine);
            }
        }

        public static string MakePlayerlistEntry(string playerName, string characterName, long steamId)
        {
            return playerName + "\t\t\t" + characterName + "\t\t\t" + steamId + "\t\t\t" + "no comment yet";
        }

        public static string GetLastNameInPlayerlist(string filePath)
        {
            string lastLineInPlayerlist = GetLastLineOfFile(filePath);
            string lastNameInPlayerlist = ExtractPlayerNameFromPlayerlistLine(lastLineInPlayerlist);
            return lastNameInPlayerlist;
        }

        public static string GetLastLineOfFile(string filePath)
        {
            return File.ReadLines(filePath).Last();
        }

        public static async void WriteLineToFile(string path, string line)
        {
            using StreamWriter streamWriter = new StreamWriter(path, append: true);
            await streamWriter.WriteLineAsync(line);
        }

        public static void ReplaceCommentInLastLineInFile(string path, string comment)
        {
            long indexCommentEnd = WriteAfterLastOccurenceOfCharInFile(path, comment, '\t');
            if (indexCommentEnd > 0) SetEndOfFileAtIndex(path, indexCommentEnd);
        }

        private static long WriteAfterLastOccurenceOfCharInFile(string path, string text, char charToWriteAfter)
        {
            long position;
            using (Stream stream = new FileStream(path, FileMode.OpenOrCreate))
            {
                stream.Seek(0, SeekOrigin.End);
                stream.Seek(-1, SeekOrigin.Current);
                int byteRead = stream.ReadByte();  // this does stream.Seek(+1, SeekOrigin.Current)
                stream.Seek(-1, SeekOrigin.Current);
                while (byteRead != charToWriteAfter && stream.Position > 0)
                {
                    stream.Seek(-1, SeekOrigin.Current);
                    byteRead = stream.ReadByte();
                    stream.Seek(-1, SeekOrigin.Current);
                }
                stream.Seek(1, SeekOrigin.Current); // move one after charToWriteAfter to not overwrite it
                byte[] bytes = Encoding.ASCII.GetBytes(text);
                stream.Write(bytes, 0, bytes.Length);
                position = stream.Position;
            }
            return position;
        }

        private static void SetEndOfFileAtIndex(string path, long index)
        {
            using (Stream stream = new FileStream(path, FileMode.OpenOrCreate))
            {
                stream.SetLength(index);
                byte[] bytes = Encoding.ASCII.GetBytes(Environment.NewLine);
                stream.Seek(0, SeekOrigin.End);
                stream.Write(bytes, 0, bytes.Length);
            }
        }

        public static void OpenPlayerlist()
        {
            Gui.PrintToGuiConsole("Opening playerlist...\r\n");
            if (File.Exists(Pointers.PLAYERLIST_PATH))
            {
                ProcessWindow.SetScreenMode(Pointers.SCREEN_MODE_WINDOWED);
                new Process { 
                    StartInfo = new ProcessStartInfo(Pointers.PLAYERLIST_PATH) { UseShellExecute = true } 
                }.Start();
            }
            else
            {
                Gui.PrintToGuiConsole("Playerlist is not found, creating a new one...\r\n");
                CreateFile(Pointers.PLAYERLIST_PATH);
            }
        }

        private static void CreateFile(string filePath)
        {
            try
            {
                using (File.Create(filePath)) {}
                Gui.PrintToGuiConsole($"File \"{filePath}\" created.\r\n");
            }
            catch (Exception ex)
            {
                Gui.PrintToGuiConsole($"Error in CreateFile: Error creating the file \"{filePath}\": {ex.Message}\r\n");
            }
        }


        public static string GetLastCharacterInPlayerlist(string filePath)
        {
            string lastLineInPlayerlist = GetLastLineOfFile(filePath);
            string lastCharacterInPlayerlist = ExtractCharacterFromPlayerlistLine(lastLineInPlayerlist);
            return lastCharacterInPlayerlist;
        }
    }
}
