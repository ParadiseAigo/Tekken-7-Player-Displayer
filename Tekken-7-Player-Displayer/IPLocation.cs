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
            string country = "";
            string region = "";
            string city = "";
            string url = "http://ip-api.com/json/" + ip;

            HttpWebResponse response;
            try
            {
                response = GetWebResponse(url);
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
                    country = data.GetProperty("country").ToString();
                    region = data.GetProperty("regionName").ToString();
                    city = data.GetProperty("city").ToString();
                    location = country
                             + (region == "" ? "" : ", " + region)
                             + (city == "" | !MainWindow.fullLocation ? "" : ", " + city);
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
