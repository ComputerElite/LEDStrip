namespace LEDStrip;

class Program
{
    static void Main(string[] args)
    {
        LEDStrip strip = new LEDStrip("/dev/ttyUSB0");
        strip.StartSerialConnection();
        strip.SetAnimation(Animation.RainbowFade);
        //strip.StartManualControl();
        WebServer s = new WebServer();
        s.Start(strip);
    }
}