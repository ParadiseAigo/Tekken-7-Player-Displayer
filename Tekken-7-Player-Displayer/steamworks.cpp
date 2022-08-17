#include "player-displayer.h"
#include "Steamworks/Headers/steam_api.h"

bool initSteamworks(char* steamAppId) {
	createSteamappidFile(steamAppId);
	bool success = SteamAPI_Init();
	return success;
}

void shutdownSteamWorks() {
	SteamAPI_Shutdown();
}

std::string getIpAddressForSteamId(QWORD steamId) {
	std::string ip = "";
	CSteamID id = CSteamID((uint64) steamId);
	P2PSessionState_t sessionState;
	if (SteamNetworking()->GetP2PSessionState(id, &sessionState))
	{
		ip = ipAddressToString(sessionState.m_nRemoteIP);
	}
	return ip;
}
