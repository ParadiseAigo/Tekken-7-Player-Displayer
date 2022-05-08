using System;
using System.Collections.Generic;
using System.Net;
using System.Text;

namespace Tekken_7_Player_Displayer
{
    class SteamURL
    {
        public static string GetSteamPageHtml(long steamId)
        {
            string url = $"https://steamcommunity.com/profiles/{steamId}";
            string htmlString = UrlToString(url);
            return htmlString;
        }

        public static string UrlToString(string url)
        {
            using WebClient web = new WebClient();
            return web.DownloadString(url);
        }

        public static string ExtractNameFromSteamHtmlString(string htmlString)
        {
            string prefix = "<title>Steam Community :: ";
            string postfix = "</title>";
            int nameIndex, nameSize, prefixIndex, postfixIndex; // locations in the html string
            prefixIndex = htmlString.IndexOf(prefix);
            postfixIndex = htmlString.IndexOf(postfix, prefixIndex);
            nameIndex = prefixIndex + prefix.Length;
            nameSize = postfixIndex - nameIndex;
            string result = htmlString.Substring(nameIndex, nameSize);
            return result;
        }

        public static string ExtractProfilePictureUrlFromSteamHtmlString(string htmlString)
        {
            string prefixOne = "playerAvatarAutoSizeInner";
            string prefixTwo = "<img src=\"";
            string postfix = "\"";
            int urlIndex, urlSize, prefixOneIndex, prefixTwoIndex, postfixIndex; // locations in the html string
            prefixOneIndex = htmlString.IndexOf(prefixOne);
            prefixTwoIndex = htmlString.IndexOf(prefixTwo, prefixOneIndex);
            postfixIndex = htmlString.IndexOf(postfix, prefixTwoIndex + prefixTwo.Length + 1);
            urlIndex = prefixTwoIndex + prefixTwo.Length;
            urlSize = postfixIndex - urlIndex;
            string result = htmlString.Substring(urlIndex, urlSize);

            int posExtension = result.LastIndexOf('.');
            string fileExtension = result.Substring(posExtension + 1);
            if (fileExtension == "png")
            {
                int secondPrefixTwoIndex, secondPostfixIndex, secondUrlIndex, secondUrlSize;
                secondPrefixTwoIndex = htmlString.IndexOf(prefixTwo, postfixIndex);
                secondPostfixIndex = htmlString.IndexOf(postfix, secondPrefixTwoIndex + prefixTwo.Length + 1);
                secondUrlIndex = secondPrefixTwoIndex + prefixTwo.Length;
                secondUrlSize = secondPostfixIndex - secondUrlIndex;
                result = htmlString.Substring(secondUrlIndex, secondUrlSize);
            }
            return result;
        }
    }
}
