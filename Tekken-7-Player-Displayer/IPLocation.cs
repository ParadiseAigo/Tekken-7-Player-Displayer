using System;
using System.Net;
using System.IO;
using System.Text.Json;

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
                using JsonDocument jsonDocument = JsonDocument.Parse(reader.ReadToEnd());
                JsonElement data = jsonDocument.RootElement;
                if (data.GetProperty("status").ToString() == "success")
                {
                    location = data.GetProperty("country").ToString()
                             + (data.GetProperty("regionName").ToString() == "" ? ""
                                : ", " + data.GetProperty("regionName").ToString())
                             + (data.GetProperty("city").ToString() == "" ? ""
                                : " (" + data.GetProperty("city").ToString() + ")");
                }
                else
                {
                    Gui.PrintLineToGuiConsole($"Failed to get location: {data.GetProperty("message")}");
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
