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
std::wstring urlToWString(LPCTSTR url) {
    IStream* stream;
    HRESULT errorCode;
    wchar_t buffer[100];
    unsigned long bytesRead;
    std::wstringstream ss;
    std::wstring result;
    errorCode = URLOpenBlockingStream(0, url, &stream, 0, 0);
    switch (errorCode) {
    case S_OK:
        break;
    case E_OUTOFMEMORY:
        return L"error: outofmemory";
    default: // other
        return L"error: other";
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

void urlToFile(std::string url, LPCTSTR filePath) {
    std::wstring wurl;
    HRESULT errorCode;
    wurl = charPtrToWString((char*)url.c_str());
    errorCode = URLDownloadToFile(0, (LPCTSTR)wurl.c_str(), filePath, 0, 0);
    switch (errorCode) {
    case S_OK:
        //std::cout << "ok" << std::endl;
        break;
    case E_OUTOFMEMORY:
        myGuiTerminalPrint(std::string("Error in urlToFile(): Out of memory.\r\n"));
        break;
    case INET_E_DOWNLOAD_FAILURE:
        myGuiTerminalPrint(std::string("Error in urlToFile(): Inet download failure.\r\n"));
        break;
    default: // other
        myGuiTerminalPrint(std::string("Error in urlToFile(): Failed to download file: ").append(url).append(std::string(" Error code: ")).append(std::to_string(errorCode)).append(std::string("\r\n")));
        break;
    }
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

std::string extractProfilePictureUrlFromSteamHtmlString(std::string htmlString) {
    const char* prefixOne = "playerAvatarAutoSizeInner";
    const char* prefixTwo = "<img src=\"";
    const char* postfix = "\"";
    int urlIndex, urlSize, prefixOneIndex, prefixTwoIndex, postfixIndex; // locations in the html string
    std::string result;
    prefixOneIndex = bruteForceFindIndex((char*)htmlString.c_str(), (char*)prefixOne);
    prefixTwoIndex = bruteForceFindIndexAfterIndex((char*)htmlString.c_str(), (char*)prefixTwo, prefixOneIndex);
    postfixIndex = bruteForceFindIndexAfterIndex((char*)htmlString.c_str(), (char*)postfix, prefixTwoIndex + (int) strlen(prefixTwo) + 1);
    urlIndex = prefixTwoIndex + (int) strlen(prefixTwo);
    urlSize = postfixIndex - urlIndex;
    result = htmlString.substr(urlIndex, urlSize);
    
    size_t posExtension = result.find_last_of('.');
    std::string fileExtension = result.substr(posExtension + 1);
    if (fileExtension.compare("png") == 0) {
        int secondPrefixTwoIndex, secondPostfixIndex, secondUrlIndex, secondUrlSize;
        secondPrefixTwoIndex = bruteForceFindIndexAfterIndex((char*)htmlString.c_str(), (char*)prefixTwo, postfixIndex);
        secondPostfixIndex = bruteForceFindIndexAfterIndex((char*)htmlString.c_str(), (char*)postfix, secondPrefixTwoIndex + (int)strlen(prefixTwo) + 1);
        secondUrlIndex = secondPrefixTwoIndex + (int)strlen(prefixTwo);
        secondUrlSize = secondPostfixIndex - secondUrlIndex;
        result = htmlString.substr(secondUrlIndex, secondUrlSize);
    }
    return result;
}

std::string getSteamPageHtml(QWORD steamId) {
    std::wstring url;
    std::string htmlString;
    url = std::wstring(L"https://steamcommunity.com/profiles/").append(std::to_wstring(steamId));
    htmlString = urlToString((LPCTSTR)url.c_str());
    return htmlString;
}

std::string getOnlineNameUsingSteamId(QWORD steamId) {
    std::string htmlString;
    std::string name;
    htmlString = getSteamPageHtml(steamId);
    name = extractNameFromSteamHtmlString(htmlString);
	return name;
}

std::string getOnlineProfilePictureUrlUsingSteamId(QWORD steamId) {
    std::string htmlString;
    std::string result;
    htmlString = getSteamPageHtml(steamId);
    result = extractProfilePictureUrlFromSteamHtmlString(htmlString);
	return result;
}















