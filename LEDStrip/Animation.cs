namespace LEDStrip;

public class Animation
{
    public static int RainbowStatic = 4;
    public static int RainbowFade = 3;
    public static int RainbowLeftRightBounceMiddle = 0;
    public static int RainbowLeftRightBounce = 1;
    public static int RainbowLeftRight = 2;
    public static int Blink = 5;
    public static int Static = 6;
    public static int LeftTurnSignal = 7;
    public static int RightTurnSignal = 8;

    public static Dictionary<int, string> GetAnimations()
    {
        Dictionary<int, string> animationDictionary = new Dictionary<int, string>();

        // Get all public static fields of the Animation class
        var fields = typeof(Animation).GetFields(System.Reflection.BindingFlags.Public | System.Reflection.BindingFlags.Static);

        foreach (var field in fields)
        {
            if (field.FieldType == typeof(int))
            {
                string fieldName = field.Name;
                int fieldValue = (int)field.GetValue(null); // null argument for static fields

                animationDictionary.Add(fieldValue, fieldName);
            }
        }

        return animationDictionary;
    }
}