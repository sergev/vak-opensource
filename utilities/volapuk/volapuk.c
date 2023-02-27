//
// Encode/decode text file as volapuk.
//
// Copyright (C) 2005-2023 Serge Vakulenko, <serge.vakulenko@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include "cyrillic.h"

const char version[] = "1.1";
const char copyright[] = "Copyright (C) 2006-2023 Serge Vakulenko";

char *progname;

//
// Get Unicode character from input stream.
// Decode UTF-8 as follows:
// 00000000.0xxxxxxx -> 0xxxxxxx
// 00000xxx.xxyyyyyy -> 110xxxxx, 10yyyyyy
// xxxxyyyy.yyzzzzzz -> 1110xxxx, 10yyyyyy, 10zzzzzz
//
int getc_utf8(FILE *fd)
{
    int c1, c2, c3;

    c1 = getc(fd);
    if (c1 < 0 || !(c1 & 0x80))
        return c1;
    c2 = getc(fd);
    if (!(c1 & 0x20))
        return (c1 & 0x1f) << 6 | (c2 & 0x3f);
    c3 = getc(fd);
    return (c1 & 0x0f) << 12 | (c2 & 0x3f) << 6 | (c3 & 0x3f);
}

//
// Write Unicode character to the output stream.
// Encode as UTF-8.
//
void putc_utf8(unsigned short c, FILE *fd)
{
    if (c < 0x80) {
        putc(c, fd);
        return;
    }
    if (c < 0x800) {
        putc(c >> 6 | 0xc0, fd);
        putc((c & 0x3f) | 0x80, fd);
        return;
    }
    putc(c >> 12 | 0xe0, fd);
    putc(((c >> 6) & 0x3f) | 0x80, fd);
    putc((c & 0x3f) | 0x80, fd);
}

//
// Encode one character as volapuk, and write to the stream.
//
void putc_volapuk(int c, FILE *fd)
{
    switch (c) {
    // clang-format off
    default: putc_utf8(c, stdout); break;

    case CYRILLIC_CAPITAL_LETTER_A:         fputs("A", fd); break;  // А -> A
    case CYRILLIC_CAPITAL_LETTER_BE:        fputs("B", fd); break;  // Б -> B
    case CYRILLIC_CAPITAL_LETTER_VE:        fputs("V", fd); break;  // В -> V
    case CYRILLIC_CAPITAL_LETTER_GHE:       fputs("G", fd); break;  // Г -> G
    case CYRILLIC_CAPITAL_LETTER_DE:        fputs("D", fd); break;  // Д -> D
    case CYRILLIC_CAPITAL_LETTER_IE:        fputs("E", fd); break;  // Е -> E
    case CYRILLIC_CAPITAL_LETTER_IO:        fputs("O`", fd); break; // Ё -> O`
    case CYRILLIC_CAPITAL_LETTER_ZHE:       fputs("J`", fd); break; // Ж -> J`
    case CYRILLIC_CAPITAL_LETTER_ZE:        fputs("Z", fd); break;  // З -> Z
    case CYRILLIC_CAPITAL_LETTER_I:         fputs("I", fd); break;  // И -> I
    case CYRILLIC_CAPITAL_LETTER_SHORT_I:   fputs("J", fd); break;  // Й -> J
    case CYRILLIC_CAPITAL_LETTER_KA:        fputs("K", fd); break;  // К -> K
    case CYRILLIC_CAPITAL_LETTER_EL:        fputs("L", fd); break;  // Л -> L
    case CYRILLIC_CAPITAL_LETTER_EM:        fputs("M", fd); break;  // М -> M
    case CYRILLIC_CAPITAL_LETTER_EN:        fputs("N", fd); break;  // Н -> N
    case CYRILLIC_CAPITAL_LETTER_O:         fputs("O", fd); break;  // О -> O
    case CYRILLIC_CAPITAL_LETTER_PE:        fputs("P", fd); break;  // П -> P
    case CYRILLIC_CAPITAL_LETTER_ER:        fputs("R", fd); break;  // Р -> R
    case CYRILLIC_CAPITAL_LETTER_ES:        fputs("S", fd); break;  // С -> S
    case CYRILLIC_CAPITAL_LETTER_TE:        fputs("T", fd); break;  // Т -> T
    case CYRILLIC_CAPITAL_LETTER_U:         fputs("U", fd); break;  // У -> U
    case CYRILLIC_CAPITAL_LETTER_EF:        fputs("F", fd); break;  // Ф -> F
    case CYRILLIC_CAPITAL_LETTER_HA:        fputs("H", fd); break;  // Х -> H
    case CYRILLIC_CAPITAL_LETTER_TSE:       fputs("C", fd); break;  // Ц -> C
    case CYRILLIC_CAPITAL_LETTER_CHE:       fputs("C`", fd); break; // Ч -> C`
    case CYRILLIC_CAPITAL_LETTER_SHA:       fputs("S`", fd); break; // Ш -> S`
    case CYRILLIC_CAPITAL_LETTER_SHCHA:     fputs("H`", fd); break; // Щ -> H`
    case CYRILLIC_CAPITAL_LETTER_HARD_SIGN: fputs("X`", fd); break; // Ъ -> X`
    case CYRILLIC_CAPITAL_LETTER_YERU:      fputs("Y", fd); break;  // Ы -> Y
    case CYRILLIC_CAPITAL_LETTER_SOFT_SIGN: fputs("X", fd); break;  // Ь -> X
    case CYRILLIC_CAPITAL_LETTER_E:         fputs("E`", fd); break; // Э -> E`
    case CYRILLIC_CAPITAL_LETTER_YU:        fputs("U`", fd); break; // Ю -> U`
    case CYRILLIC_CAPITAL_LETTER_YA:        fputs("A`", fd); break; // Я -> A`

    case CYRILLIC_SMALL_LETTER_A:           fputs("a", fd); break;  // а -> a
    case CYRILLIC_SMALL_LETTER_BE:          fputs("b", fd); break;  // б -> b
    case CYRILLIC_SMALL_LETTER_VE:          fputs("v", fd); break;  // в -> v
    case CYRILLIC_SMALL_LETTER_GHE:         fputs("g", fd); break;  // г -> g
    case CYRILLIC_SMALL_LETTER_DE:          fputs("d", fd); break;  // д -> d
    case CYRILLIC_SMALL_LETTER_IE:          fputs("e", fd); break;  // е -> e
    case CYRILLIC_SMALL_LETTER_IO:          fputs("o`", fd); break; // ё -> o`
    case CYRILLIC_SMALL_LETTER_ZHE:         fputs("j`", fd); break; // ж -> j`
    case CYRILLIC_SMALL_LETTER_ZE:          fputs("z", fd); break;  // з -> z
    case CYRILLIC_SMALL_LETTER_I:           fputs("i", fd); break;  // и -> i
    case CYRILLIC_SMALL_LETTER_SHORT_I:     fputs("j", fd); break;  // й -> j
    case CYRILLIC_SMALL_LETTER_KA:          fputs("k", fd); break;  // к -> k
    case CYRILLIC_SMALL_LETTER_EL:          fputs("l", fd); break;  // л -> l
    case CYRILLIC_SMALL_LETTER_EM:          fputs("m", fd); break;  // м -> m
    case CYRILLIC_SMALL_LETTER_EN:          fputs("n", fd); break;  // н -> n
    case CYRILLIC_SMALL_LETTER_O:           fputs("o", fd); break;  // о -> o
    case CYRILLIC_SMALL_LETTER_PE:          fputs("p", fd); break;  // п -> p
    case CYRILLIC_SMALL_LETTER_ER:          fputs("r", fd); break;  // р -> r
    case CYRILLIC_SMALL_LETTER_ES:          fputs("s", fd); break;  // с -> s
    case CYRILLIC_SMALL_LETTER_TE:          fputs("t", fd); break;  // т -> t
    case CYRILLIC_SMALL_LETTER_U:           fputs("u", fd); break;  // у -> u
    case CYRILLIC_SMALL_LETTER_EF:          fputs("f", fd); break;  // ф -> f
    case CYRILLIC_SMALL_LETTER_HA:          fputs("h", fd); break;  // х -> h
    case CYRILLIC_SMALL_LETTER_TSE:         fputs("c", fd); break;  // ц -> c
    case CYRILLIC_SMALL_LETTER_CHE:         fputs("c`", fd); break; // ч -> c`
    case CYRILLIC_SMALL_LETTER_SHA:         fputs("s`", fd); break; // ш -> s`
    case CYRILLIC_SMALL_LETTER_SHCHA:       fputs("h`", fd); break; // щ -> h`
    case CYRILLIC_SMALL_LETTER_HARD_SIGN:   fputs("x`", fd); break; // ъ -> x`
    case CYRILLIC_SMALL_LETTER_YERU:        fputs("y", fd); break;  // ы -> y
    case CYRILLIC_SMALL_LETTER_SOFT_SIGN:   fputs("x", fd); break;  // ь -> x
    case CYRILLIC_SMALL_LETTER_E:           fputs("e`", fd); break; // э -> e`
    case CYRILLIC_SMALL_LETTER_YU:          fputs("u`", fd); break; // ю -> u`
    case CYRILLIC_SMALL_LETTER_YA:          fputs("a`", fd); break; // я -> a`
    // clang-format on
    }
}

//
// If next character is `, return b, otherwise return a.
//
int decode(FILE *fd, int a, int b)
{
    int c = getc_utf8(fd);
    if (c < 0)
        return a;

    if (c == '`')
        return b;

    ungetc(c, fd);
    return a;
}

//
// Read one character from the stream, decode as volapuk.
//
int getc_volapuk(FILE *fd)
{
    int c = getc_utf8(fd);
    if (c < 0)
        return c;

    switch (c) {
    // clang-format off
    default:  return c;
    case 'A': return decode(fd, CYRILLIC_CAPITAL_LETTER_A,          // A -> А
                                CYRILLIC_CAPITAL_LETTER_YA);        // A`-> Я
    case 'B': return CYRILLIC_CAPITAL_LETTER_BE;                    // B -> Б
    case 'C': return decode(fd, CYRILLIC_CAPITAL_LETTER_TSE,        // C -> Ц
                                CYRILLIC_CAPITAL_LETTER_CHE);       // C`-> Ч
    case 'D': return CYRILLIC_CAPITAL_LETTER_DE;                    // D -> Д
    case 'E': return decode(fd, CYRILLIC_CAPITAL_LETTER_IE,         // E -> Е
                                CYRILLIC_CAPITAL_LETTER_E);         // E`-> Э
    case 'F': return CYRILLIC_CAPITAL_LETTER_EF;                    // F -> Ф
    case 'G': return CYRILLIC_CAPITAL_LETTER_GHE;                   // G -> Г
    case 'H': return decode(fd, CYRILLIC_CAPITAL_LETTER_HA,         // H -> Х
                                CYRILLIC_CAPITAL_LETTER_SHCHA);     // H`-> Щ
    case 'I': return CYRILLIC_CAPITAL_LETTER_I;                     // I -> И
    case 'J': return decode(fd, CYRILLIC_CAPITAL_LETTER_SHORT_I,    // J -> Й
                                CYRILLIC_CAPITAL_LETTER_ZHE);       // J`-> Ж
    case 'K': return CYRILLIC_CAPITAL_LETTER_KA;                    // K -> К
    case 'L': return CYRILLIC_CAPITAL_LETTER_EL;                    // L -> Л
    case 'M': return CYRILLIC_CAPITAL_LETTER_EM;                    // M -> М
    case 'N': return CYRILLIC_CAPITAL_LETTER_EN;                    // N -> Н
    case 'O': return decode(fd, CYRILLIC_CAPITAL_LETTER_O,          // O -> О
                                CYRILLIC_CAPITAL_LETTER_IO);        // O`-> Ё
    case 'P': return CYRILLIC_CAPITAL_LETTER_PE;                    // P -> П
    case 'R': return CYRILLIC_CAPITAL_LETTER_ER;                    // R -> Р
    case 'S': return decode(fd, CYRILLIC_CAPITAL_LETTER_ES,         // S -> С
                                CYRILLIC_CAPITAL_LETTER_SHA);       // S`-> Ш
    case 'T': return CYRILLIC_CAPITAL_LETTER_TE;                    // T -> Т
    case 'U': return decode(fd, CYRILLIC_CAPITAL_LETTER_U,          // U -> У
                                CYRILLIC_CAPITAL_LETTER_YU);        // U`-> Ю
    case 'V': return CYRILLIC_CAPITAL_LETTER_VE;                    // V -> В
    case 'X': return decode(fd, CYRILLIC_CAPITAL_LETTER_SOFT_SIGN,  // X -> Ь
                                CYRILLIC_CAPITAL_LETTER_HARD_SIGN); // X`-> Ъ
    case 'Y': return CYRILLIC_CAPITAL_LETTER_YERU;                  // Y -> Ы
    case 'Z': return CYRILLIC_CAPITAL_LETTER_ZE;                    // Z -> З
    case 'a': return decode(fd, CYRILLIC_SMALL_LETTER_A,            // a -> а
                                CYRILLIC_SMALL_LETTER_YA);          // a`-> я
    case 'b': return CYRILLIC_SMALL_LETTER_BE;                      // b -> б
    case 'c': return decode(fd, CYRILLIC_SMALL_LETTER_TSE,          // c -> ц
                                CYRILLIC_SMALL_LETTER_CHE);         // c`-> ч
    case 'd': return CYRILLIC_SMALL_LETTER_DE;                      // d -> д
    case 'e': return decode(fd, CYRILLIC_SMALL_LETTER_IE,           // e -> е
                                CYRILLIC_SMALL_LETTER_E);           // e`-> э
    case 'f': return CYRILLIC_SMALL_LETTER_EF;                      // f -> ф
    case 'g': return CYRILLIC_SMALL_LETTER_GHE;                     // g -> г
    case 'h': return decode(fd, CYRILLIC_SMALL_LETTER_HA,           // h -> х
                                CYRILLIC_SMALL_LETTER_SHCHA);       // h`-> щ
    case 'i': return CYRILLIC_SMALL_LETTER_I;                       // i -> и
    case 'j': return decode(fd, CYRILLIC_SMALL_LETTER_SHORT_I,      // j -> й
                                CYRILLIC_SMALL_LETTER_ZHE);         // j`-> ж
    case 'k': return CYRILLIC_SMALL_LETTER_KA;                      // k -> к
    case 'l': return CYRILLIC_SMALL_LETTER_EL;                      // l -> л
    case 'm': return CYRILLIC_SMALL_LETTER_EM;                      // m -> м
    case 'n': return CYRILLIC_SMALL_LETTER_EN;                      // n -> н
    case 'o': return decode(fd, CYRILLIC_SMALL_LETTER_O,            // o -> о
                                CYRILLIC_SMALL_LETTER_IO);          // o`-> ё
    case 'p': return CYRILLIC_SMALL_LETTER_PE;                      // p -> п
    case 'r': return CYRILLIC_SMALL_LETTER_ER;                      // r -> р
    case 's': return decode(fd, CYRILLIC_SMALL_LETTER_ES,           // s -> с
                                CYRILLIC_SMALL_LETTER_SHA);         // s`-> ш
    case 't': return CYRILLIC_SMALL_LETTER_TE;                      // t -> т
    case 'u': return decode(fd, CYRILLIC_SMALL_LETTER_U,            // u -> у
                                CYRILLIC_SMALL_LETTER_YU);          // u`-> ю
    case 'v': return CYRILLIC_SMALL_LETTER_VE;                      // v -> в
    case 'x': return decode(fd, CYRILLIC_SMALL_LETTER_SOFT_SIGN,    // x -> ь
                                CYRILLIC_SMALL_LETTER_HARD_SIGN);   // x`-> ъ
    case 'y': return CYRILLIC_SMALL_LETTER_YERU;                    // y -> ы
    case 'z': return CYRILLIC_SMALL_LETTER_ZE;                      // z -> з
    // clang-format on
    }
}

//
// Read the file and encode/decode as volapuk.
// Write to stdout.
//
void process(FILE *fd, int decode_flag)
{
    int c;

    if (decode_flag) {
        // Decode.
        for (;;) {
            c = getc_volapuk(fd);
            if (c < 0)
                break;

            putc_utf8(c, stdout);
        }
    } else {
        // Encode.
        for (;;) {
            c = getc_utf8(fd);
            if (c < 0)
                break;

            putc_volapuk(c, stdout);
        }
    }
}

void usage()
{
    fprintf(stderr, "Volapuk encoder, Version %s, %s\n", version, copyright);
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "    %s [-d] [file]\n", progname);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "    -d        decode\n");
    exit(-1);
}

int main(int argc, char **argv)
{
    int decode_flag = 0;

    progname = *argv;
    for (;;) {
        switch (getopt(argc, argv, "dh")) {
        case EOF:
            break;
        case 'd':
            decode_flag = 1;
            continue;
        case 'h':
        default:
            usage();
        }
        break;
    }
    argc -= optind;
    argv += optind;

    if (argc > 1) {
        usage();
    }

    if (argc == 1) {
        FILE *fd;

        fd = fopen(argv[0], "r");
        if (!fd) {
            perror(argv[0]);
            exit(-1);
        }
        process(fd, decode_flag);
    } else {
        process(stdin, decode_flag);
    }

    return (0);
}
