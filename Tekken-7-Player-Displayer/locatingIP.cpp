#include "player-displayer.h"

// how to use:
//getIpLocation(std::wstring(L"103.149.162.195"))
//getIpLocation(std::wstring(L"175.45.176.0"))
//myGuiTerminalPrint(getIpLocation(std::wstring(L"184.154.82.162")).append("\r\n")); // amreeeka
std::string getIpLocation(std::wstring ipAddress) {
    std::wstring url;
	std::string result;
    std::string htmlString;
    url = std::wstring(L"http://ip-api.com/xml/").append(ipAddress);
    htmlString = urlToString((LPCTSTR)url.c_str());
    result = extractLocationFromHtmlString(htmlString);
	return result;
}

std::string extractLocationFromHtmlString(std::string htmlString) {
    std::string result, country, region, city;
    std::string countryTagName, regionTagName, cityTagName;
    countryTagName = "country";
    regionTagName = "regionName";
    cityTagName = "city";
    country = findXmlTagValue(htmlString, countryTagName);
    region = findXmlTagValue(htmlString, regionTagName);
    city = findXmlTagValue(htmlString, cityTagName);
    result = country.append(", ").append(region).append(", ").append(city);
    return result;
}

std::string findXmlTagValue(std::string xmlString, std::string tag) {
    int resultIndex, resultSize, prefixIndex, postfixIndex; // locations in the xml string
    std::string result;
    std::string prefix = std::string("<").append(tag).append(">");
    std::string postfix = std::string("</").append(tag).append(">");
    prefixIndex = bruteForceFindIndex((char*)xmlString.c_str(), (char*)prefix.c_str());
    postfixIndex = bruteForceFindIndex((char*)xmlString.c_str(), (char*)postfix.c_str());
    if (prefixIndex == -1 || postfixIndex == -1) {
        return "";
    }
    resultIndex = prefixIndex + (int) strlen(prefix.c_str());
    resultSize = postfixIndex - resultIndex;
    result = xmlString.substr(resultIndex, resultSize);
    return result;
}