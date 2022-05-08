using Steamworks;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;

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
    }
}
