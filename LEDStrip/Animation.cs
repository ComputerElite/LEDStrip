namespace LEDStrip;

public class Animation
{
    public static string RainbowStatic = "rbs";
    public static string RainbowFade = "rbf";
    public static string RainbowLeftRightBounceInMiddle = "rlbm";
    public static string RainbowLeftRightBounce = "rlb";
    public static string Blink = "blk";
    public static string Static = "static";

    public static Dictionary<string, string> GetAnimations()
    {
        Dictionary<string, string> animationDictionary = new Dictionary<string, string>();

        // Get all public static fields of the Animation class
        var fields = typeof(Animation).GetFields(System.Reflection.BindingFlags.Public | System.Reflection.BindingFlags.Static);

        foreach (var field in fields)
        {
            if (field.FieldType == typeof(string))
            {
                string fieldName = field.Name;
                string fieldValue = (string)field.GetValue(null); // null argument for static fields

                animationDictionary.Add(fieldValue, fieldName);
            }
        }

        return animationDictionary;
    }
}