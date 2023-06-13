using System.IO.Ports;

namespace LEDStrip;

public class LEDStrip
{
    public string portName = "";
    public int numLEDs { get; set; } = 0;
    public int currentStep  { get; set; }= 0;
    public int brightness  { get; set; }= 0;
    public int currentAnimation { get; set; } = 0;
    public int color0 { get; set; } = 0;
    public int msDelay { get; set; } = 0;
    public int shortDelay { get; set; } = 0;
    public bool logSerial = true;

    public SerialPort port;
    public Thread connectionThread;
    
    public delegate void MessageRecievedDelegate(string message);
    public event MessageRecievedDelegate MessageRecieved;

    public List<string> msgsToSend = new List<string>();

    public LEDStrip(string portName, int baudRate = 9600)
    {
        this.portName = portName;
        port = new SerialPort(portName, baudRate);
    }

    /// <summary>
    /// Makes the LED strip accept commands from the standard input of the program
    /// </summary>
    public void StartManualControl()
    {
        while (true)
        {
            string msg = Console.ReadLine();
            if (msg == "exit")
            {
                ClosePort();
                Environment.Exit(0);
            }
            else
            {
                AddManualMessage(msg);
            }
        }
    }
    
    /// <summary>
    /// Stops and closes the serial connection
    /// </summary>
    public void ClosePort()
    {
        port.Close();
    }

    /// <summary>
    /// Sets one of the colors the strip animations use
    /// </summary>
    /// <param name="colorIndex">index of the color (e. g. 0 for static)</param>
    /// <param name="color">color to assign</param>
    public void SetColorVariable(int colorIndex, int color)
    {
        if (colorIndex == 0) color0 = color;
        msgsToSend.Add("sc," + colorIndex + "," + color.ToString("X6"));
    }
    
    /// <summary>
    /// Handles incoming messages from the LED strip
    /// </summary>
    /// <param name="msg"></param>
    public void HandleMessage(string msg)
    {
        string[] args = msg.Split(",");
        switch (args[0])
        {
            case "numleds":
                numLEDs = int.Parse(args[1]);
                break;
            case "step":
                currentStep = int.Parse(args[1]);
                break;
            case "color0":
                color0 = int.Parse(args[1]);
                break;
            case "msdelay":
                msDelay = int.Parse(args[1]);
                break;
            case "shortdelay":
                shortDelay = int.Parse(args[1]);
                break;
            case "animation":
                currentAnimation = int.Parse(args[1]);
                break;
            case "brightness":
                brightness = int.Parse(args[1]);
                break;
        }
        if(MessageRecieved != null) MessageRecieved(msg);
    }

    /// <summary>
    /// Sets an leds color. It won't be shown until ShowLEDs() is called.
    /// </summary>
    /// <param name="ledIndex">index of the led on the strip</param>
    /// <param name="ledColor">color of the led</param>
    public void SetLEDColor(int ledIndex, int ledColor)
    {
        msgsToSend.Add("l," + (char)ledIndex + "," + ColorConverter.ColorToString(ledColor.ToString("X6")));
    }

    /// <summary>
    /// Shows all led colors set via SetLEDColor()
    /// </summary>
    public void ShowLEDs()
    {
        msgsToSend.Add("s");
    }

    /// <summary>
    /// Sets one of the predefined animations of the LED strip
    /// </summary>
    /// <param name="a">Animation name</param>
    public void SetAnimation(int a)
    {
        currentAnimation = a;
        msgsToSend.Add("sa," + a);
    }
    
    /// <summary>
    /// Sets color steps for rainbow animations
    /// </summary>
    /// <param name="steps">Steps per update</param>
    public void SetStep(int steps)
    {
        currentStep = steps;
        msgsToSend.Add("ss," + steps);
    }
    
    /// <summary>
    /// Sets the brightness of the LED strip
    /// </summary>
    /// <param name="brightness">brightness 0 - 255</param>
    public void SetBrightness(int brightness)
    {
        this.brightness = brightness;
        msgsToSend.Add("sb," + brightness);
    }
    
    /// Sets the delay for animations such as blinking
    /// </summary>
    /// <param name="delayInMs">delay in ms</param>
    public void SetDelay(int delayInMs)
    {
        msDelay = delayInMs;
        msgsToSend.Add("sd," + delayInMs);
    }
    
    /// Sets the delay for animations such as blinking
    /// </summary>
    /// <param name="delayInMs">delay in ms</param>
    public void SetShortDelay(int shortDelayInMs)
    {
        shortDelay = shortDelayInMs;
        msgsToSend.Add("ssd," + shortDelayInMs);
    }

    /// <summary>
    /// Sets all LEDs on the strip to one specific color
    /// </summary>
    /// <param name="ledColor">color of the LEDs</param>
    public void SetAllLEDs(int ledColor)
    {
        msgsToSend.Add("al," + ColorConverter.ColorToString(ledColor.ToString("X6")));
    }
    
    /// <summary>
    /// Asks the LED strip for information about itself
    /// </summary>
    public void UpdateStripInfo()
    {
        msgsToSend.Add("info");
    }

    /// <summary>
    /// Adds a message to be sent to the strip
    /// </summary>
    /// <param name="msg">message to send</param>
    public void AddManualMessage(string msg)
    {
        msgsToSend.Add(msg);
    }
    
    
    private string currentMsg = "";
    DateTime lastInfoRequested = DateTime.Now;
    
    /// <summary>
    /// Starts a thread that handles the serial connection
    /// </summary>
    public void StartSerialConnection()
    {
        port.Open();
        connectionThread = new Thread(() =>
        {
            while (port.IsOpen)
            {
                // Update strip info every 2 seconds
                if(DateTime.Now - lastInfoRequested > TimeSpan.FromSeconds(2))
                {
                    UpdateStripInfo();
                    lastInfoRequested = DateTime.Now;
                }
                while (msgsToSend.Count > 0)
                {
                    if (logSerial) Console.WriteLine("<< " + msgsToSend[0]);
                    port.WriteLine(msgsToSend[0]);
                    ReadAllFromSerial();
                    msgsToSend.RemoveAt(0);
                }

                ReadAllFromSerial();
            }
        });
        connectionThread.Start();
        // Wait 2 seconds for initialization
        Thread.Sleep(2000);
    }

    public void ReadAllFromSerial()
    {
        while (port.BytesToRead > 0)
        {
            char c = (char)port.ReadChar();
            if (c == '\n')
            {
                if (logSerial) Console.WriteLine(">> " + currentMsg);
                HandleMessage(currentMsg);
                currentMsg = "";
            }
            else
            {
                currentMsg += c;
            }
        }
    }
}