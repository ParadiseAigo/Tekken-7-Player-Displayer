﻿using Steamworks;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading;

namespace Tekken_7_Player_Displayer
{
    static public class LobbyListKeys
    {
        //public const string Mode = "tks4s_searchable_int_atter";
        public const string Mode = "tksex_match_type";
        public const string Rank = "tksex_fighter.rank_id";
        public const string Character = "tksex_fighter.fighter_id";
        public const string SteamId = "tksex_owner_online_id";
        public const string Name = "tksex_owner_player_name";
    }

    static public class LobbyListFilters
    {
        //public const int Ranked = 1376289;
        /*
        // these numbers work with "tks4s_searchable_int_atter"
        public const int Ranked = 393249;
        public const int QuickMatch = 393250;
        public const int PlayerSessions = 393252;
        */
        // these numbers work with "tksex_match_type"
        public const int Ranked = 0;
        public const int QuickMatch = 1;
        public const int PlayerSessions = 2;
        public const int None = -1; // this one is not in the game, i made it up
        public const int Off = -2; // another one i made up
    }

    public class SteamworksAPI
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
            if (SteamNetworking.GetP2PSessionState(id, out sessionState)) // returns false if there is no open session with the specified user
            {
                byte[] ipBytes = BitConverter.GetBytes(sessionState.m_nRemoteIP).Reverse().ToArray();
                ip = new IPAddress(ipBytes).ToString();
            }
            return ip;
        }

        public static void SavePlayerLobbies(CallResult<LobbyMatchList_t> callResult)
        {
            SteamAPI.RunCallbacks(); // needs to be called to dispatch call results to listeners
            if (MainWindow.OnlineModeFilter != LobbyListFilters.None)
            {
                SteamMatchmaking.AddRequestLobbyListNumericalFilter(LobbyListKeys.Mode, MainWindow.OnlineModeFilter, Steamworks.ELobbyComparison.k_ELobbyComparisonEqual);
            }
            //debugging
            //Gui.PrintLineToGuiConsole("mode: " + LobbyListKeys.Mode);
            //Gui.PrintLineToGuiConsole("filter: " + MainWindow.OnlineModeFilter.ToString());
            SteamMatchmaking.AddRequestLobbyListDistanceFilter(Steamworks.ELobbyDistanceFilter.k_ELobbyDistanceFilterWorldwide);
            // unused filter by rank
            //SteamMatchmaking.AddRequestLobbyListNumericalFilter("tksex_fighter.rank_id", playerRankId + maxRankDifference, Steamworks.ELobbyComparison.k_ELobbyComparisonLessThan);
            //SteamMatchmaking.AddRequestLobbyListNumericalFilter("tksex_fighter.rank_id", playerRankId - maxRankDifference, Steamworks.ELobbyComparison.k_ELobbyComparisonGreaterThan);
            // unused filter by character
            //SteamMatchmaking.AddRequestLobbyListNumericalFilter("tksex_fighter.fighter_id", 22, Steamworks.ELobbyComparison.k_ELobbyComparisonEqual);
            SteamAPICall_t hSteamAPICall = SteamMatchmaking.RequestLobbyList();
            callResult.Set(hSteamAPICall);
        }

        static public void MyCallbackLobbyMatchList(LobbyMatchList_t lobbyMatchList, bool bIOFailure)
        {
            uint nrOfLobbies = lobbyMatchList.m_nLobbiesMatching;
            for (int i = 0; i < nrOfLobbies; i++)
            {
                CSteamID lobbySteamId = SteamMatchmaking.GetLobbyByIndex(i);
                long steamId = 0;
                string name = "";
                string character = "";
                string rank = "";
                int onlineModeOfPlayer = -1;
                int lobbyDataCount = SteamMatchmaking.GetLobbyDataCount(lobbySteamId);
                for (int k = 0; k < lobbyDataCount; k++)
                {
                    SteamMatchmaking.GetLobbyDataByIndex(lobbySteamId, k, out string key, Steamworks.Constants.k_nMaxLobbyKeyLength, out string value, Steamworks.Constants.k_nMaxLobbyKeyLength);
                    // debugging
                    //Gui.PrintLineToGuiConsole($"Lobby data {k}: Key = {key} , Value = {value}");
                    /*
                    if (key == LobbyListKeys.Name) Gui.PrintLineToGuiConsole($"Lobby data {k}: Key = {key} , Value = {value}");
                    if (key == LobbyListKeys.Mode) Gui.PrintLineToGuiConsole($"Lobby data {k}: Key = {key} , Value = {value}");
                    */
                    if (key == LobbyListKeys.Mode) { onlineModeOfPlayer = int.Parse(value); }
                    if (key == LobbyListKeys.SteamId) { steamId = long.Parse(value, System.Globalization.NumberStyles.HexNumber); }
                    if (key == LobbyListKeys.Name) { name = value; }
                    if (key == LobbyListKeys.Character) { character = Pointers.ALL_CHARACTERS[int.Parse(value)]; }
                    if (key == LobbyListKeys.Rank) {
                        int index = int.Parse(value);
                        if (index < Pointers.ALL_RANKS.Length)
                        {
                            rank = Pointers.ALL_RANKS[index];
                        }
                        else rank = index.ToString();
                    }
                }
                //Thread.Sleep(2000); // helps with debugging
                if (MainWindow.OnlineModeFilter == onlineModeOfPlayer ||
                    MainWindow.OnlineModeFilter == LobbyListFilters.None)
                {
                    PlayerLobbyInfo.AddToList(MainWindow.ListOfPlayerLobbies, new PlayerLobbyInfo(lobbySteamId, name, steamId, character, rank));
                }
            }
        }
    }

    public class PlayerLobbyInfo
    {
        public CSteamID LobbyId;
        public String Name;
        public long SteamId;
        public String Character;
        public String Rank;

        public PlayerLobbyInfo(CSteamID lobbyId, String name, long steamId, String character, String rank)
        {
            LobbyId = lobbyId;
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
                    player.LobbyId = newPlayer.LobbyId;
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
    }
}
