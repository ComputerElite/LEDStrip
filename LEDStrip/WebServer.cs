using System.Text.Json;
using ComputerUtils.Webserver;

namespace LEDStrip;

public class WebServer
{
    public HttpServer server;
    public LEDStrip strip;
    public void Start(LEDStrip strip)
    {
        this.strip = strip;
        string html = "../../../html/";
        server = new HttpServer();
        server.DefaultCacheValidityInSeconds = 0;
        server.AddRouteFile("/", html + "index.html", false, true, false);
        server.AddRouteFile("/style.css", html + "style.css", false, true, false);
        server.AddRouteFile("/color-picker.css", html + "color-picker.css", false, true, false);
        server.AddRouteFile("/color-picker.js", html + "color-picker.js", false, true, false);
        server.AddRoute("GET", "/api/animations", request =>
        {
            request.SendString(JsonSerializer.Serialize(Animation.GetAnimations()), "application/json");
            return true;
        });
        server.AddRoute("POST", "/api/setanimation", request =>
        {
            strip.SetAnimation(request.bodyString);
            request.SendString("Set animation to " + request.bodyString);
            return true;
        });
        server.AddRoute("GET", "/api/strip", request =>
        {
            request.SendString(JsonSerializer.Serialize(strip), "application/json");
            return true;
        });
        server.AddRoute("POST", "/api/setdelay", request =>
        {
            strip.SetDelay(int.Parse(request.bodyString));
            request.SendString("Set delay to " + request.bodyString);
            return true;
        });
        server.AddRoute("POST", "/api/setshortdelay", request =>
        {
            strip.SetShortDelay(int.Parse(request.bodyString));
            request.SendString("Set short delay to " + request.bodyString);
            return true;
        });
        server.AddRoute("POST", "/api/setstep", request =>
        {
            strip.SetStep(int.Parse(request.bodyString));
            request.SendString("Set step to " + request.bodyString);
            return true;
        });
        server.AddRoute("POST", "/api/setbrightness", request =>
        {
            strip.SetBrightness(int.Parse(request.bodyString));
            request.SendString("Set brightness to " + request.bodyString);
            return true;
        });
        server.AddRoute("POST", "/api/setcolor0", request =>
        {
            strip.SetColorVariable(0, Convert.ToInt32(request.bodyString.Replace("#", ""), 16));
            request.SendString("Set color0 to " + request.bodyString);
            return true;
        });
        server.AddRoute("POST", "/api/setled", request =>
        {
            string[] args = request.bodyString.Split(',');
            strip.SetLEDColor(int.Parse(args[0]), Convert.ToInt32(args[1], 16));
            request.SendString("Set color of led " + args[0] + " to " + args[1]);
            return true;
        });
        server.StartServer(20007);
    }
}