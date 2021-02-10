#include "player-displayer.h"

std::string urlToString(LPCTSTR url) {
    IStream* stream;
    HRESULT errorCode;
    char buffer[100];
    unsigned long bytesRead;
    std::stringstream ss;
    std::string result;
    errorCode = URLOpenBlockingStream(0, url, &stream, 0, 0);
    switch (errorCode) {
    case S_OK:
        break;
    case E_OUTOFMEMORY:
        return "";
    default: // other
        return "";
    }
    stream->Read(buffer, 100, &bytesRead);
    while (bytesRead > 0U) {
        ss.write(buffer, (long long)bytesRead);
        stream->Read(buffer, 100, &bytesRead);
    }
    stream->Release();
    result = ss.str();
    return result;
}

void urlToFile(char* url, char* filePath) {
	// player picture in steam search for the keyword:  playerAvatarAutoSizeInner
	//URLDownloadToFile()  <_< use this to dl the profile picture
}

std::string extractNameFromSteamHtmlString(std::string htmlString) {
    const char* prefix = "<title>Steam Community :: ";
    const char* postfix = "</title>";
    int nameIndex, nameSize, prefixIndex, postfixIndex; // locations in the html string
    std::string result;
    prefixIndex = bruteForceFindIndex((char*)htmlString.c_str(), (char*)prefix);
    postfixIndex = bruteForceFindIndex((char*)htmlString.c_str(), (char*)postfix);
    nameIndex = prefixIndex + (int) strlen(prefix);
    nameSize = postfixIndex - nameIndex;
    result = htmlString.substr(nameIndex, nameSize);
    return result;
}

std::string getOnlineNameUsingSteamId(QWORD steamId) {
    std::wstring url;
    std::string htmlString;
    std::string name;
    url = std::wstring(L"https://steamcommunity.com/profiles/").append(std::to_wstring(steamId));
    htmlString = urlToString((LPCTSTR)url.c_str());
    name = extractNameFromSteamHtmlString(htmlString);
	return name;
}















