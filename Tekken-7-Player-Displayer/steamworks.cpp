#include "player-displayer.h"
#include "Steamworks/Headers/steam_api.h"

bool initSteamworks() {
	createSteamappidFile();									// now writing this function
	//bool success = SteamAPI.Init();                     // this function and class
	//return success;
}

/*
public static void Shutdown() {
	SteamAPI.Shutdown();
}*/

/*
public static string GetIPAddressForSteamId(long steamId) {             // steam id of an account
	string ip = "";
	CSteamID id = new CSteamID((ulong)steamId);                         // this
	P2PSessionState_t sessionState = new P2PSessionState_t();           // this
	if (SteamNetworking.GetP2PSessionState(id, out sessionState))       // this
	{
		byte[] ipBytes = BitConverter.GetBytes(sessionState.m_nRemoteIP).Reverse().ToArray();   // this
		ip = new IPAddress(ipBytes).ToString();
	}
	return ip;
}
*/
