using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;

namespace Tekken_7_Player_Displayer_csharp
{
    class Tekken
    {
        public static bool IsNewOpponentReceived()
        {
            return IsNewSteamIdReceived();
        }

        public static bool IsNewSteamIdReceived()
        {
            long steamIdPointer = ProcessMemory.GetDynamicAddress(MainWindow.steamModulePointer + Pointers.STEAM_ID_BETTER_STATIC_POINTER, Pointers.STEAM_ID_BETTER_POINTER_OFFSETS);
            if (!IsSteamApiLoaded(steamIdPointer))								// if steam api not loaded
            {
                ResetSteamApiBaseModuleAddress();
                MainWindow.isSteamIdFound = false;
                MainWindow.lastFoundSteamId = -1;
                return false;
            }
            long newFoundSteamId = ReadSteamIdIfValid(steamIdPointer);
            if (newFoundSteamId == -1)                                         // if steam id not valid
            {
                MainWindow.isSteamIdFound = false;
                MainWindow.lastFoundSteamId = -1;
                return false;
            }
            else if (MainWindow.userSteamId == newFoundSteamId)                            // if equal to user
            {
                MainWindow.isSteamIdFound = false;
                MainWindow.lastFoundSteamId = -1;
                return false;
            }
            else if (newFoundSteamId == MainWindow.lastFoundSteamId)                       // if unchanged
            {
                MainWindow.isSteamIdFound = true;
                return false;
            }
            else                                                                // new steam id received!
            {
                Gui.PrintToGuiConsole($"New Steam id found: {newFoundSteamId}\r\n");
                MainWindow.isSteamIdFound = true;
                MainWindow.lastFoundSteamId = newFoundSteamId;
                return true;
            }
        }

        private static bool IsSteamApiLoaded(long steamIdAddress)
        {
            return steamIdAddress != 0;
        }

        private static void ResetSteamApiBaseModuleAddress()
        {
            Gui.PrintToGuiConsole("Resetting Steam api module base address.\r\n");
            MainWindow.main.InitModuleAdresses();
        }

        private static long ReadSteamIdIfValid(long steamIdAddress)
        {
            if (!ProcessMemory.IsMemoryReadable(steamIdAddress))
            {
                // aigo debugging
                //PrintToGuiConsole("Steam Id (memory) not readable (probably late).\r\n");
                return -1;
            }
            long steamId = ProcessMemory.ReadMemory<long>(steamIdAddress);
            if (steamId < 0x0110000100000000 || steamId > 0x0110000200000000)
            {
                // aigo debugging
                //		PrintToGuiConsole($"Steam Id is bad!: {steamIdBuffer}\r\n");
                // steam id is 64 bits (8 bytes) long: 0x FF FF FF FF FF FF FF FF
                // first 8 bits represent the "universe" and its always equal to 1 for normal accounts
                // next 4 bits represent the "type" and its also always equal to 1 for normal accounts
                // next 20 bits represent the "instance" and its also always equal to 1 for normal accounts
                // final 32 bits (4 bytes) represent the real steam id
                // so a full steam id in hex looks like this (with X variable)  0x 01 10 00 01 XX XX XX XX
                return -1;
            }
            return steamId;
        }

        public static void HandleNewReceivedOpponent()
        {
            string newOpponentNameMessage;
            string characterName;
            string playerlistComment;

            string line = File.ReadAllLines(Pointers.PLAYERLIST_PATH).Where(i => i.Contains(MainWindow.lastFoundSteamId.ToString())).ToList().FirstOrDefault();
            if (line != null)
            {  // steam id found in player list!
                Gui.PrintToGuiConsole("Steam id found in player list!\r\n");
                newOpponentNameMessage = PlayerList.ExtractPlayerNameFromPlayerlistLine(line);
                playerlistComment = PlayerList.ExtractCommentFromPlayerlistLine(line);
                characterName = PlayerList.ExtractCharacterFromPlayerlistLine(line);
                Gui.PrintToGuiConsole($"New opponent found:  {newOpponentNameMessage}\r\n");
                Gui.PrintToGuiConsole($"Last used character: {characterName}\r\n");
                Gui.PrintToGuiConsole($"Comment:             {playerlistComment}\r\n");
            }
            else
            {  // steam id NOT found in player list!
                Gui.PrintToGuiConsole("Steam id NOT found in player list!\r\n");
                newOpponentNameMessage = "New (not in list)";
                playerlistComment = "Brand new opponent!";
                characterName = "?";
            }
            Gui.UpdateAllGuiMessages(newOpponentNameMessage, characterName, playerlistComment);
            Tekken.UpdateAllInGameMessages(newOpponentNameMessage, characterName, playerlistComment);
        }

        public static void UpdateAllInGameMessages(string newOpponentNameMessage, string characterName, string playerlistComment)
        {
            UpdateSecondsRemainingMessage(newOpponentNameMessage);
        }

        public static void UpdateSecondsRemainingMessage(string message)
        {
            MainWindow.secondsRemainingMessagePointer = ProcessMemory.GetDynamicAddress(MainWindow.tekkenModulePointer + Pointers.SECONDS_REMAINING_MESSAGE_STATIC_POINTER, Pointers.SECONDS_REMAINING_MESSAGE_POINTER_OFFSETS);
            if (!MainWindow.silentMode)
            {
                ProcessMemory.WriteMemory<string>(MainWindow.secondsRemainingMessagePointer, message);
            }
        }

        public static bool IsNewFightAccepted()
        {
            return IsNewOpponentLoaded();
        }

        public static bool IsNewOpponentLoaded()
        {
            int numberOfCharacters = Pointers.ALL_CHARACTERS.Length;
            long opponentStructNameAddress = ProcessMemory.GetDynamicAddress(MainWindow.tekkenModulePointer + Pointers.OPPONENT_STRUCT_NAME_STATIC_POINTER, Pointers.OPPONENT_STRUCT_NAME_POINTER_OFFSETS);
            long opponentStructCharacterAddress = ProcessMemory.GetDynamicAddress(MainWindow.tekkenModulePointer + Pointers.OPPONENT_STRUCT_CHARACTER_STATIC_POINTER, Pointers.OPPONENT_STRUCT_CHARACTER_POINTER_OFFSETS);
            if ((!ProcessMemory.IsMemoryReadable(opponentStructNameAddress)) ||
                (!ProcessMemory.IsMemoryReadable(opponentStructCharacterAddress)))
            {
                return false;
            }
            string opponentStructName = ProcessMemory.ReadString(opponentStructNameAddress);
            int opponentStructCharacter = ProcessMemory.ReadMemory<int>(opponentStructCharacterAddress);
            // aigo debugging (delete this)
            //	PrintToGuiConsole($"opponentstructcharacter = {opponentStructCharacter} , opponentstructname = {opponentStructName}\r\n";
            if (opponentStructName != "NOT_LOGGED_IN" &&
                (opponentStructName[0] != '\0') &&
                (MainWindow.isSteamIdFound == true) &&
                (opponentStructCharacter < numberOfCharacters) &&
                (opponentStructCharacter >= 0) && // 0 is paul ^_^
                (MainWindow.lastNameInPlayerlist != opponentStructName))
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        public static void CleanAllProcessMessages()
        {
            UpdateSecondsRemainingMessage("Failed to get the name.");
        }

        public static string GetNewCurrentLoadedOpponent()
        {
            long opponentStructNameAddress = ProcessMemory.GetDynamicAddress(MainWindow.tekkenModulePointer + Pointers.OPPONENT_STRUCT_NAME_STATIC_POINTER, Pointers.OPPONENT_STRUCT_NAME_POINTER_OFFSETS);
            string newOpponentStructName = ProcessMemory.ReadString(opponentStructNameAddress);
            return newOpponentStructName;
        }
        public static void DisplayOpponentInfoFromWeb(long steamId)
        {
            string htmlString = SteamURL.GetSteamPageHtml(steamId);
            string name = SteamURL.ExtractNameFromSteamHtmlString(htmlString);
            string pictureLink = SteamURL.ExtractProfilePictureUrlFromSteamHtmlString(htmlString);
            DisplayOpponentNameFromWeb(name);
            DisplayOpponentProfilePictureFromWeb(pictureLink);
            Tekken.UpdateSecondsRemainingMessage(name);
        }

        public static void DisplayOpponentNameFromWeb(string opponentName)
        {
            Gui.PrintToGuiConsole($"Steam id's name:  {opponentName}\r\n");
            Gui.SetOpponentNameInGui(opponentName);
        }

        public static void DisplayOpponentProfilePictureFromWeb(string pictureLink)
        {
            string picturePath = System.IO.Path.GetTempPath() + "\\opponent.jpg";
            using WebClient web = new WebClient();
            web.DownloadFile(pictureLink, picturePath);
            Gui.SetProfilePictureInGui(picturePath);
        }
        public static void TurnOffSilentMode()
        {
            if (MainWindow.silentMode == false)
            { // if already turned off
                Gui.PrintToGuiConsole("Silent mode is already off. Restart Tekken 7 to turn it on.\r\n");
            }
            else
            {
                MainWindow.silentMode = false;
                Gui.PrintToGuiConsole("Silent mode turned off. Now feedback will also be given in-game.\r\n");
                CleanAllProcessMessages();
            }
        }
    }
}
