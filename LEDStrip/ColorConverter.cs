namespace LEDStrip;

public class ColorConverter
{
    public static string ColorToString(string hexString)
    {
        string c = "";

        for (int i = 0; i < hexString.Length; i += 2)
        {
            string hexByte = hexString.Substring(i, 2);
            int decimalValue = Convert.ToInt32(hexByte, 16);
            c += (char)decimalValue;
        }

        return c;
    }
}