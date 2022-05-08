using System;
using System.Net;
using Newtonsoft.Json.Linq;
using System.IO;

namespace Tekken_7_Player_Displayer
{
    static class IPLocation
    {
        public static string GetLocation(string ip)
        {
            string location = "";
            string query = "http://ip-api.com/json/" + ip;

            HttpWebResponse response;
            try
            {
                response = GetWebResponse(query);
            }
            catch (WebException error)
            {
                Gui.PrintLineToGuiConsole($"Failed to get location: {error.Message}");
                return location;
            }

            if (response.StatusCode == HttpStatusCode.OK)
            {
                using StreamReader reader = new StreamReader(response.GetResponseStream());
                JObject data = JObject.Parse(reader.ReadToEnd());
                if (data["status"].ToString() == "success")
                {
                    location = data["country"].ToString()
                             + (data["regionName"].ToString() == "" ? ""
                                : ", " + data["regionName"].ToString())
                             + (data["city"].ToString() == "" ? ""
                                : " (" + data["city"].ToString() + ")");
                }
                else
                {
                    Gui.PrintLineToGuiConsole($"Failed to get location: {data["message"]}");
                }
            } 
            else
            {
                Gui.PrintLineToGuiConsole($"Failed to get location: Http error {response.StatusCode}: {response.StatusDescription}");
            }
            return location;
        }

        private static HttpWebResponse GetWebResponse(string query)
        {
            WebRequest request = WebRequest.Create(query);
            return (HttpWebResponse)request.GetResponse();
        }
    }
}
