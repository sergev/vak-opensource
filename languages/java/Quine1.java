public class Quine1
{
  public static void main(String[] args)
  {
    char q = 34;      // Quotation mark character
    String[] l = {    // Array of source code
    "public class Quine1",
    "{",
    "  public static void main(String[] args)",
    "  {",
    "    char q = 34;      // Quotation mark character",
    "    String[] l = {    // Array of source code",
    "    ",
    "    };",
    "    for (int i = 0; i < 6; i++)           // Print opening code",
    "        System.out.println(l[i]);",
    "    for (int i = 0; i < l.length; i++)    // Print string array",
    "        System.out.println(l[6] + q + l[i] + q + ',');",
    "    for (int i = 7; i < l.length; i++)    // Print this code",
    "        System.out.println(l[i]);",
    "  }",
    "}",
    };
    for (int i = 0; i < 6; i++)           // Print opening code
        System.out.println(l[i]);
    for (int i = 0; i < l.length; i++)    // Print string array
        System.out.println(l[6] + q + l[i] + q + ',');
    for (int i = 7; i < l.length; i++)    // Print this code
        System.out.println(l[i]);
  }
}
