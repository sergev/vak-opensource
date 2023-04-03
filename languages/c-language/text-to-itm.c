//
// GOST-10859 encoding: http://en.wikipedia.org/wiki/GOST_10859
//
// Encoding of ИТМ autocode: https://github.com/besm6/besm6.github.io/raw/master/doc/%D0%90%D0%B2%D1%82%D0%BE%D0%BA%D0%BE%D0%B4-%D0%A7%D0%B0%D0%B9%D0%BA%D0%BE%D0%B2%D1%81%D0%BA%D0%BE%D0%B3%D0%BE.pdf
//
#include <stdio.h>
#include <locale.h>
#include <stdint.h>
#include <wchar.h>
#include <string.h>

//
// Convert GOST-10859 encoding to ИТМ encoding.
//
const unsigned char gost_to_itm[256] = {
    /* 000-007 */ 0000, 0001, 0002, 0003, 0004, 0005, 0006, 0007,
    /* 010-017 */ 0010, 0011, 0061, 0070, 0067, 0046, 0047, 0017,
    /* 020-027 */ 0220, 0143, 0076, 0051, 0227, 0057, 0045, 0053,
    /* 030-037 */ 0066, 0170, 0064, 0041, 0152, 0074, 0054, 0056,
    /* 040-047 */ 0230, 0323, 0223, 0313, 0322, 0220, 0317, 0321,
    /* 050-057 */ 0314, 0332, 0236, 0311, 0207, 0205, 0203, 0315,
    /* 060-067 */ 0215, 0216, 0201, 0225, 0326, 0227, 0316, 0331,
    /* 070-077 */ 0324, 0301, 0325, 0327, 0320, 0334, 0335, 0222,
    /* 100-107 */ 0226, 0213, 0214, 0232, 0211, 0206, 0235, 0212,
    /* 110-117 */ 0224, 0234, 0217, 0231, 0221, 0050, 0000, 0000,
    /* 120-127 */ 0217, 0071, 0055, 0145, 0000, 0057, 0062, 0042,
    /* 130-137 */ 0044, 0070, 0043, 0063, 0134, 0136, 0064, 0041,
};

unsigned char unicode_to_gost(unsigned short val)
{
    static const unsigned char tab0[256] = {
        /* 00 - 07 */ 017,   017,  017,  017,  017,  017,  017,  017,
        /* 08 - 0f */ 017,   017,  0214, 017,  017,  0174, 017,  017,
        /* 10 - 17 */ 017,   017,  017,  017,  017,  017,  017,  017,
        /* 18 - 1f */ 017,   017,  017,  017,  017,  017,  017,  017,
        /*  !"#$%&' */ 0017, 0133, 0134, 0034, 0127, 0126, 0121, 0033,
        /* ()*+,-./ */ 0022, 0023, 0031, 0012, 0015, 0013, 0016, 0014,
        /* 01234567 */ 0000, 0001, 0002, 0003, 0004, 0005, 0006, 0007,
        /* 89:;<=>? */ 0010, 0011, 0037, 0026, 0035, 0025, 0036, 0136,
        /* @ABCDEFG */ 0021, 0040, 0042, 0061, 0077, 0045, 0100, 0101,
        /* HIJKLMNO */ 0055, 0102, 0103, 0052, 0104, 0054, 0105, 0056,
        /* PQRSTUVW */ 0060, 0106, 0107, 0110, 0062, 0111, 0112, 0113,
        /* XYZ[\]^_ */ 0065, 0063, 0114, 0027, 0175, 0030, 0115, 0132,
        /* `abcdefg */ 0032, 0040, 0042, 0061, 0077, 0045, 0100, 0101,
        /* hijklmno */ 0055, 0102, 0103, 0052, 0104, 0054, 0105, 0056,
        /* pqrstuvw */ 0060, 0106, 0107, 0110, 0062, 0111, 0112, 0113,
        /* xyz{|}~  */ 0065, 0063, 0114, 0125, 0130, 017,  0123, 017,
        /* 80 - 87 */ 017,   017,  017,  017,  017,  017,  017,  017,
        /* 88 - 8f */ 017,   017,  017,  017,  017,  017,  017,  017,
        /* 90 - 97 */ 017,   017,  017,  017,  017,  017,  017,  017,
        /* 98 - 9f */ 017,   017,  017,  017,  017,  017,  017,  017,
        /* a0 - a7 */ 017,   017,  017,  017,  017,  017,  017,  017,
        /* a8 - af */ 017,   017,  017,  017,  0123, 017,  017,  017,
        /* b0 - b7 */ 0136,  017,  017,  017,  017,  017,  017,  017,
        /* b8 - bf */ 017,   017,  017,  017,  017,  017,  017,  017,
        /* c0 - c7 */ 017,   017,  017,  017,  017,  017,  017,  017,
        /* c8 - cf */ 017,   017,  017,  017,  017,  017,  017,  017,
        /* d0 - d7 */ 017,   017,  017,  017,  017,  017,  017,  0024,
        /* d8 - df */ 017,   017,  017,  017,  017,  017,  017,  017,
        /* e0 - e7 */ 017,   017,  017,  017,  017,  017,  017,  017,
        /* e8 - ef */ 017,   017,  017,  017,  017,  017,  017,  017,
        /* f0 - f7 */ 017,   017,  017,  017,  017,  017,  017,  0124,
        /* f8 - ff */ 017,   017,  017,  017,  017,  017,  017,  017,
    };
    switch (val >> 8) {
    case 0x00:
        return tab0[val];
    case 0x04:
        switch ((unsigned char)val) {
        case 0x10:
            return 0040;
        case 0x11:
            return 0041;
        case 0x12:
            return 0042;
        case 0x13:
            return 0043;
        case 0x14:
            return 0044;
        case 0x15:
            return 0045;
        case 0x16:
            return 0046;
        case 0x17:
            return 0047;
        case 0x18:
            return 0050;
        case 0x19:
            return 0051;
        case 0x1a:
            return 0052;
        case 0x1b:
            return 0053;
        case 0x1c:
            return 0054;
        case 0x1d:
            return 0055;
        case 0x1e:
            return 0056;
        case 0x1f:
            return 0057;
        case 0x20:
            return 0060;
        case 0x21:
            return 0061;
        case 0x22:
            return 0062;
        case 0x23:
            return 0063;
        case 0x24:
            return 0064;
        case 0x25:
            return 0065;
        case 0x26:
            return 0066;
        case 0x27:
            return 0067;
        case 0x28:
            return 0070;
        case 0x29:
            return 0071;
        case 0x2a:
            return 0135;
        case 0x2b:
            return 0072;
        case 0x2c:
            return 0073;
        case 0x2d:
            return 0074;
        case 0x2e:
            return 0075;
        case 0x2f:
            return 0076;
        case 0x30:
            return 0040;
        case 0x31:
            return 0041;
        case 0x32:
            return 0042;
        case 0x33:
            return 0043;
        case 0x34:
            return 0044;
        case 0x35:
            return 0045;
        case 0x36:
            return 0046;
        case 0x37:
            return 0047;
        case 0x38:
            return 0050;
        case 0x39:
            return 0051;
        case 0x3a:
            return 0052;
        case 0x3b:
            return 0053;
        case 0x3c:
            return 0054;
        case 0x3d:
            return 0055;
        case 0x3e:
            return 0056;
        case 0x3f:
            return 0057;
        case 0x40:
            return 0060;
        case 0x41:
            return 0061;
        case 0x42:
            return 0062;
        case 0x43:
            return 0063;
        case 0x44:
            return 0064;
        case 0x45:
            return 0065;
        case 0x46:
            return 0066;
        case 0x47:
            return 0067;
        case 0x48:
            return 0070;
        case 0x49:
            return 0071;
        case 0x4a:
            return 0135;
        case 0x4b:
            return 0072;
        case 0x4c:
            return 0073;
        case 0x4d:
            return 0074;
        case 0x4e:
            return 0075;
        case 0x4f:
            return 0076;
        }
        break;
    case 0x20:
        switch ((unsigned char)val) {
        case 0x15:
            return 0131;
        case 0x18:
            return 0032;
        case 0x19:
            return 0033;
        case 0x32:
            return 0137;
        case 0x3e:
            return 0115;
        }
        break;
    case 0x21:
        switch ((unsigned char)val) {
        case 0x2f:
            return 0020;
        case 0x91:
            return 0021;
        }
        break;
    case 0x22:
        switch ((unsigned char)val) {
        case 0x27:
            return 0121;
        case 0x28:
            return 0120;
        case 0x60:
            return 0034;
        case 0x61:
            return 0125;
        case 0x64:
            return 0116;
        case 0x65:
            return 0117;
        case 0x83:
            return 0122;
        }
        break;
    case 0x23:
        switch ((unsigned char)val) {
        case 0xe8:
            return 0020;
        }
        break;
    case 0x25:
        switch ((unsigned char)val) {
        case 0xc7:
            return 0127;
        case 0xca:
            return 0127;
        }
        break;
    }
    return 017;
}

void print_itm(const char *text)
{
    wchar_t string[100];

    sscanf(text, "%ls", string);

    for (wchar_t *p = string; *p; p++) {
        uint8_t gost = unicode_to_gost(*p);
        uint8_t itm  = gost_to_itm[gost];
        printf("'%o'", itm);
    }
    printf("\n");
}

int main()
{
    setlocale(LC_ALL, "");
    print_itm("ИТМ0encoding");
}
