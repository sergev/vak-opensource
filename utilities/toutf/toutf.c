/*
 * Convert text files from any cyrillic encoding to UTF-8.
 * Accepted input encodings are KOI8-R, CP-866 and CP-1251.
 *
 * Copyright (GPL) 2008 Serge Vakulenko, <serge@vak.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <utime.h>
#include <wchar.h>
#include <sys/stat.h>

/* Revision number is updated automatically by SVN. */
static const char revision[] = "$Rev: 104 $";

int verbose;
int force = 1;
int preserve;
int error;

/* Таблица флагов. */
struct option longopts[] = {
    /* option        has arg        integer code */
    { "help",           0,  0,  'h'     },
    { "version",        0,  0,  'V'     },
    { "verbose",        0,  0,  'v'     },
    { "force",          0,  0,  'f'     },
    { "preserve",       0,  0,  'p'     },
    { "interactive",    0,  0,  'i'     },
    { 0,                0,  0,  0       },
};

const unsigned short koi8_to_unicode [128] = {
    0x2500, 0x2502, 0x250c, 0x2510, 0x2514, 0x2518, 0x251c, 0x2524,
    0x252c, 0x2534, 0x253c, 0x2580, 0x2584, 0x2588, 0x258c, 0x2590,
    0x2591, 0x2592, 0x2593, 0x2320, 0x25a0, 0x2219, 0x221a, 0x2248,
    0x2264, 0x2265, 0xa0,   0x2321, 0xb0,   0xb2,   0xb7,   0xf7,
    0x2550, 0x2551, 0x2552, 0x0451, 0x2553, 0x2554, 0x2555, 0x2556,
    0x2557, 0x2558, 0x2559, 0x255a, 0x255b, 0x255c, 0x255d, 0x255e,
    0x255f, 0x2560, 0x2561, 0x0401, 0x2562, 0x2563, 0x2564, 0x2565,
    0x2566, 0x2567, 0x2568, 0x2569, 0x256a, 0x256b, 0x256c, 0xa9,
    0x044e, 0x0430, 0x0431, 0x0446, 0x0434, 0x0435, 0x0444, 0x0433,
    0x0445, 0x0438, 0x0439, 0x043a, 0x043b, 0x043c, 0x043d, 0x043e,
    0x043f, 0x044f, 0x0440, 0x0441, 0x0442, 0x0443, 0x0436, 0x0432,
    0x044c, 0x044b, 0x0437, 0x0448, 0x044d, 0x0449, 0x0447, 0x044a,
    0x042e, 0x0410, 0x0411, 0x0426, 0x0414, 0x0415, 0x0424, 0x0413,
    0x0425, 0x0418, 0x0419, 0x041a, 0x041b, 0x041c, 0x041d, 0x041e,
    0x041f, 0x042f, 0x0420, 0x0421, 0x0422, 0x0423, 0x0416, 0x0412,
    0x042c, 0x042b, 0x0417, 0x0428, 0x042d, 0x0429, 0x0427, 0x042a,
};

const unsigned short cp1251_to_unicode [128] = {
    0x0402, 0x0403, 0x201a, 0x0453, 0x201e, 0x2026, 0x2020, 0x2021,
    0x20ac, 0x2030, 0x0409, 0x2039, 0x040a, 0x040c, 0x040b, 0x040f,
    0x0452, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014,
    0x98,   0x2122, 0x0459, 0x203a, 0x045a, 0x045c, 0x045b, 0x045f,
    0xa0,   0x040e, 0x045e, 0x0408, 0xa4,   0x0490, 0xa6,   0xa7,
    0x0401, 0xa9,   0x0404, 0xab,   0xac,   0xad,   0xae,   0x0407,
    0xb0,   0xb1,   0x0406, 0x0456, 0x0491, 0xb5,   0xb6,   0xb7,
    0x0451, 0x2116, 0x0454, 0xbb,   0x0458, 0x0405, 0x0455, 0x0457,
    0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
    0x0418, 0x0419, 0x041a, 0x041b, 0x041c, 0x041d, 0x041e, 0x041f,
    0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
    0x0428, 0x0429, 0x042a, 0x042b, 0x042c, 0x042d, 0x042e, 0x042f,
    0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
    0x0438, 0x0439, 0x043a, 0x043b, 0x043c, 0x043d, 0x043e, 0x043f,
    0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
    0x0448, 0x0449, 0x044a, 0x044b, 0x044c, 0x044d, 0x044e, 0x044f,
};

const unsigned short cp866_to_unicode [128] = {
    0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
    0x0418, 0x0419, 0x041a, 0x041b, 0x041c, 0x041d, 0x041e, 0x041f,
    0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
    0x0428, 0x0429, 0x042a, 0x042b, 0x042c, 0x042d, 0x042e, 0x042f,
    0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
    0x0438, 0x0439, 0x043a, 0x043b, 0x043c, 0x043d, 0x043e, 0x043f,
    0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
    0x2555, 0x2563, 0x2551, 0x2557, 0x255d, 0x255c, 0x255b, 0x2510,
    0x2514, 0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x255e, 0x255f,
    0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0x2567,
    0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256b,
    0x256a, 0x2518, 0x250c, 0x2588, 0x2584, 0x258c, 0x2590, 0x2580,
    0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
    0x0448, 0x0449, 0x044a, 0x044b, 0x044c, 0x044d, 0x044e, 0x044f,
    0x0401, 0x0451, 0x0404, 0x0454, 0x0407, 0x0457, 0x040e, 0x045e,
    0xb0,   0x2219, 0xb7,   0x221a, 0x2116, 0xa4,   0x25a0, 0xa0,
};

/*
 * "Плохие" пары русских букв.
 * Взяты из скрипта a.charset.php Юрия Попова, popoff.donetsk.ua.
 */
wchar_t *bad_pair [] = {
    /* u0430 а */   L"ъыь",
    /* u0431 б */   L"йпфэ",
    /* u0432 в */   L"йфэ",
    /* u0433 г */   L"жйпфхцщъыьэю",
    /* u0434 д */   L"йфщ",
    /* u0435 е */   L"ъыь",
    /* u0436 ж */   L"зйтфхшщъыэя",
    /* u0437 з */   L"йпфхщ",
    /* u0438 и */   L"ъыь",
    /* u0439 й */   L"абжийущъыьэюя",
    /* u043A к */   L"бгйпфхщъыьэюя",
    /* u043B л */   L"йрцъэ",
    /* u043C м */   L"джзйъ",
    /* u043D н */   L"ймпъ",
    /* u043E о */   L"ъыь",
    /* u043F п */   L"бвгджзйхщъэю",
    /* u0440 р */   L"йъэ",
    /* u0441 с */   L"й",
    /* u0442 т */   L"жй",
    /* u0443 у */   L"ъыь",
    /* u0444 ф */   L"бвгджзйкпхцшщъьэюя",
    /* u0445 х */   L"бжзйфхцчщъыьюя",
    /* u0446 ц */   L"бгджзйлнпрстфхцчшщъьэюя",
    /* u0447 ч */   L"бгджзйпсфхцчщъыэюя",
    /* u0448 ш */   L"бгджзйфхшщъыэя",
    /* u0449 щ */   L"бвгджзйклмпстфхцчшщъыэюя",
    /* u044A ъ */   L"абвгджзийклмнопрстуфхцчшщъыьэ",
    /* u044B ы */   L"аофъыьэю",
    /* u044C ь */   L"айлрухъыьэ",
    /* u044D э */   L"абежиоуцчшщъыьэю",
    /* u044E ю */   L"иоуфъыьэя",
    /* u044F я */   L"аоуфъыьэ",
};

/*
 * Чтение символа из файла в кодировке UTF-8.
 */
int utf8_getc(FILE *fin)
{
    int c1, c2, c3;
again:
    c1 = getc(fin);
    if (c1 < 0 || ! (c1 & 0x80))
        return c1;
    c2 = getc(fin);
    if (! (c1 & 0x20))
        return (c1 & 0x1f) << 6 | (c2 & 0x3f);
    c3 = getc(fin);
    if (c1 == 0xEF && c2 == 0xBB && c3 == 0xBF) {
        /* Пропускаем нулевой пробел. */
        goto again;
    }
    return (c1 & 0x0f) << 12 | (c2 & 0x3f) << 6 | (c3 & 0x3f);
}

/*
 * Запись символа в файл в кодировке UTF-8.
 */
void utf8_putc(unsigned short ch, FILE *fout)
{
    if (ch < 0x80) {
        putc(ch, fout);
        return;
    }
    if (ch < 0x800) {
        putc(ch >> 6 | 0xc0, fout);
        putc((ch & 0x3f) | 0x80, fout);
        return;
    }
    putc(ch >> 12 | 0xe0, fout);
    putc(((ch >> 6) & 0x3f) | 0x80, fout);
    putc((ch & 0x3f) | 0x80, fout);
}

/*
 * Выдаём признак кодировки UTF-8 - пробел нулевой ширины.
 */
void utf8_puttag(FILE *fout)
{
    putc(0xEF, fout);
    putc(0xBB, fout);
    putc(0xBF, fout);
}

/*
 * Перепись файла с перекодировкой.
 */
void convert(FILE *fin, FILE *fout, const unsigned short *to_unicode)
{
    int ch;

    fseek(fin, 0L, 0);
/*  utf8_puttag(fout);*/
    for (;;) {
        ch = getc(fin);
        if (ch < 0)
            break;
        if (ch >= 0200)
            ch = to_unicode [ch - 0200];
        utf8_putc(ch, fout);
    }
}

/*
 * Определение кодировки по парам символов.
 */
int probe_encoding(FILE *fin, const unsigned short *to_unicode)
{
    int good_chars, bad_chars, bad_pairs, prev, ch;

    good_chars = 0;
    bad_chars = 0;
    bad_pairs = 0;
    prev = 0;
    fseek(fin, 0L, 0);
    for (;;) {
        ch = to_unicode ? getc(fin) : utf8_getc(fin);
        if (ch < 0)
            break;
        if (ch < 0200) {
            prev = 0;
            continue;
        }
        if (to_unicode) {
            /* Преобразуем в Юникод. */
            ch = to_unicode [ch - 0200];
        }

        /* Преобразуем в маленькие буквы. */
        if (ch >= L'А' && ch <= L'Я')
            ch += L'а' - L'А';
        else if (ch == L'Ё' || ch == L'ё')
            ch = L'е';
        else if (ch < L'а' || ch > L'я') {
            ++bad_chars;
            prev = 0;
            continue;
        }
        ++good_chars;
        if (prev && wcschr(bad_pair [prev - L'а'], ch)) {
            ++bad_pairs;
            if (verbose > 1) {
                printf("    bad pair: ");
                utf8_putc(prev, stdout);
                printf("-");
                utf8_putc(ch, stdout);
                printf(", %04x-%04x\n", prev, ch);
            }
        }
        prev = ch;
    }
    if (verbose > 1) {
        printf("*** %s: good chars = %d, bad chars = %d, bad pairs = %d\n",
            to_unicode == koi8_to_unicode ? "koi8" :
            to_unicode == cp866_to_unicode ? "cp866" :
            to_unicode == cp1251_to_unicode ? "cp1251" : "utf",
            good_chars, bad_chars, bad_pairs);
    }
    return 32 * bad_pairs + 64 * bad_chars - good_chars;
}

/*
 * Определение кодировки текста, находящегося в файле.
 */
char *file_detect_encoding(FILE *fin)
{
    int weight_koi, weight_dos, weight_win, weight_utf;

    /* Определяем "вес" кодировок, чем больше тем хуже. */
    weight_koi = probe_encoding(fin, koi8_to_unicode);
    weight_dos = probe_encoding(fin, cp866_to_unicode);
    weight_win = probe_encoding(fin, cp1251_to_unicode);
    weight_utf = probe_encoding(fin, 0);
    if (verbose > 1) {
        printf("*** weight: koi=%d, dos=%d, win=%d, utf=%d\n",
            weight_koi, weight_dos, weight_win, weight_utf);
    }
    if (weight_koi >= 0 && weight_dos >= 0 && weight_win >= 0 &&
        weight_utf >= 0) {
        /* Нерусский файл. */
        return 0;
    }

    /* Находим лучшую кодировку. */
    if (weight_koi <= weight_dos && weight_koi <= weight_win &&
        weight_koi <= weight_utf)
        return "koi8-r";

    if (weight_dos <= weight_koi && weight_dos <= weight_win &&
        weight_dos <= weight_utf)
        return "cp866";

    if (weight_win <= weight_koi && weight_win <= weight_dos &&
        weight_win <= weight_utf)
        return "cp1251";

    return "utf-8";
}

/*
 * Определяем, есть ли в первом килобайте нетекстовые символы.
 */
int is_binary_file(FILE *fin)
{
    int bytes, bad_chars, ch;

    bad_chars = 0;
    fseek(fin, 0L, 0);
    for (bytes=0; bytes<1024; ++bytes) {
        ch = getc(fin);
        if (ch < 0)
            break;
        if (ch >= ' ' && ch != 0177)
            continue;
        if (ch == '\n' || ch == '\r' || ch == '\t' ||
            ch == '\b' || ch == '\f' || ch == '\v')
            continue;
        ++bad_chars;
    }
    return (bad_chars >= 2);
}

int ask_yes(char *message)
{
    char reply [80];

    for (;;) {
        printf("%s (y, N): ", message);
        if (!fgets(reply, sizeof(reply), stdin))
            return 0;
        if (reply[0] == 'y' || reply[0] == 'Y')
            return 1;
        if (reply[0] == 'n' || reply[0] == 'N' ||
            reply[0] == '\n' || reply[0] == '\r')
            return 0;
        printf("\nEnter `y' or `n'.\n");
    }
}

void process(char *filename)
{
    FILE *fin, *fout;
    char *encoding, *newname;
    struct stat st;
    struct utimbuf ut;

    if (stat(filename, &st) < 0) {
        perror(filename);
        error = 1;
        return;
    }
    if (! S_ISREG(st.st_mode)) {
        if (verbose)
            printf("%s: not a file, skipped\n", filename);
        return;
    }
    fin = fopen(filename, "rb");
    if (! fin) {
        perror(filename);
        error = 1;
        return;
    }
    if (is_binary_file(fin)) {
        fclose(fin);
        if (verbose)
            printf("%s: binary file, skipped\n", filename);
        return;
    }
    encoding = file_detect_encoding(fin);
    if (! encoding) {
        fclose(fin);
        if (verbose)
            printf("%s: unknown encoding, skipped\n", filename);
        return;
    }
    if (strcmp(encoding, "utf-8") == 0) {
        fclose(fin);
        if (verbose)
            printf("%s: already utf-8, skipped\n", filename);
        return;
    }
    if (force) {
        printf("%s: convert from %s to utf-8\n", filename, encoding);
    } else {
        printf("File %s has encoding %s\n", filename, encoding);
        if (! ask_yes("Convert to utf-8?")) {
            fclose(fin);
            return;
        }
    }

    /* Создаём новый файл. */
    newname = malloc(strlen(filename) + 2);
    if (! newname) {
        fclose(fin);
        fprintf(stderr, "%s: out of memory\n", filename);
        error = 1;
        return;
    }
    strcpy(newname, filename);
    strcat(newname, "~");
    fout = fopen(newname, "wb");
    if (! fout) {
        fclose(fin);
        perror(newname);
        free(newname);
        error = 1;
        return;
    }

    /* Перекодируем. */
    if (strcmp(encoding, "koi8-r") == 0)
        convert(fin, fout, koi8_to_unicode);
    else if (strcmp(encoding, "cp866") == 0)
        convert(fin, fout, cp866_to_unicode);
    else if (strcmp(encoding, "cp1251") == 0)
        convert(fin, fout, cp1251_to_unicode);
    else {
        /* Cannot happen. */
        fprintf(stderr, "%s: internal error\n", filename);
        abort();
    }
    fclose(fin);
    fclose(fout);

    /* Переименовываем. */
    if (rename(newname, filename) < 0) {
        perror(filename);
        free(newname);
        error = 1;
        return;
    }
    free(newname);

    if (preserve) {
        /* Ставим новому файлу старую дату. */
        ut.actime = st.st_atime;
        ut.modtime = st.st_mtime;
        utime(filename, &ut);
    }
}

void usage()
{
    fprintf(stderr, "UTF encoder, Version 1.%d, Copyright (GPL) Serge Vakulenko\n",
        atoi(revision+6));
    fprintf(stderr, "This is free software, covered by the GNU General Public License.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Automatic conversion from KOI8-R, CP-1251 and CP-866 to UTF-8.\n");
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "        toutf [options] file...\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -v, --verbose      display information encoding detection\n");
    fprintf(stderr, "  -i, --interactive  prompt before convertion\n");
    fprintf(stderr, "  -f, --force        do not prompt before convertion (default)\n");
    fprintf(stderr, "  -p, --preserve     preserve moditivation times\n");
    exit(1);
}

int main(int argc, char **argv)
{
    int i;

    for (;;) {
        i = getopt_long(argc, argv, "hVvfpi", longopts, 0);
        if (i < 0)
            break;
        switch (i) {
        case 'h':
            usage();
            break;
        case 'V':
            printf("Version: 1.%d\n", atoi(revision+6));
            return 0;
        case 'v':
            ++verbose;
            break;
        case 'f':
            force = 1;
            break;
        case 'i':
            force = 0;
            break;
        case 'p':
            preserve = 1;
            break;
        }
    }
    if (optind >= argc)
        usage();

    while (optind < argc)
        process(argv[optind++]);
    return error;
}
