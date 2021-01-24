#include "player-displayer.h"
#include "gui.h"


void setPlayerNameInGui(char* playerName) {
	sendMessage(guiWindows.playerNameValueTextHandle, WM_SETTEXT, 0, (LPARAM)stringToWString(playerName));
}

void updateAllGuiMessages(char* newOpponentName, char* characterName, char* playerlistComment) {
	sendMessage(guiWindows.opponentNameValueTextHandle, WM_SETTEXT, 0, (LPARAM)stringToWString(newOpponentName));
	if (characterName == NULL) {
		sendMessage(guiWindows.opponentCharacterValueTextHandle, WM_SETTEXT, 0, (LPARAM)TEXT(""));
	} else {
		sendMessage(guiWindows.opponentCharacterValueTextHandle, WM_SETTEXT, 0, (LPARAM)stringToWString(characterName));
	}
	sendMessage(guiWindows.commentValueTextHandle, WM_SETTEXT, 0, (LPARAM)stringToWString(playerlistComment));
}

void cleanAllGuiMessages() {
	sendMessage(guiWindows.opponentNameValueTextHandle, WM_SETTEXT, 0, (LPARAM)TEXT(""));
	sendMessage(guiWindows.opponentCharacterValueTextHandle, WM_SETTEXT, 0, (LPARAM)TEXT(""));
	sendMessage(guiWindows.commentValueTextHandle, WM_SETTEXT, 0, (LPARAM)TEXT(""));
}