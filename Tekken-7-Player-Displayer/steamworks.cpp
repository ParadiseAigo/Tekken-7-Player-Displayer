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

std::string getIpAddressOfSteamId(QWORD steamId) {
	std::string ip = "";
	CSteamID id = CSteamID((uint64) steamId);
	P2PSessionState_t sessionState;
	if (SteamNetworking()->GetP2PSessionState(id, &sessionState)) {
		ip = ipAddressToString(sessionState.m_nRemoteIP);
	}
	return ip;
}

std::string ipAddressToString(u_long ip) {  
    // the ip address is a hex number with 8 digits: 2 digits for each number in the ip address
    u_long ip4thNumber = ip % 0x100;
    u_long ip3rdNumber = (ip / 0x100) % 0x100;
    u_long ip2ndNumber = (ip / 0x10000) % 0x100;
    u_long ip1stNumber = (ip / 0x1000000) % 0x100;
    return std::to_string(ip1stNumber).append(".")
        .append(std::to_string(ip2ndNumber)).append(".")
        .append(std::to_string(ip3rdNumber)).append(".")
        .append(std::to_string(ip4thNumber));
}
