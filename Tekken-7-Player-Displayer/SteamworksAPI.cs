using Steamworks;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading;

namespace Tekken_7_Player_Displayer
{
    class SteamworksAPI
    {
        public static bool Init(string steamAppId)
        {
            File.WriteAllText("steam_appid.txt", steamAppId);
            bool success = SteamAPI.Init();
            return success;
        }

        public static void Shutdown()
        {
            SteamAPI.Shutdown();
        }

        public static string GetIPAddressForSteamId(long steamId)
        {
            string ip = "";
            CSteamID id = new CSteamID((ulong)steamId);
            P2PSessionState_t sessionState = new P2PSessionState_t();
            if (SteamNetworking.GetP2PSessionState(id, out sessionState))
            {
                byte[] ipBytes = BitConverter.GetBytes(sessionState.m_nRemoteIP).Reverse().ToArray();
                ip = new IPAddress(ipBytes).ToString();
            }
            return ip;
        }

        public static void SavePlayerLobbies(CallResult<LobbyMatchList_t> callResult, long userSteamId)
        {
            SteamAPI.RunCallbacks(); // needs to be called to dispatch call results to listeners
            //SteamMatchmaking.AddRequestLobbyListNumericalFilter("tks4s_searchable_int_atter", 1376289, Steamworks.ELobbyComparison.k_ELobbyComparisonEqual); // filter to get ranked lobbies
            //SteamMatchmaking.AddRequestLobbyListDistanceFilter(Steamworks.ELobbyDistanceFilter.k_ELobbyDistanceFilterWorldwide);
            //SteamMatchmaking.AddRequestLobbyListNumericalFilter("tksex_fighter.rank_id", playerRankId + maxRankDifference, Steamworks.ELobbyComparison.k_ELobbyComparisonLessThan);
            //SteamMatchmaking.AddRequestLobbyListNumericalFilter("tksex_fighter.rank_id", playerRankId - maxRankDifference, Steamworks.ELobbyComparison.k_ELobbyComparisonGreaterThan);
            //SteamMatchmaking.AddRequestLobbyListNumericalFilter("tksex_fighter.fighter_id", 22, Steamworks.ELobbyComparison.k_ELobbyComparisonEqual);
            SteamAPICall_t hSteamAPICall = SteamMatchmaking.RequestLobbyList();
            callResult.Set(hSteamAPICall);
        }

        public static void DisplayPlayerList(CallResult<LobbyMatchList_t> callResult, long userSteamId)
        {
            // the following lines get the lobby data of the player to get the rank of the player and the max rank difference selected
            int playerRankId = -1;
            int playerLobbyMaxRankId = -1;

            if (SteamFriends.GetFriendGamePlayed(new CSteamID((ulong)userSteamId), out FriendGameInfo_t playerGameInfo)
                && playerGameInfo.m_steamIDLobby.IsValid())
            {
                CSteamID lobbySteamId = playerGameInfo.m_steamIDLobby;

                // the following lines get the lobby game server - unused code (returns nothing)
                SteamMatchmaking.GetLobbyGameServer(lobbySteamId, out uint punGameServerIP, out ushort punGameServerPort, out CSteamID psteamIDGameServer);
                byte[] ipBytes = BitConverter.GetBytes(punGameServerIP).Reverse().ToArray();
                string serverIp = new IPAddress(ipBytes).ToString();
                Gui.PrintLineToGuiConsole($"Lobby game server: steamID = {psteamIDGameServer} , address = {serverIp}:{punGameServerPort}");

                // the following lines get the lobby members - unused code
                int numLobbyMembers = SteamMatchmaking.GetNumLobbyMembers(lobbySteamId);
                Gui.PrintLineToGuiConsole($"Lobby: steamId = {lobbySteamId} , members count: {numLobbyMembers}");
                for (int i = 0; i < numLobbyMembers; i++)
                {
                    CSteamID lobbyMemberSteamId = SteamMatchmaking.GetLobbyMemberByIndex(lobbySteamId, i);
                    string lobbyMemberName = SteamFriends.GetFriendPersonaName(lobbyMemberSteamId);
                    Gui.PrintLineToGuiConsole($"Lobby member {i}: {lobbyMemberName}");
                }

                // the following lines get the lobby data
                int lobbyDataCount = SteamMatchmaking.GetLobbyDataCount(lobbySteamId);
                for (int i = 0; i < lobbyDataCount; i++)
                {
                    SteamMatchmaking.GetLobbyDataByIndex(lobbySteamId, i, out string key, 256, out string value, 256);
                    Gui.PrintLineToGuiConsole($"Lobby data {i}: Key = {key} , Value = {value}");
                    if (key == "tks4s_rank_id") { playerRankId = int.Parse(value); }
                    if (key == "tksex_max_rank_fighter.rank_id") { playerLobbyMaxRankId = int.Parse(value); }
                }
            }
            int maxRankDifference = 5;// playerLobbyMaxRankId != -1 && playerRankId != -1 ? playerLobbyMaxRankId - playerRankId : 3;
            playerRankId = 13; //playerRankId == -1 ? 0 : playerRankId;
            //----------------------------------------------------------------------------------------------------------------------------

            // the following lines request a list of filtered lobbies, the results will be returned asynchronously to the method OnLobbyMatchList
            SteamAPI.RunCallbacks(); // needs to be called to dispatch call results to listeners
            SteamMatchmaking.AddRequestLobbyListNumericalFilter("tks4s_searchable_int_atter", 1376289, Steamworks.ELobbyComparison.k_ELobbyComparisonEqual); // filter to get ranked lobbies
            //SteamMatchmaking.AddRequestLobbyListDistanceFilter(Steamworks.ELobbyDistanceFilter.k_ELobbyDistanceFilterWorldwide);
            //SteamMatchmaking.AddRequestLobbyListNumericalFilter("tksex_fighter.rank_id", playerRankId + maxRankDifference, Steamworks.ELobbyComparison.k_ELobbyComparisonLessThan);
            //SteamMatchmaking.AddRequestLobbyListNumericalFilter("tksex_fighter.rank_id", playerRankId - maxRankDifference, Steamworks.ELobbyComparison.k_ELobbyComparisonGreaterThan);
            //SteamMatchmaking.AddRequestLobbyListNumericalFilter("tksex_fighter.fighter_id", 22, Steamworks.ELobbyComparison.k_ELobbyComparisonEqual);
            SteamAPICall_t hSteamAPICall = SteamMatchmaking.RequestLobbyList();
            callResult.Set(hSteamAPICall);
        }

        static public void MyCallbackLobbyInfo(LobbyMatchList_t pCallback, bool bIOFailure)
        {
            uint nrOfLobbies = pCallback.m_nLobbiesMatching;
            //Gui.PrintLineToGuiConsole($"LobbyMatchList: lobbies count = {nrOfLobbies}");
            for (int i = 0; i < nrOfLobbies; i++)
            {
                CSteamID lobbySteamId = SteamMatchmaking.GetLobbyByIndex(i);
                
                /*
                // the following lines print the lobby member names (the names returned here are empty) 
                int numLobbyMembers = SteamMatchmaking.GetNumLobbyMembers(lobbySteamId);
                Gui.PrintLineToGuiConsole($"Lobby {i}: steamId = {lobbySteamId.m_SteamID} , members count: {numLobbyMembers}");
                for (int j = 0; j < numLobbyMembers; j++)
                {
                    CSteamID lobbyMemberSteamId = SteamMatchmaking.GetLobbyMemberByIndex(lobbySteamId, j);
                    string lobbyMemberName = SteamFriends.GetFriendPersonaName(lobbyMemberSteamId);
                    Gui.PrintLineToGuiConsole($"Lobby member {j}: steamId = {lobbyMemberSteamId.m_SteamID} , name = {lobbyMemberName} , location = {IPLocation.GetLocation(SteamworksAPI.GetIPAddressForSteamId((long) lobbyMemberSteamId.m_SteamID))}");
                }*/

                // the following lines print the lobby data
                long steamId = 0;
                string name = "";
                string character = "";
                string rank = "";
                int lobbyDataCount = SteamMatchmaking.GetLobbyDataCount(lobbySteamId);
                for (int k = 0; k < lobbyDataCount; k++)
                {
                    SteamMatchmaking.GetLobbyDataByIndex(lobbySteamId, k, out string key, Steamworks.Constants.k_nMaxLobbyKeyLength, out string value, Steamworks.Constants.k_nMaxLobbyKeyLength);
                    //Gui.PrintLineToGuiConsole($"Lobby data {k}: Key = {key} , Value = {value}");
                    if (key == "tkstma_ID") { steamId = long.Parse(value); }
                    if (key == "tksex_owner_player_name") { name = value; }
                    if (key == "tksex_fighter.fighter_id") { character = Pointers.ALL_CHARACTERS[int.Parse(value)]; }
                    if (key == "tks4s_rank_id") { rank = Pointers.ALL_RANKS[int.Parse(value)]; }
                    PlayerLobbyInfo.AddToList(MainWindow.ListOfPlayerLobbies,
                        new PlayerLobbyInfo(name, steamId, character, rank));
                }
                //string ip = SteamworksAPI.GetIPAddressForSteamId(steamId); // this does not return the correct ip, need to debug to see if steamId is correct or if long.Parse is not working well
                //string location = IPLocation.GetLocation(ip);
            }
        }
    }

    public class PlayerLobbyInfo
    {
        public String Name;
        public long SteamId;
        public String Character;
        public String Rank;

        public PlayerLobbyInfo(String name, long steamId, String character, String rank)
        {
            Name = name;
            SteamId = steamId;
            Character = character;
            Rank = rank;
        }

        static public void AddToList(List<PlayerLobbyInfo> theList, PlayerLobbyInfo newPlayer)
        {
            if (newPlayer.SteamId == 0) return;
            foreach (PlayerLobbyInfo player in theList)
            {
                if (player.SteamId == newPlayer.SteamId)
                {
                    player.Name = newPlayer.Name;
                    player.Character = newPlayer.Character;
                    player.Rank = newPlayer.Rank;
                    return;
                }
            }
            theList.Add(newPlayer);
        }

        static public bool DoesListContainPlayer(List<PlayerLobbyInfo> theList, PlayerLobbyInfo newPlayer)
        {
            foreach (PlayerLobbyInfo player in theList)
            {
                if (player.SteamId == newPlayer.SteamId) return true;
            }
            return false;
        }

        static public void PrintList(List<PlayerLobbyInfo> theList)
        {
            String toBePrinted = "";
            String currentFoundPlayer = "";
            int charactersPadding = 1 + Pointers.ALL_CHARACTERS.OrderByDescending(s => s.Length).First().Length;
            int rankPadding = 1 + Pointers.ALL_RANKS.OrderByDescending(s => s.Length).First().Length;
            theList.Sort(delegate(PlayerLobbyInfo x, PlayerLobbyInfo y)
            {
                return x.Name.CompareTo(y.Name);
            });
            foreach (PlayerLobbyInfo player in theList)
            {
                String steamId = player.SteamId.ToString() + " ";
                String character = player.Character;
                String rank = player.Rank;
                String name = player.Name;
                String line = (steamId + character.PadRight(charactersPadding) + rank.PadRight(rankPadding) + name + "\r\n");
                toBePrinted += line;
                if (player.SteamId == MainWindow.lastFoundSteamId)
                {
                    currentFoundPlayer = line;
                }
            }
            Gui.PrintToGuiNextOpponent(currentFoundPlayer);
            Gui.PrintToGuiPlayerList(toBePrinted);
        }

        static public String GetPlayerRank(List<PlayerLobbyInfo> theList, long steamId)
        {
            foreach (PlayerLobbyInfo player in theList)
            {
                if (player.SteamId == steamId) return player.Rank;
            }
            return "";
        }

        static public String GetPlayerCharacter(List<PlayerLobbyInfo> theList, long steamId)
        {
            foreach (PlayerLobbyInfo player in theList)
            {
                if (player.SteamId == steamId) return player.Character;
            }
            return "";
        }

        static public void PrintPlayerRank(List<PlayerLobbyInfo> theList, long steamId)
        {
            String rank = GetPlayerRank(theList, steamId);
            if (rank == "") rank = "?";
            Gui.PrintLineToGuiConsole("Rank:      " + rank);
        }

        static public void PrintPlayerCharacter(List<PlayerLobbyInfo> theList, long steamId)
        {
            String character = GetPlayerCharacter(theList, steamId);
            if (character == "") character = "?";
            Gui.PrintLineToGuiConsole("Character: " + character);
        }
    }
}
