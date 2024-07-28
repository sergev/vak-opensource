//
// Print conversion table from Flexowriter to internal Algol encoding.
//
#include <stdio.h>

unsigned flex_table[128];

#define FLEX_TAB    62
#define FLEX_SPACE  16
#define FLEX_CRLF   26

#if 0
code -  uc  lc
   0 -          shift disable
   1 -  78   1  ∨ 1
   2 -  66   2  × 2
   4 -  72   4  = 4
   7 - 101   7  ] 7
   8 -  98   8  ( 8
  14 - 162 163  | _
  16 - 123 123  space
  19 -  67   3  / 3
  21 -  91   5  ; 5
  22 - 100   6  [ 6
  25 -  99   9  ) 9
  26 - 119 119  crlf
  32 -  77   0  ∧ 0
  35 -  56  29  T t
  37 -  58  31  V v
  38 -  59  32  W w
  41 -  62  35  Z z
  49 -  70  74  > <
  50 -  55  28  S s
  52 -  57  30  U u
  55 -  60  33  X x
  56 -  61  34  Y y
  59 - 120  89  ' ⏨
  62 - 118 118  tab
  64 -  76  65  ¬ -
  67 -  48  21  L l
  69 -  50  23  N n
  70 -  51  24  O o
  73 -  54  27  R r
  81 -  46  19  J j
  82 -  47  20  K k
  84 -  49  22  M m
  87 -  52  25  P p
  88 -  53  26  Q q
  91 - 122  87  ? ,
  97 -  37  10  A a
  98 -  38  11  B b
 100 -  40  13  D d
 103 -  43  16  G g
 104 -  44  17  H h
 107 - 124  88  : .
 112 - 121  64  " +
 115 -  39  12  C c
 117 -  41  14  E e
 118 -  42  15  F f
 121 -  45  18  I i
 122 -          shift to lowercase
 124 -          shift to uppercase
 127 -          erase
#endif

void print(int flex_code, int value)
{
    printf("%4d - ", flex_code);

    if (value < 128) {
        printf("\\%02o", value);
    } else {
        int uppercase = value / 256;
        int lowercase = value % 256;
        printf("%3d %3d", uppercase, lowercase);
    }

    switch (flex_code) {
    case FLEX_TAB:
        printf("  tab");
        break;
    case FLEX_SPACE:
        printf("  space");
        break;
    case FLEX_CRLF:
        printf("  crlf");
        break;
    }
    printf("\n");
}

int main()
{
    // initialization of flex_table
    flex_table[  0] =    -2; flex_table[  1] = 19969; flex_table[  2] = 16898;
    flex_table[  3] =    -0; flex_table[  4] = 18436; flex_table[  5] =    -0;
    flex_table[  6] =    -0; flex_table[  7] = 25863; flex_table[  8] = 25096;
    flex_table[  9] =    -0; flex_table[ 10] =    -0; flex_table[ 11] =    -1;
    flex_table[ 12] =    -0; flex_table[ 13] =    -1; flex_table[ 14] = 41635;
    flex_table[ 15] =    -0; flex_table[ 16] = 31611; flex_table[ 17] =    -0;
    flex_table[ 18] =    -0; flex_table[ 19] = 17155; flex_table[ 20] =    -0;
    flex_table[ 21] = 23301; flex_table[ 22] = 25606; flex_table[ 23] =    -0;
    flex_table[ 24] =    -0; flex_table[ 25] = 25353; flex_table[ 26] = 30583;
    flex_table[ 27] =    -0; flex_table[ 28] =    -1; flex_table[ 29] =    -0;
    flex_table[ 30] =    -0; flex_table[ 31] =    -1; flex_table[ 32] = 19712;
    flex_table[ 33] =    -0; flex_table[ 34] =    -0; flex_table[ 35] = 14365;
    flex_table[ 36] =    -0; flex_table[ 37] = 14879; flex_table[ 38] = 15136;
    flex_table[ 39] =    -0; flex_table[ 40] =    -0; flex_table[ 41] = 15907;
    flex_table[ 42] =    -1; flex_table[ 43] =    -0; flex_table[ 44] =    -1;
    flex_table[ 45] =    -0; flex_table[ 46] =    -0; flex_table[ 47] =    -1;
    flex_table[ 48] =    -0; flex_table[ 49] = 17994; flex_table[ 50] = 14108;
    flex_table[ 51] =    -0; flex_table[ 52] = 14622; flex_table[ 53] =    -0;
    flex_table[ 54] =    -0; flex_table[ 55] = 15393; flex_table[ 56] = 15650;
    flex_table[ 57] =    -0; flex_table[ 58] =    -0; flex_table[ 59] = 30809;
    flex_table[ 60] =    -0; flex_table[ 61] =    -1; flex_table[ 62] = 30326;
    flex_table[ 63] =    -0; flex_table[ 64] = 19521; flex_table[ 65] =    -0;
    flex_table[ 66] =    -0; flex_table[ 67] = 12309; flex_table[ 68] =    -0;
    flex_table[ 69] = 12823; flex_table[ 70] = 13080; flex_table[ 71] =    -0;
    flex_table[ 72] =    -0; flex_table[ 73] = 13851; flex_table[ 74] =    -1;
    flex_table[ 75] =    -0; flex_table[ 76] =    -1; flex_table[ 77] =    -0;
    flex_table[ 78] =    -0; flex_table[ 79] =    -1; flex_table[ 80] =    -0;
    flex_table[ 81] = 11795; flex_table[ 82] = 12052; flex_table[ 83] =    -0;
    flex_table[ 84] = 12566; flex_table[ 85] =    -0; flex_table[ 86] =    -0;
    flex_table[ 87] = 13337; flex_table[ 88] = 13594; flex_table[ 89] =    -0;
    flex_table[ 90] =    -0; flex_table[ 91] = 31319; flex_table[ 92] =    -0;
    flex_table[ 93] =    -1; flex_table[ 94] =    -1; flex_table[ 95] =    -0;
    flex_table[ 96] =    -0; flex_table[ 97] =  9482; flex_table[ 98] =  9739;
    flex_table[ 99] =    -0; flex_table[100] = 10253; flex_table[101] =    -0;
    flex_table[102] =    -0; flex_table[103] = 11024; flex_table[104] = 11281;
    flex_table[105] =    -0; flex_table[106] =    -0; flex_table[107] = 31832;
    flex_table[108] =    -0; flex_table[109] =    -1; flex_table[110] =    -1;
    flex_table[111] =    -0; flex_table[112] = 31040; flex_table[113] =    -0;
    flex_table[114] =    -0; flex_table[115] =  9996; flex_table[116] =    -0;
    flex_table[117] = 10510; flex_table[118] = 10767; flex_table[119] =    -0;
    flex_table[120] =    -0; flex_table[121] = 11538; flex_table[122] =    -2;
    flex_table[123] =    -0; flex_table[124] =    -2; flex_table[125] =    -0;
    flex_table[126] =    -0; flex_table[127] =    -2;

    printf("code -  uc  lc\n");
    for (int i = 0; i < 128; i++) {
        int ch = flex_table[i];
        if (ch > 0) {
            print(i, ch);
        }
    }
}
