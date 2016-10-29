/***************************************************************************
 *   Copyright (C) 2007 by SEMIKO                                          *
 *   mail@semico.ru                                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Коды выхода BSD from @(#)sysexits.h  8.1 (Berkeley) 6/2/93
// Copyright (c) 1987, 1993
// The Regents of the University of California.  All rights reserved.
#define EX_USAGE    64  /* command line usage error */
#define EX_DATAERR  65  /* data format error */
#define EX_NOINPUT  66  /* cannot open input */
#define EX_NOUSER   67  /* addressee unknown */
#define EX_NOHOST   68  /* host name unknown */
#define EX_UNAVAILABLE  69  /* service unavailable */
#define EX_SOFTWARE 70  /* internal software error */
#define EX_OSERR    71  /* system error (e.g., can't fork) */
#define EX_OSFILE   72  /* critical OS file missing */
#define EX_CANTCREAT    73  /* can't create (user) output file */
#define EX_IOERR    74  /* input/output error */
#define EX_TEMPFAIL 75  /* temp failure; user is invited to retry */
#define EX_PROTOCOL 76  /* remote error in protocol */
#define EX_NOPERM   77  /* permission denied */
#define EX_CONFIG   78  /* configuration error */
#define EX_COMPILATION  100 /* error in a compilation */
// end sysexits.h

char mkversion[]="Version 0.27";


// 0.11 - устранена ошибка
// 0.12 - добавлена команда РР В/О
// 0.13 - введена ошибка компилятора Е015: совпадение имени метки и идентификатора
//  - исправлена ошибка перекодирвки в/из UTF-8 и UCS-2
// NPP  - Исправлена ошибка невывода в .htm команды "F X=0" строка 2790 (mkkom.name1[94])
// 0.14 - изменен номер версии для исправления, сделанного NPP см.: http://arbinada.com/pmk/node/403
// 0.15 - исправлена ошибка операторов DA DAH DAL при разборе меток на втором проходе
// 0.16 - исправлена ошибка v0.15 при разборе меток на втором проходе
// NPP - обнаружена ошибка вывода в начало файла .mkp лишнего байта равного нулю
// 0.17 - изменён номер версии для устранения ошибки, обнаруженной NPP см.: http://arbinada.com/pmk/node/403
// 0.18 - добавлены псевдооператоры .DD .DDH .DDL (адреса DEC-HEX)
//  - исправлены ошибки в реализации .DA .DAH .DAL (адреса DEC-BCD)
//  - [соответствует mk.exe v1.14]
//  - используется файл конфигурации пакета mk.cfg (вместо mkl2mkp.cfg)
//  - не используется установка кодировки ключом -c
// 0.19 - корректная обработка букв Ё и ё в кодировке 1251 при разборе .TEXT
// 0.20 (15.01.10) - используется файл конфигурации mkcfg v0.11, распознавание mkcfg v0.10
// 0.21 (16.04.10) - добавлен псевдооператор .NUMT - адрес из трёх байт
//  - в MKP добавлен признак текста 200h (hex - 100h)
//  - изменён формат вывода в TXT и HTM файлы
//  - ключ -i - вывод информации
//  - предупреждение W003 - четырёхзначное число в NUMT
// 0.22 (21.04.10) - в вывод помощи -h добавлена информация по -i, сообщения переведены на русский
//  - дополнительная проверка при выводе символов текста в HTM-файлы
// 0.23 (28.03.11)
//  - исправлена ошибка в реализации .DD;
//  - в операторах .DA, .DB, .DD строки могут заканчиватся запятой;
//  - добавлены псевдооператоры .BASE и .DBB - установка базы и запись смещения относительно базы.
// 0.24 (14.10.11)
//  - добавлены псевдооператоры .NUMD и .NUMI;
//  - исправлена ошибка при вычислении контрольной суммы XOR для HTM-файла
//  - убраны лишние символы пробелов в HTM-файле
// 0.25 (14.11.11)
//  - добавлены псевдооператоры .DW и .DWB
// 0.25 En (06.08.13)
//  - интерфейс переведен на английский
// (Перевод выполнил Vitali Samurov aka Vitasam - https://gitorious.org/mk161_stuff )
// 0.26 En (08.08.13)
//  - вывод об ошибках модифицирован для интеграции с парсером Geany.
// (Vitali Samurov aka Vitasam - https://gitorious.org/mk161_stuff )
// 0.27 (11.11.13)
// - добавлены команды интерфейса (макросы) .FORM, .TAB, .LDX, .LDY, .TYPE,
// .ATYPE, .LDADR, .SQU, .FRAME, .DRAWR, .DRAWB, .TXTP, .TXTB, .TXTK,
// .PREXP, .PRFORM, .PRHEX, .MCALL, .RETUM, .LCALL, .RETUL, .JNEZ, .JM, .END.
// - для обозначения конца секции MKP используется .ENDP вместо .END


int dos=0;  // 0-GNU/Linux 1-DOS 2-Windows (Win32)
char mes[24][256];  // сообщения программы

char fname[6][256]; //0-MKL без расширения 1-MKL исходный 2-MKP 3-TXT 4-HTM
int fname_vsego;    // всего файлов в строке аргументов
FILE *fp0, *fp1,*fp2, *fp3, *fp4;   //потоки 0-CFG 1-MKL 2-MKP 3-MKP.TXT 4-MKP.HTM

int kluch_i;        /* ключ -i - вывод информации при компилировании v0.21 */

char temp[256];
char ctemp[512];    /* вспомогательная строка */
char ctemp1[512];   /* вспомогательная строка */
char ctemptoa[40]; // вспомогательная строка для sprintf (DOS - itoa ltoa)
int mkbufp[10240]; /* буфер для компиляции программы */

int flag_mkbufp;    /* 0 - нет информации 1 - есть информация */
int comp_const=4096;    /* константа признаков для первого прохода - с v0.21*/
int comp_progks1=0;
int comp_progks2=0; /* контрольные суммы ADD XOR */

long comp_stroka;   /* текущая компилируемая строка */
long comp_adres;    /* текущий адрес в mkbufp при компиляции */
long comp_chislo;  /* число считанное comp_get_chislo */
long comp_charset; /* кодировка 1251 или 866 (пока) */
char comp_metka[40];  /* метка считанная comp_get_metka */
char comp_ident[40];  /* идентификатор считанный comp_get_ident */
char comp_text[256];
int comp_flag_adres;    /* 0-только десятичные адреса 1-допустимы шестнадцатиричные цифры */

int comp_progmin;   // начало программы в страницах
int comp_proglen;   // длина программы в страницах

long comp_base;     // v0.23 (v1.18 mk.exe) - текущая база для операторов .BASE и .DBB

int mkiocharset;    // 0=CP866|1=CP1251|2=KOI8R|3=UTF8|4=UCS2

char compilation_error = 0;

struct MKKOM{ // команды МК
        int len[256];   // количество доп байт команды 0-нет
        int par[256];  // параметр команды 0-нет; 1,2 - адрес; 3,4 - номер регистра 1 или 2 байта
        char name[256][16]; // мнемоника команды ЛАТ
        char name1[256][20]; // мнемоника команды РУС HTML [20] 17 min!
        char name2[256][16]; // мнемоника команды LAT комп (упакованная без пробелов)
        int init; // признак инициализации структуры 0-нет 1-есть
        } mkkom;


struct METKA // структура меток для компилятора
    {
    char name[10000][34]; // имя
    long val[10000];        // значение
    long stroka[10000];     // строка программы в которой использована метка
    int use[10000];     // использование 0-нет,
                        // 1 - есть val (определено)
                        // 2 - используется в тексте
                        // после компиляции все метки должны быть 3 (по ИЛИ)
    long vsego;         // всего установлено меток
    } metka;

struct MBASE
    {       // v0.23 (mk_v118.exe)для операторов .BASE и .DBB - список баз
    long val[10004];    // значение
    long stroka [10004];    // строка программы в которой установлена база
    long vsego;         // всего установлено баз
    } mbase;

struct IDENT // структура идентификаторов EQU для компилятора
    {
    char name[10000][34]; // имя идентификатора
    long val[10000];        // значение
    long stroka[10000];     // строка программы в которой определен
    int use[10000];     // использование 0-нет,
                        // 1 - есть val (.EQU)
                        // 2 - используется в тексте
                        // после компиляции все метки должны быть 3 (по ИЛИ)
    long vsego;         // всего установлено идентификаторов
    } ident;

struct MKERROR // таблица найденных ошибок компиляции
    {
    int n[1024];    // тип ошибки
    long stroka[1024];
    char sss[1024][40]; // сопровождающий текст (например имя метки или непонятный оператор)
    long vsego;     // всего при компиляции ошибок

    char name[20][256]; // описание ошибок с номером n
    int w;          // номер с которого идут предупреждения, ошибки начинаются с 0
    int initvsego;      // количество сообщений (name)=20
    int init;       // признак инициализации структуры 0-нет 1 -есть
    } mkerror;

long mkcharset[68][5]= //а=0 я=32 А=33 Я=65 0-866 1-1251 2-koi8 3-utf8 4-ucs2
    {0xA0,0xE0,0xC1,0xD0B0,0x0430, // а
    0xA1,0xE1,0xC2,0xD0B1,0x0431, // б
    0xA2,0xE2,0xD7,0xD0B2,0x0432,
    0xA3,0xE3,0xC7,0xD0B3,0x0433,
    0xA4,0xE4,0xC4,0xD0B4,0x0434,
    0xA5,0xE5,0xC5,0xD0B5,0x0435,   // е
    0xF1,0xB8,0xA3,0xD191,0x0451, // ё
    0xA6,0xE6,0xD6,0xD0B6,0x0436,   // ж
    0xA7,0xE7,0xDA,0xD0B7,0x0437,
    0xA8,0xE8,0xC9,0xD0B8,0x0438,
    0xA9,0xE9,0xCA,0xD0B9,0x0439,
    0xAA,0xEA,0xCB,0xD0BA,0x043A,
    0xAB,0xEB,0xCC,0xD0BB,0x043B,
    0xAC,0xEC,0xCD,0xD0BC,0x043C,
    0xAD,0xED,0xCE,0xD0BD,0x043D,
    0xAE,0xEE,0xCF,0xD0BE,0x043E,
    0xAF,0xEF,0xD0,0xD0BF,0x043F, // п

    0xE0,0xF0,0xD2,0xD180,0x0440,   // р
    0xE1,0xF1,0xD3,0xD181,0x0441,
    0xE2,0xF2,0xD4,0xD182,0x0442,
    0xE3,0xF3,0xD5,0xD183,0x0443,
    0xE4,0xF4,0xC6,0xD184,0x0444,
    0xE5,0xF5,0xC8,0xD185,0x0445,
    0xE6,0xF6,0xC3,0xD186,0x0446,
    0xE7,0xF7,0xDE,0xD187,0x0447,
    0xE8,0xF8,0xDB,0xD188,0x0448,
    0xE9,0xF9,0xDD,0xD189,0x0449,
    0xEA,0xFA,0xDF,0xD18A,0x044A,
    0xEB,0xFB,0xD9,0xD18B,0x044B,
    0xEC,0xFC,0xD8,0xD18C,0x044C,
    0xED,0xFD,0xDC,0xD18D,0x044D,
    0xEE,0xFE,0xC0,0xD18E,0x044E,
    0xEF,0xFF,0xD1,0xD18F,0x044F,   // я

    0x80,0xC0,0xE1,0xD090,0x0410,   // А
    0x81,0xC1,0xE2,0xD091,0x0411,
    0x82,0xC2,0xF7,0xD092,0x0412,
    0x83,0xC3,0xE7,0xD093,0x0413,
    0x84,0xC4,0xE4,0xD094,0x0414,
    0x85,0xC5,0xE5,0xD095,0x0415,
    0xF0,0xA8,0xB3,0xD081,0x0401,   // Ё
    0x86,0xC6,0xF6,0xD096,0x0416,
    0x87,0xC7,0xFA,0xD097,0x0417,
    0x88,0xC8,0xE9,0xD098,0x0418,
    0x89,0xC9,0xEA,0xD099,0x0419,
    0x8A,0xCA,0xEB,0xD09A,0x041A,
    0x8B,0xCB,0xEC,0xD09B,0x041B,
    0x8C,0xCC,0xED,0xD09C,0x041C,
    0x8D,0xCD,0xEE,0xD09D,0x041D,
    0x8E,0xCE,0xEF,0xD09E,0x041E,
    0x8F,0xCF,0xF0,0xD09F,0x041F,   // П

    0x90,0xD0,0xF2,0xD0A0,0x0420,   // Р
    0x91,0xD1,0xF3,0xD0A1,0x0421,
    0x92,0xD2,0xF4,0xD0A2,0x0422,
    0x93,0xD3,0xF5,0xD0A3,0x0423,
    0x94,0xD4,0xE6,0xD0A4,0x0424,   //Ф v0.13 - кодировка koi8 исправлено
    0x95,0xD5,0xE8,0xD0A5,0x0425,
    0x96,0xD6,0xE3,0xD0A6,0x0426,
    0x97,0xD7,0xFE,0xD0A7,0x0427,
    0x98,0xD8,0xFB,0xD0A8,0x0428,
    0x99,0xD9,0xFD,0xD0A9,0x0429,
    0x9A,0xDA,0xFF,0xD0AA,0x042A,
    0x9B,0xDB,0xF9,0xD0AB,0x042B,
    0x9C,0xDC,0xF8,0xD0AC,0x042C,
    0x9D,0xDD,0xFC,0xD0AD,0x042D,
    0x9E,0xDE,0xE0,0xD0AE,0x042E,
    0x9F,0xDF,0xF1,0xD0AF,0x042F,   // Я
    0,0,0,0,0};

/*================================================================= Функции ============================*/
//void compil(void);
void compil_l(void); // компиляция исходного текста (MKL)
//int win2dos(char* );
//int dos2win(char* );
void mkstrcpy (char* , char* , int , int );
//int file2text(void);
//int text2file(void);
int init_mkkom(void);   // мнемоники команд
int init_mkerror(void); // коды ошибок
int init_msg(void); // сообщения

void buf2txt(void); // из mkbufp в файл листинга TXT
void buf2htm(void); // из mkbufp в файл HTML

int comp_strupr(void); // аналог DOS strupr для корректной работы со строками программы в ctemp
int comp_get_chislo(char*, int); // считать число из строки
int comp_set_error(int, long, char*); // установить ошибку в структуре
int comp_get_metka(char* ); // считать метку из строки
int comp_get_ident(char*, int ); // считать идентификатор из строки
int comp_get_equ(char* , int ); // считать псевдооператор EQU из строки
int comp_set_metka(char*, long, int); // установить метку в структуре metka
int comp_set_ident(char*, long, int); // установить идентификатор в структуре ident
int comp_get_oper(char*, int); // считать оператор из строки и произвести требуемые действия с mkbufp
int comp_get_adres(char*, int, int); // считать адрес из строки (м.б. число или идентификатор)
int comp_set_adres(char*, int, int); // считать и записать число-адрес z байт
int comp_find_ident(char*); // возвращает номер записи с найденным значением
int comp_find_metka(char*); // возвращает номер записи с найденным значением
int comp_add_mkbufp(int);   // устанавливает число в буфере программы
                // по текущему адресу с должными проверками
int comp_set_base(char*, int, int); // добавление базы в структуру mbase (v0.23)
int comp_get_text(char*, int); // считывает строку в comp_text
int comp_set_text(void); // переносит строку из comp_text в mkbufp
int comp_get_num(char*, int); // считать число из строки, м.б. метка или адрес, записать его в mkbufp
int comp_get_num_hl(char*, int, int); // разбор NUMH NUML с v0.15
int comp_set_oper(char* ,int); // разбор одной мнемоники оператора с начала строки,
                // возврат - число параметров и т.д.
                // z-0 вернуть оператор если есть оканчивающийся в этом месте
int comp_pass2(void);   // второй проход - заменить метки в тексте программы re=0 OK re!=0 есть ошибки

int comp_get_db(char*, int, int); // считать ряд чисел, перечисленных через запятую из строки до конца строки
int comp_get_dbz(char*, int, int); // считать ряд из z чисел, перечисленных через запятую из строки (v0.27)
int comp_get_dbb(char*, int, int); // считать ряд чисел как однобайтовое смещение
			// относительно базы .BASE  (v0.23)
int comp_get_dbbz(char*, int, int); // считать ряд из z чисел как однобайтовое смещение
			// относительно базы .BASE (v0.27)

int comp_get_da(char*, int, int); // считать ряд чисел или меток 2 байта,
            //  перечисленных через запятую из строки до конца строки
            //  последний параметр - 0-все число, 1 - млчасть (%100), 2 - ст часть (/100)
int comp_get_da1(char*, int, int); // считать ряд чисел или меток 2 байта - 1 шт
		// и удалить следующую запятую (v0.27)

int comp_get_dd(char*, int, int); // считать ряд чисел или меток как DEC
int comp_get_dw(char*, int, int); // считать ряд чисел или меток как HEX (v0.25)

/*================================================================ ПРОГРАММА ================================*/
/*------------------------------------------- Вспомогательные ПП компиляции MKL -----*/

int comp_get_chislo (char* sss, int n)
{
int i,j,len;
int flag_chislo, flag_hex;
char c;
int re;

// Считывание числа long d DEC или HEX из строки sss с позиции n
i=n;
j=0;
len=strlen(sss);
//strupr(sss);
comp_chislo=-1;
flag_chislo=0;
flag_hex=0;

m0:  c=sss[i];
     if ((c==0)||(c==';')||(c=='\r')||(c=='\n'))
        {if (flag_chislo==0) {re=0; goto end;}
         else {re=i; goto m1;}
        }

     if ((c=='H')||(c=='h'))
        {
        flag_hex=1;
        re=i+1;
        goto m1;
        }

     if ((c==' ')||(c=='\t'))
        {
        if (flag_chislo!=0) {re=i; goto m1;}
            else {
                if (i<len) {i++; goto m0;}
                else {re=i; goto m1;}
                }
        }

     if ( ((c>='0')&&(c<='9')) || ((c>='A')&&(c<='F')) || ((c>='a')&&(c<='f')))
        {
        temp[j]=c;
        if (j<255) j++;
        temp[j]=0;
        flag_chislo=1;
        if (i<len) {i++; goto m0;}
            else {re=i; goto m1;}
        }
        else {re=i; goto m1;}

m1:  // разбор перенесенного в temp
    comp_chislo=0;
    if (j==0) goto end;

    //comp_chislo=0;
    for (i=0; i<j; i++)
        {
        if (flag_hex==0) comp_chislo*=10; else comp_chislo*=16;

        switch (temp[i])
            {
            case '0': break;
            case '1': comp_chislo+=1; break;
            case '2': comp_chislo+=2; break;
            case '3': comp_chislo+=3; break;
            case '4': comp_chislo+=4; break;
            case '5': comp_chislo+=5; break;
            case '6': comp_chislo+=6; break;
            case '7': comp_chislo+=7; break;
            case '8': comp_chislo+=8; break;
            case '9': comp_chislo+=9; break;

            case 'a':
            case 'A': if (flag_hex!=0) comp_chislo+=10;
                        else goto end;
                        break;
            case 'b':
            case 'B': if (flag_hex!=0) comp_chislo+=11;
                        else goto end;
                        break;
            case 'c':
            case 'C': if (flag_hex!=0) comp_chislo+=12;
                        else goto end;
                        break;
            case 'd':
            case 'D': if (flag_hex!=0) comp_chislo+=13;
                        else goto end;
                        break;
            case 'e':
            case 'E': if (flag_hex!=0) comp_chislo+=14;
                        else goto end;
                        break;
            case 'f':
            case 'F': if (flag_hex!=0) comp_chislo+=15;
                        else goto end;
                        break;
            default: goto end;
            }
        }


end: return(re);
}

/*----------------- установить ошибку ---------------*/
int comp_set_error(int n, long str, char* sss)
{
int re=0;
long lv;

if (mkerror.init==0) init_mkerror();

lv=mkerror.vsego;
if (lv<1020)
    {
    mkerror.n[lv]=n;
    mkerror.stroka[lv]=str;
    if ((sss!=NULL)&&(strlen(sss)<38)) strcpy(mkerror.sss[lv],sss);
        else mkerror.sss[lv][0]=0;
    mkerror.vsego=lv+1;
    }
else
    {
    lv=1020;
    mkerror.n[lv]=1;
    mkerror.stroka[lv]=0;
    mkerror.sss[lv][0]=0;
    mkerror.vsego=lv;
    }

return(re);
}

/*----------------------------- считать метку из строки */
int comp_get_metka(char* sss)
{
int re=0;
/* указателя смещения в строке нет, ибо подразумевается,
что метка перехода всегда начинается с начала строки
Возврат re=символ, следующий за последним разобранным (в данном случае ':')
re<=0 - ошибка
метка переносится в comp_metka
*/
int i,j,k;
int err;
char c;


i=0;
j=0;
k=strlen(sss);
comp_metka[0]=0;
err=0;

m1:

c=sss[i];

if ((c==' ')&&(j==0)) goto m2;  // игнорировать начальные пробелы


if ( (c==0) || ((c==' ')&&(j!=0)) || ((c==':')&&(j==0)) ||(c=='\r')||(c=='\n'))
    {
    // недопустимое имя метки - метка пустая или непонятная ошибка разбора
    err=1;
    re=i;
    goto end;
    }

if ( ((c>='0')&&(c<='9')&&(j==0)) )
    {
    // недопустимое имя метки начинается с цифры
    err=1;
    }

if (c==':')
    {
    // конец метки re-указатель на символ за двоеточием
    re=i+1;
    goto end;
    }

if ( (c<'0') || ((c>'9')&&(c<'A')) || ((c>'Z')&&(c!='_')) )
    {
    // недопустимое имя не те символы
    err=1;
    }


if (j<32)
    {
    comp_metka[j]=c;
    j++;
    comp_metka[j]=0;
    }

m2:
i++;
if (i<k) goto m1;
re=i;

end:
    if (err!=0)
        {
        // ошибка - недопустимое имя метки
        comp_set_error(13, comp_stroka, comp_metka);
        re=-re;
        }

return(re);
}

/*------------------------------ установить метку в структуре */
int comp_set_metka(char* sss, long val, int use)
{
int re=0;
int i;
long lv;

lv=metka.vsego;

// v0.13 - поиск совпадения
i=comp_find_ident(sss);
if (i>=0)
    {
    comp_set_error(15,comp_stroka,sss); // ошибка - совпадение имени
    goto end;
    }

i=comp_find_metka(sss);
if (i>=0)
    {
    comp_set_error(13,comp_stroka,sss); // неправильное имя метки
    goto end;
    }
// end of add v0.13


if (lv<9999)
    {
    strcpy(metka.name[lv],sss);
    metka.val[lv]=val;
    metka.stroka[lv]=comp_stroka;
    if ((val>=0)&&(val<10000)&&(use==1)) metka.use[lv]=1; // в поле val - значение метки м.б. ее адрес
    if ((val>=0)&&(val<10000)&&(use==2)) metka.use[lv]=2; // в поле val - адрес установки метки (заменить значением)
    if (use==0) metka.use[lv]=0;
    metka.vsego=lv+1;
    }
else
    {
    comp_set_error(12,comp_stroka,""); // слишком много меток
    }

end: return(re);
}

/*------------------------------ установить идентификатор в структуре */
int comp_set_ident(char* sss, long val, int use)
{
int re=0;
int i;
long lv;

lv=ident.vsego;

// v0.13 - поиск совпадения
i=comp_find_metka(sss);
if (i>=0)
    {
    comp_set_error(15,comp_stroka,sss); // ошибка - совпадение имени
    goto end;
    }

i=comp_find_ident(sss);
if (i>=0)
    {
    comp_set_error(4,comp_stroka,sss); // недопустимое имя идентификатора
    goto end;
    }
// end of add v0.13


if (lv<9999)
    {
    strcpy(ident.name[lv],sss);
    ident.val[lv]=val;
    ident.stroka[lv]=comp_stroka;
    if ((val>=0)&&(val<10000)&&(use==1)) ident.use[lv]=1; // в поле val - значение идентификатора по EQU
    if ((val>=0)&&(val<10000)&&(use==2)) ident.use[lv]=2; // в поле val - адрес установки идентификатора (заменить значением)
    if (use==0) ident.use[lv]=0;
    ident.vsego=lv+1;
    }
else
    {
    comp_set_error(3,comp_stroka,""); // слишком много идентификаторов
    }

end: return(re);
}

/*-------------------------------------------- FIND IDENT */
int comp_find_ident(char* sss)
{
int re=-1;
int i,j;

if (ident.vsego==0) goto end;
if (strlen(sss)>32) sss[32]=0;

for (i=0; i<ident.vsego; i++)
    {
    j=strcmp(ident.name[i],sss);
    if ((j==0)&&((ident.use[i]&1)!=0))
        {
        re=i;
        goto end;
        }
    }

end: return(re);
}

/*-------------------------------------------- FIND METKA */
int comp_find_metka(char* sss)
{
int re=-1;
int i,j;

if (metka.vsego==0) goto end;
if (strlen(sss)>32) sss[32]=0;


for (i=0; i<metka.vsego; i++)
    {
    j=strcmp(metka.name[i],sss);
    if ((j==0)&&((metka.use[i]&1)!=0))
        {
        re=i;
        goto end;
        }
    }

end: return(re);
}
/*----------------------------- считать идентификатор из строки */
int comp_get_ident(char* sss, int n)
{
int re=0;
/*
Возврат re=символ, следующий за последним разобранным
re<=0 - ошибка!!!

идентификатор переносится в comp_ident
*/
int err;
int i,j,k;
char c;


i=n;
j=0;
k=strlen(sss);
comp_ident[0]=0;
err=0;

m1:

c=sss[i];

if ((c==' ')&&(j==0)) goto m2; // пропустить пробелы в начале

if ( ((c>='0')&&(c<='9')&&(j==0)) )
    {
    // недопустимое имя идентификатора начинается с цифры
    err=1;
    re=0;
    }


if ((c==0)||(c==' ')||(c==',')||(c==';')||(c=='\r')||(c=='\n')) // добавлена запятая для разбора DB и DA
    {
    // конец идентификатора re-указатель на символ
    re=i;
    goto end;
    }

if ( (c<'0') || ((c>'9')&&(c<'A')) || ((c>'Z')&&(c!='_')) )
    {
    // недопустимое имя не те символы
    err=1;
    re=0;
    }


if (j<32)
    {
    comp_ident[j]=c;
    j++;
    comp_ident[j]=0;
    }

m2:
i++;
if (i<k) goto m1;
re=i;

end:
    if (err!=0)
        {
        comp_set_error(4, comp_stroka, comp_ident); // недопустимое имя идентификатора
        re=0;
        }
return(re);
}


/*----------------------------- считать адрес из строки */
int comp_get_adres(char* sss, int n, int z)
{
int re=0;
/*
Возврат re=символ, следующий за последним разобранным
re<=0 (т.е. re<n) - ошибка!!!
z - параметр
    =1 - только число
    =2 - число или идент
    =3 - число, идент или метка (метка м.б. не определена, тогда установить адрес)

*/
int i,i1,j,k;
char c;


i=n;
j=0;
k=strlen(sss);
comp_chislo=-1;

m1:

c=sss[i];

if (c==' ') {i++; goto m1;}

if ((c==0)||(c==';'))
    {
    // адреса нет re-указатель на символ за
    comp_set_error(6, comp_stroka,"");  // недопустимый адрес
    re=i;
    goto end;
    }



if  ((c>='0')&&(c<='9')&&(z>0))
    {
    i1=comp_get_chislo(sss,i);
    if ((comp_chislo>=0)&&(comp_chislo<=9999)&&(i1>=i))
        {
        re=i1;
        goto end;
        }
    else
        {
        sprintf(ctemptoa,"%d",comp_chislo);
        comp_set_error(6, comp_stroka, ctemptoa);   // недопустимый адрес
        re=i;
        goto end;
        }
    }

if (z<=1)
    {
    comp_set_error(6, comp_stroka,""); // недопустимый адрес
    re=i;
    goto end;
    }

if ( ((c>='A')&&(c<='Z')) || (c=='_'))
    {
    i1=comp_get_ident(sss,i);
    if (i1>i)
        { // имя идентификатора или метки существует
        j=comp_find_ident(comp_ident);
        if ((j>=0)&&(j<ident.vsego))
            {
            if ((ident.val[j]>=0) && (ident.val[j]<10000) && ((ident.use[j] & 1)!=0))
                {
                ident.use[j]|=2;    // идентификатор использован в программе
                comp_chislo=ident.val[j]; // переместить адрес в comp_chislo
                re=i1;
                goto end;
                }
            else
                {
                sprintf(ctemptoa,"%d",ident.val[j]);
                comp_set_error(5, comp_stroka, ctemptoa);   // недопустимое значение идентификатора
                re=i1;
                goto end;
                }
            }

        if (z<=2)   // метку не искать
            {
            comp_set_error(6, comp_stroka,""); // недопустимый адрес
            re=i;
            goto end;
            }

        j=comp_find_metka(comp_ident);
        if ((j>=0)&&(j<metka.vsego))
            {
            if ((metka.val[j]>=0) && (metka.val[j]<10000) && ((metka.use[j] & 1)!=0))
                {
                metka.use[j]|=2;    // метка использована в программе
                comp_chislo=metka.val[j]; // переместить адрес в comp_chislo
                re=i1;
                goto end;

                }
            else
                {
                sprintf(ctemptoa,"%d",metka.val[j]);
                comp_set_error(14, comp_stroka, ctemptoa);  // недопустимое значение метки
                re=i1;
                goto end;
                }
            }
        else
            {
            comp_set_metka(comp_ident,comp_adres,2);    // метка используется но не найдена
            }

        re=i1;
        }
    else
        {
        comp_set_error(4, comp_stroka,"");  // недопустимое имя идентификатора
        re=i;
        }

    goto end;
    }


end: return(re);
}
/*--------------------------------ADD_MKBUFP ----------------------- */
int comp_add_mkbufp(int x)
{
int re=0;

if ((comp_adres>=0)&&(comp_adres<10000))
        {mkbufp[comp_adres]=x;
        comp_adres++;
        }
    else
        {
        sprintf(ctemptoa,"%d",comp_adres);
        comp_set_error(6,comp_stroka,ctemptoa);
        }

return(re);
}

/*-------------------------------- SET_BASE (v0.23) ----------------------- */
int comp_set_base(char* sss, int n, int z)
{
int re=0;
int i,j;
long l;

if ((comp_adres>=0)&&(comp_adres<10000))
        {
        comp_base=comp_adres;

        if (mbase.vsego<=0) goto m1;    // пока нет баз
        // Поиск по структуре, если адрес уже есть - не добавлять
        j=0;
        for (i=0; i<mbase.vsego; i++)
            {
            if (mbase.val[i]==comp_adres) goto end;
            }

m1:     if (mbase.vsego>10000) mbase.vsego=10000;
        mbase.val[mbase.vsego]=comp_adres;
        mbase.stroka[mbase.vsego]=comp_stroka;
        mbase.vsego++;

        if (mbase.vsego<=1) goto end;
        // Упорядочить таблицу баз
        for (i=1; i<mbase.vsego; i++)
            {
            for (j=0; j<i; j++)
                {
                // без исключения повторяющихся элементов - их нет
                if (mbase.val[j]>mbase.val[i])
                    {
                    l=mbase.val[j];
                    mbase.val[j]=mbase.val[i];
                    mbase.val[i]=l;
                    l=mbase.stroka[j];
                    mbase.stroka[j]=mbase.stroka[i];
                    mbase.stroka[i]=l;
                    }
                }
            }
        }
    else
        {sprintf(ctemptoa,"%d",comp_adres);
         comp_set_error(6,comp_stroka,ctemptoa);
        }

end:
re=n+5;
return(re);
}

/*------------------------------- COMP ADRES ------------------*/
int comp_set_adres(char* sss, int n, int z)
{
// считать из строки и записать в программу адрес z 1 или 2 байт
// z==3 или 4 - соответственно номер регистра памяти 1 или 2 байт
// z==5 или 6 - вставить адрес из comp_chislo - один или два байта
// (вставить с учетом comp_adres)
// v0.16
// z==7 - вставить адрес из comp_chislo - один байт без проверки comp_adres

// v0.18
// z==8 - вставить адрес из comp_chislo - один байт без проверки comp_adres (DEC)
// z==9 - вставить адрес из comp_chislo - два байта без проверки comp_adres (DEC)
// v0.25
// z==10 - вставить число из comp_chislo - два байта без проверки (HEX)

int i,j,k,pa,pch;
int re=0;


if ((z>=1)&&(z<=4)) comp_chislo=-1;

  if ((z==1)||(z==2))
    {
    i=comp_get_adres(sss,n,3);

    if (comp_chislo<0)
        {
        // значение метки не обнаружено
        // оставить на второй проход, а пока вставить временное значение в mkbufp,
        // в структуру metka метка уже д.б. вставлена с use=2
        if (z==1)
            {
            comp_add_mkbufp(comp_const+513); // metka z=1
            }
        if (z==2)
            {
            comp_add_mkbufp(comp_const+514); // метка 2 байта
            comp_add_mkbufp(comp_const+512); // байт не используется
            }
        re=i;
        goto end;
        }
    }


  if ((z==3)||(z==4))
        {
        i=comp_get_adres(sss,n,2); // только число или идентификатор
        if (comp_chislo<0)
            {
            comp_add_mkbufp(comp_const+515); // неверное значение
            if (z==4) comp_add_mkbufp(comp_const+512); // не используется
            comp_set_error(7,comp_stroka,""); // значение идентификатора не определено
            re=i;
            goto end;
            }
        }

  if ((z==5)||(z==6)||(z==7)||(z==8)||(z==9)||(z==10)) // z==7 - v0.16 z==8,9 - v0.18 z==10 v0.25
        {
        // подставить фиктивное значение i
        i=n+1;
        if (comp_chislo<0)
            {
            // неверное значение
            comp_set_error(7,comp_stroka,""); // значение идентификатора не определено
            re=i;
            goto end;
            }
        }


  if ((z==1)||(z==5))
    {
    // адрес 1 байт
    if (comp_flag_adres==0) j=100; else j=166;

    if ((z==1)&&(i<=n))
        {
        comp_add_mkbufp(comp_const+515);
        comp_set_error(8,comp_stroka,""); // неверное значение
        re=n;
        goto end;
        }

    pa=comp_adres/100;
    pch=comp_chislo/100;

    if (pa==pch)
        {
        // вставить на той же странице
        k=comp_chislo%100;
        k=16*(k/10)+(k%10); // преобразовать в BCD
        comp_add_mkbufp(k);
        re=i;
        goto end;
        }

    k=comp_chislo-100*pa; // расстояние от начала текущей страницы

    if ((pa==pch-1) && (k>=100)&&(k<j))
        {
        // вставить короткий переход по шестнадцатиричному адресу, если расстояние позволит
        if ((k>=100)&&(k<160)) k=16*(k/10)+(k%10); // преобразовать в BCD
        if ((k>=160)&&(k<166)) k+=90; // k=240+(k-150);
        comp_add_mkbufp(k);
        re=i;
        goto end;
        }

    comp_add_mkbufp(comp_const+515);
    comp_set_error(6,comp_stroka,""); // неверный адрес
    re=i;
    goto end;
    } // end z=1

  if (z==7)
    {   //v0.16
    k=comp_chislo%100;
    k=16*(k/10)+(k%10); // преобразовать в BCD
    comp_add_mkbufp(k);
    re=i;
    goto end;
    }

  if (z==8)
    {   //v0.18
    k=comp_chislo%256;
    // не преобразовывать в BCD
    comp_add_mkbufp(k+256);
    re=i;
    goto end;
    }

  if (z==9)
    {   //v0.18
    k=comp_chislo/100;
    // не преобразовывать в BCD
    comp_add_mkbufp(k+256);
    k=comp_chislo%100;
    // не преобразовывать в BCD
    comp_add_mkbufp(k+256);
    re=i;
    goto end;
    }

 if (z==10)
    {   //v0.25
    k=comp_chislo/256;  // HEX ст.
    comp_add_mkbufp(k+256);
    k=comp_chislo%256;  // HEX мл.
    comp_add_mkbufp(k+256);
    re=i;
    goto end;
    }

  if (z==3)
    {
    // номер регистра памяти
    if (comp_flag_adres==0) j=100; else j=166;

    if ((i<=n)||(comp_chislo<0)||(comp_chislo>=j))
        {
        comp_add_mkbufp(comp_const+515);
        comp_set_error(8,comp_stroka,""); // неверное значение
        re=n;
        goto end;
        }

    k=comp_chislo;
        if ((k>=0)&&(k<160)) k=16*(k/10)+(k%10); // преобразовать в BCD
        if ((k>=160)&&(k<166)) k+=90; // k=240+(k-150);
    comp_add_mkbufp(k);
    re=i;
    goto end;
    } // end z==3

  if ((z==2)||(z==4)||(z==6))
    {
    // адрес или номер регистра всегда только десятичный
    // при z==6 i=n+1
    if ((i>n)&&(comp_chislo>=0)&&(comp_chislo<10000))
        {
        k=comp_chislo/100;
        k=16*(k/10)+(k%10); // преобразовать в BCD
        comp_add_mkbufp(k);
        k=comp_chislo%100;
        k=16*(k/10)+(k%10); // преобразовать в BCD
        comp_add_mkbufp(k);
        re=i;
        goto end;
        }
    else
        {
        if (z!=6)
            {comp_add_mkbufp(comp_const+515);
             comp_add_mkbufp(comp_const+512);
            }
        sprintf(ctemptoa,"%d",comp_chislo);
        if (z==2) comp_set_error(8,comp_stroka, ctemptoa); // неверное значение
            else  comp_set_error(6,comp_stroka, ctemptoa); // неверный адрес
        re=n;
        goto end;
        }
    } // end z==2



end: return(re);
}

/*------------------------- GET DB --------------------*/
int comp_get_db(char* sss, int n, int z)
{
int i,i1,j,k;
char c;         // v0.23 (v1.18 mk.exe) для sss[]
int m;          // v0.23 (v1.18 mk.exe) флаг наличия символа

int re=0;
// считать из строки и записать в программу ряд чисел или идентификаторов
// через запятую отсюда и до конца строки
// z = по 1 или 2 байт - не используется см. comp_get_da

i=n;
k=strlen(sss);


m1:
        j=comp_get_adres(sss,i,3);
        if ((j>i)&&(comp_chislo>=0)&&(comp_chislo<256))
                {
                if ((comp_adres>=0)&&(comp_adres<10000))
                        {
                        comp_add_mkbufp(comp_chislo+256);   // v +256 - HEX

                        }
                else {  sprintf(ctemptoa,"%d",comp_adres);
                        comp_set_error(6,comp_stroka,ctemptoa);
                     }
                }
        else
                {
                sprintf(ctemptoa,"%d",comp_chislo);
                comp_set_error(8,comp_stroka,ctemptoa); // неверное значение
                }


    if ((j<=0)||(j<=n))
        {
        re=j;       // ошибка или конец
        goto end;
        }

    // v0.23 (v1.18 mk.exe) - проверка наличия чего-либо до конца строки или комментария
    m=0;
    for (i1=j; i1<k; i1++)
        {
        c=sss[i1];
        if ((c==';')||(c=='\r')||(c=='\n')||(c==0)) goto m2;
        if ((c!=' ')&&(c!=',')&&(c!='\t')) m=1;
        }
m2: if (m==0) goto end; // нет в конце ничего

    for (i1=j; i1<k; i1++)
        {
        if (sss[i1]==',')
            {
            i=i1+1;
            goto m1;    // повтор разбора числа
            }
        if ((sss[i1]==';')||(sss[i1]=='\r')||(sss[i1]=='\n')||(sss[i1]==0))
            {
            re=0;
            goto end;
            }
        }

// затычка
re=0;
end:
return(re);
}

/*------------------------- GET DBZ (v0.27) --------------------*/

int comp_get_dbz(char* sss, int n, int z)
{
int i,i1,j,k;
char c;         // v0.23 (v1.18 mk.exe) для sss[]
int m;          // v0.23 (v1.18 mk.exe) флаг наличия символа

int re=0;
// считать из строки и записать в программу один байт - одно число или идентификатор
// для операторов ГИ
// z - количество считываемых байт

i=n;
k=strlen(sss);


m1:
        j=comp_get_adres(sss,i,3);
        if ((j>i)&&(comp_chislo>=0)&&(comp_chislo<256))
                {
                if ((comp_adres>=0)&&(comp_adres<10000))
                        {
                        comp_add_mkbufp(comp_chislo+256);   // v +256 - HEX

                        }
                else {  sprintf(ctemptoa,"%d",comp_adres);
                        comp_set_error(6,comp_stroka,ctemptoa);
                     }
                }
        else
                {
                sprintf(ctemptoa,"%d",comp_chislo);
                comp_set_error(8,comp_stroka,ctemptoa); // неверное значение
                }
	z--;

   if ((j<=0)||(j<=n))
        {
        re=j;       // ошибка или конец
        goto end;
        }

    // проверка наличия чего-либо до конца строки или комментария
    m=0;
    for (i1=j; i1<k; i1++)
        {
        c=sss[i1];
        if ((c==';')||(c=='\r')||(c=='\n')||(c==0)) goto m2;
        if ((c!=' ')&&(c!=',')&&(c!='\t')) m=1;
        }
m2: if (m==0)
	{
	if (z>0)
		{   sprintf(ctemptoa,"%d",comp_adres);
                    comp_set_error(10,comp_stroka,ctemptoa);	// нет требуемого количества параметров
		}

	goto end; // нет в конце ничего
	}

    for (i1=j; i1<k; i1++)
        {
        if (sss[i1]==',')
            {
            i=i1+1;
	    if (z<=0) {re=i; goto end;}	// Z чисел данного типа считаны
            goto m1;    // повтор разбора числа - для следующего параметра
            }
        if ((sss[i1]==';')||(sss[i1]=='\r')||(sss[i1]=='\n')||(sss[i1]==0))
            {
	    if (z>0)
		{   sprintf(ctemptoa,"%d",comp_adres);
                    comp_set_error(10,comp_stroka,ctemptoa);	// нет требуемого количества параметров
		}

            re=0;
            goto end;
            }
        }


// затычка
re=0;
end:
return(re);
}



/*------------------------- GET DBB v0.23 (v1.18 mk.exe) --------------------*/
int comp_get_dbb(char* sss, int n, int z)
{
int i,i1,j,k;
int i2;
char c;
int m;
int re=0;
// считать из строки и записать в программу ряд чисел или идентификаторов
// через запятую отсюда и до конца строки
// из числа вычесть базу установленную по .BASE
// z - не используется
i=n;
k=strlen(sss);

m1:
        j=comp_get_adres(sss,i,3);  // считать число, метку или т.п.
        if ((j>i)&&(comp_chislo>=0)&&(comp_chislo<10000))
                {
                if ((comp_adres>=0)&&(comp_adres<10000))
                    {
                    i2=comp_chislo-comp_base;   // вычитание базы
                    if ((i2>=0)&&(i2<256)) comp_add_mkbufp(i2+256); // HEX
                    else
                        {
                        sprintf(ctemptoa,"%d",comp_adres);
                        comp_set_error(8,comp_stroka,ctemptoa);
                        }
                    }
                else {
                    sprintf(ctemptoa,"%d",comp_adres);
                    comp_set_error(6,comp_stroka,ctemptoa);
                    }

                }
        else
                {
                if (j>i)
                    {
                        // метка уже в структуре после comp_get_adres
                    comp_add_mkbufp(comp_const+526);    // 526 метка с базой
                    }
                else
                    {sprintf(ctemptoa,"%d",comp_chislo);
                    comp_set_error(8,comp_stroka,ctemptoa); // неверное значение
                    }
                }


    if ((j<=0)||(j<=n))
        {
        re=j;       // ошибка или конец
        goto end;
        }

    m=0;
    for (i1=j; i1<k; i1++)
        {
        c=sss[i1];
        if ((c==';')||(c=='\r')||(c=='\n')||(c==0)) goto m2;
        if ((c!=' ')&&(c!=',')&&(c!='\t')) m=1;
        }
m2: if (m==0) goto end; // нет в конце ничего

    for (i1=j; i1<k; i1++)
        {
        if (sss[i1]==',')
            {
            i=i1+1;
            goto m1;    // повтор разбора числа
            }
        if ((sss[i1]==';')||(sss[i1]=='\r')||(sss[i1]=='\n')||(sss[i1]==0))
            {
            re=0;
            goto end;
            }
        }


re=0;
end:
return(re);

}

/*------------------------- GET DBBZ v0.27 --------------------*/
int comp_get_dbbz(char* sss, int n, int z)
{
int i,i1,j,k;
int i2;
char c;
int m;
int re=0;
// считать из строки и записать в программу ряд чисел или идентификаторов
// через запятую отсюда и до z чисел или конца строки
// из числа вычесть базу установленную по .BASE
// z - количество считываемых чисел
// для операторов ГИ
i=n;
k=strlen(sss);

m1:
        j=comp_get_adres(sss,i,3);  // считать число, метку или т.п.
	z--;
        if ((j>i)&&(comp_chislo>=0)&&(comp_chislo<10000))
                {
                if ((comp_adres>=0)&&(comp_adres<10000))
                    {
                    i2=comp_chislo-comp_base;   // вычитание базы
                    if ((i2>=0)&&(i2<256)) comp_add_mkbufp(i2+256); // HEX
                    else
                        {
                        sprintf(ctemptoa,"%d",comp_adres);
                        comp_set_error(8,comp_stroka,ctemptoa);
                        }
                    }
                else {
                    sprintf(ctemptoa,"%d",comp_adres);
                    comp_set_error(6,comp_stroka,ctemptoa);
                    }

                }
        else
                {
                if (j>i)
                    {
                        // метка уже в структуре после comp_get_adres
                    comp_add_mkbufp(comp_const+526);    // 526 метка с базой
                    }
                else
                    {sprintf(ctemptoa,"%d",comp_chislo);
                    comp_set_error(8,comp_stroka,ctemptoa); // неверное значение
                    }
                }


    if ((j<=0)||(j<=n))
        {
        re=j;       // ошибка или конец
        goto end;
        }

    m=0;
    for (i1=j; i1<k; i1++)
        {
        c=sss[i1];
        if ((c==';')||(c=='\r')||(c=='\n')||(c==0)) goto m2;
        if ((c!=' ')&&(c!=',')&&(c!='\t')) m=1;
        }
m2: if (m==0)
	{
	// если z!=0 - ошибка
	if (z>0)
		{   sprintf(ctemptoa,"%d",comp_adres);
                    comp_set_error(10,comp_stroka,ctemptoa);	// нет требуемого количества параметров
		}

	goto end; // нет в конце ничего
	}

    for (i1=j; i1<k; i1++)
        {
        if (sss[i1]==',')
            {
            i=i1+1;
	    if (z<=0) {re=i; goto end;}
            goto m1;    // повтор разбора числа
            }
        if ((sss[i1]==';')||(sss[i1]=='\r')||(sss[i1]=='\n')||(sss[i1]==0))
            {
	if (z>0)
		{   sprintf(ctemptoa,"%d",comp_adres);
                    comp_set_error(10,comp_stroka,ctemptoa);	// нет требуемого количества параметров
		}

            re=0;
            goto end;
            }
        }


re=0;
end:
return(re);

}


/*------------------------- GET DА --------------------*/
int comp_get_da(char* sss, int n, int z)
{

int i,i1,j,k;
int i2; // v0.18
char c; // v0.23 (v1.18 mk.exe)
int m;  // v0.23 (v1.18 mk.exe)

int re=0;
// считать из строки и записать в программу ряд чисел, меток или идентификаторов
// через запятую отсюда и до конца строки и записать в программу как адреса
// z = 0 - 2 байта, =1 - мл часть =2 - ст. часть числа

i=n;
k=strlen(sss);


m1:
        j=comp_get_adres(sss,i,3);  // считать число, метку или т.п.
        if ((j>i)&&(comp_chislo>=0)&&(comp_chislo<10000))
                {
                if ((z==0)||(z==2))
                    {
                    if ((comp_adres>=0)&&(comp_adres<10000))
                        {
                        i2=comp_chislo/100;
                        i2=(16*(i2/10)+(i2%10))%256;        // v0.18 - BCD
                        //comp_add_mkbufp(comp_chislo/100+256); // ст. часть адреса +256=HEX v0.15
                        comp_add_mkbufp(i2+256);        //v0.18
                        }
                    else {
                        sprintf(ctemptoa,"%d",comp_adres);
                        comp_set_error(6,comp_stroka,ctemptoa);
                         }
                    }


                if ((z==0)||(z==1))
                    {
                    if ((comp_adres>=0)&&(comp_adres<10000))
                        {
                        i2=comp_chislo%100;
                        i2=(16*(i2/10)+(i2%10))%256;        // v0.18 - BCD
                        //comp_add_mkbufp(comp_chislo%100+256); // мл. часть адреса +256=HEX v0.15
                        comp_add_mkbufp(i2+256);        //v0.18
                        }
                    else {  sprintf(ctemptoa,"%d",comp_adres);
                        comp_set_error(6,comp_stroka,ctemptoa);
                         }
                    }


                }
        else
                {
                // v 0.15 - добавить разбор метки не определенной к моменту использования
                if ((j>i)&&(z>=0)&&(z<=2))
                    {
                        // метка уже в структуре после comp_get_adres
                    if (z==0)
                        {
                        comp_add_mkbufp(comp_const+514);
                        comp_add_mkbufp(comp_const+512);
                        }
                    if (z==1)
                        {
                        //comp_add_mkbufp(513); до v0.18
                        comp_add_mkbufp(comp_const+524);    //v0.18 - 1 байт BCD
                        }

                    if (z==2)
                        {
                        comp_add_mkbufp(comp_const+520);    // метка - ст часть
                        }

                    }
                else
                    {sprintf(ctemptoa,"%d",comp_chislo);
                    comp_set_error(8,comp_stroka,ctemptoa); // неверное значение
                    }

                }


    if ((j<=0)||(j<=n))
        {
        re=j;       // ошибка или конец
        goto end;
        }

    // v0.23 (v1.18 mk.exe) - проверка наличия чего-либо до конца строки или комментария
    m=0;
    for (i1=j; i1<k; i1++)
        {
        c=sss[i1];
        if ((c==';')||(c=='\r')||(c=='\n')||(c==0)) goto m2;
        if ((c!=' ')&&(c!=',')&&(c!='\t')) m=1;
        }
m2: if (m==0) goto end; // нет в конце ничего

    for (i1=j; i1<k; i1++)
        {
        if (sss[i1]==',')
            {
            i=i1+1;
            goto m1;    // повтор разбора числа
            }
        if ((sss[i1]==';')||(sss[i1]=='\r')||(sss[i1]=='\n')||(sss[i1]==0))
            {
            re=0;
            goto end;
            }
        }


re=0;
end:
return(re);

}
/*------------------------- GET DА1 (v0.27) --------------------*/
int comp_get_da1(char* sss, int n, int z)
{

int i,i1,j,k;
int i2; // v0.18
char c; // v0.23 (v1.18 mk.exe)
int m;  // v0.23 (v1.18 mk.exe)

int re=0;
// считать из строки и записать в программу одно чисел, меток или идентификаторов
// до запятой и записать в программу как адреса
// для операторов ГИ (v0.27)
// z = 0 - 2 байта, =1 - мл часть =2 - ст. часть числа

i=n;
k=strlen(sss);


m1:
        j=comp_get_adres(sss,i,3);  // считать число, метку или т.п.
        if ((j>i)&&(comp_chislo>=0)&&(comp_chislo<10000))
                {
                if ((z==0)||(z==2))
                    {
                    if ((comp_adres>=0)&&(comp_adres<10000))
                        {
                        i2=comp_chislo/100;
                        i2=(16*(i2/10)+(i2%10))%256;        // v0.18 - BCD
                        comp_add_mkbufp(i2+256);
                        }
                    else {
                        sprintf(ctemptoa,"%d",comp_adres);
                        comp_set_error(6,comp_stroka,ctemptoa);
                         }
                    }


                if ((z==0)||(z==1))
                    {
                    if ((comp_adres>=0)&&(comp_adres<10000))
                        {
                        i2=comp_chislo%100;
                        i2=(16*(i2/10)+(i2%10))%256;        // v0.18 - BCD
                        comp_add_mkbufp(i2+256);
                        }
                    else {  sprintf(ctemptoa,"%d",comp_adres);
                        comp_set_error(6,comp_stroka,ctemptoa);
                         }
                    }


                }
        else
                {
                // v 0.15 - добавить разбор метки не определенной к моменту использования
                if ((j>i)&&(z>=0)&&(z<=2))
                    {
                        // метка уже в структуре после comp_get_adres
                    if (z==0)
                        {
                        comp_add_mkbufp(comp_const+514);
                        comp_add_mkbufp(comp_const+512);
                        }
                    if (z==1)
                        {
                        comp_add_mkbufp(comp_const+524);    //v0.18 - 1 байт BCD
                        }

                    if (z==2)
                        {
                        comp_add_mkbufp(comp_const+520);    // метка - ст часть
                        }

                    }
                else
                    {sprintf(ctemptoa,"%d",comp_chislo);
                    comp_set_error(8,comp_stroka,ctemptoa); // неверное значение
                    }

                }


    if ((j<=0)||(j<=n))
        {
        re=j;       // ошибка или конец
        goto end;
        }

    // v0.23 (v1.18 mk.exe) - проверка наличия чего-либо до конца строки или комментария
    m=0;
    for (i1=j; i1<k; i1++)
        {
        c=sss[i1];
        if ((c==';')||(c=='\r')||(c=='\n')||(c==0)) goto m2;
        if ((c!=' ')&&(c!=',')&&(c!='\t')) m=1;
        }
m2: if (m==0) goto end; // нет в конце ничего

    for (i1=j; i1<k; i1++)
        {
        if (sss[i1]==',')
            {
            i=i1+1;
		// Для DA1 (v0.27)
	    re=i;
            goto end;    // выход после первого числа после запятой
            }
        if ((sss[i1]==';')||(sss[i1]=='\r')||(sss[i1]=='\n')||(sss[i1]==0))
            {
            re=0;
            goto end;
            }
        }


re=0;
end:
return(re);

}



/*----------------------------- считать псевдооператор .EQU из строки */
int comp_get_equ(char* sss, int n)
{
int re=0;
/*
Возврат re=символ, следующий за последним разобранным re<=0 - ошибка
происходит только чтение псевдооператора EQU
поскольку он разбирается отдельно особым образом
*/
int i,k;
char c;


i=n;
k=strlen(sss);

m1:

c=sss[i];

if (c=='.')
    {
    if (i+3<k)
        {
        if ((sss[i+1]=='E')&&(sss[i+2]=='Q')&&(sss[i+3]=='U'))
            {
            re=i+4;
            goto end;
            }
        }

    }


if ((c==0)||(c==';'))
    {
    // конец строки а EQU нет
    re=0;
    goto end;
    }

i++;
if (i<k) goto m1;

end: return(re);
}

/*----------------------------- считать "TEXT" из строки */
int comp_get_text(char* sss, int n)
{
int re=0;
int i,j,k,ic;
int flag_text, flag_slash;
char c;


i=n;
k=strlen(sss);
flag_text=0;
flag_slash=0;
j=0;
comp_text[0]=0;

m1:

c=sss[i];
ic=(int)c;
if (ic<0) ic+=256;
if ((ic<0)||(ic>255)) ic=0;


if (c=='"')
    {
    if (flag_text==0) {flag_text=1; goto next_i;} // начало текста
    if ((flag_text!=0)&&(flag_slash==0)) {re=i+1; goto end;} // конец текста
    if ((flag_text!=0)&&(flag_slash!=0))
        { // кавычки внутри текста
        flag_slash=0;
        comp_text[j]='"';
        goto next_j; // код символа '"'
        }
    }

if ((c==0)||(c=='\r')||(c=='\n')|| ((flag_text==0)&&(c==';')) )
    {
    comp_text[0]=0; // удалить все
    comp_set_error(mkerror.w+2,comp_stroka,""); // текст отсутствует
    re=i;
    goto end;   // обрыв строки
    }

if (flag_text==0) goto next_i;

if ((ic==0x5C)&&(flag_text!=0))
    {
    if (flag_slash==0)
        {
        flag_slash=1;
        goto next_i;
        }
    else
        {
        flag_slash=0;
        comp_text[j]=(char)0x5C; // код символа '\'
        }

    }


if (ic<=32)
    { // служебные символы
    comp_text[j]=' ';
    goto next_j;
    }

if (flag_slash!=0)
    { // возврат каретки, перевод строки и табуляция
    if (c=='r') comp_text[j]=13;
    if (c=='n') comp_text[j]=10;
    if (c=='t') comp_text[j]=9;
    if (c=='0') comp_text[j]=3; // символ 3 ETX заменяет 0 при разборе
    flag_slash=0;
    goto next_j;
    }

if ((ic>32)&&(ic<128))
    { // ASCII
    comp_text[j]=c; // перекодировка не требуется
    goto next_j;
    }

if ((ic>=128)&&(ic<256))
    {
    if (comp_charset==866)
        {comp_text[j]=c; // перекодировка не требуется
         goto next_j;
        }
    if (comp_charset==1251)
        {  // Win 2 DOS
        if ((ic>=192)&&(ic<240)) {ic-=64; comp_text[j]=(char)ic; goto next_j;}
        if (ic>=240) {ic-=16; comp_text[j]=(char)ic; goto next_j;}

        if (ic==168) {ic=240; comp_text[j]=(char)ic; goto next_j;} // Ё - v1.19
        if (ic==184) {ic=241; comp_text[j]=(char)ic; goto next_j;} // ё - v1.19

        ic=32;
        comp_text[j]=(char)ic;
        goto next_j;
        }

    comp_text[j]=c; // если charset не определен перекодировка не делается
    goto next_j;
    }


next_j:
    if (j<255) j++;
    comp_text[j]=0;

next_i: i++;
        if (i<k) goto m1;

end: return(re);
}

/*----------------------------- записать comp_text d mkbufp */
int comp_set_text(void)
{
int re=0;
int i,ic,k;

k=strlen(comp_text);

for (i=0; i<k; i++)
    {
    ic=(int)comp_text[i];
    if (ic<0) ic+=256;
    if ((ic<0)||(ic==3)||(ic>255)) ic=0;    // символ 3 ETX заменяет 0
    //comp_add_mkbufp(ic+256);  // До v0.21 перенести символ в программу +256=HEX
    comp_add_mkbufp(ic+512);    // С v0.21 перенести символ в программу +512=TEXT
    }

return(re);
}

/*-------------------------- COMP_STRUPR -----------*/

int comp_strupr(void)
{
// strupr для ctemp с учетом специфики строки программы
// т.е. не трогать текст и комментарии
int re=0;
int i,ic,k;
int flag_k, flag_tz, flag_s;
char c;

flag_k=0; // кавычки
flag_tz=0; // точка с запятой
flag_s=0; // слэш

k=strlen(ctemp);
i=0;

m1: c=ctemp[i];

    ic=(int)c;
    if (ic<0) ic+=256;
    if ((ic<0)||(ic>255)) ic=0;

    if (ic==0) goto end;

    if ((c=='"')&&(flag_s==0))
        { if (flag_k==0) flag_k=1;
          else flag_k=0;
        }

    if ((c==';')&&(flag_k==0))
        {
        flag_tz=1;
        }

    if ((ic==0x5C)&&(flag_k!=0))
        {
        if (flag_s==0) flag_s=1;
            else flag_s=0;
        }
        else flag_s=0;


    if ((c>='a')&&(c<='z')&&(flag_k==0)&&(flag_tz==0))
        ctemp[i]=(char)(ic-32); // a-z преобразовать в A-Z если не в кавычках и не в комментариях


    if (i<k) {i++; goto m1;}


end: return(re);
}

/*--------------------------------------------- */
int comp_get_num(char* sss, int n)
{
int i,j,j1,j2,j3;
int re=0;
int len;
int flag_chislo, flag_metka, flag_hex, flag_zap;
int flag_minus, flag_eeminus;
char c;
char temp[256];


// Считывание числа формат FORTRAN (число МК) DEC или HEX из строки sss с позиции n
// Может быть метка или идентификатор, если первая буква

i=n;
j=0;
len=strlen(sss);

comp_chislo=-1;
flag_chislo=0; // это число
flag_metka=0; // это метка
flag_hex=0; // шестн целое число
flag_zap=0; // запятая
flag_minus=0;   // минус в числе
flag_eeminus=0; // минус в порядке

m0:  c=sss[i];
     if ((c==0)||(c==';')||(c=='\r')||(c=='\n'))
        {if ((flag_chislo==0)&&(flag_metka==0)) {re=0; goto end;}
         if (flag_chislo!=0) {re=i; goto m1;}
         if (flag_metka!=0) {re=i; goto m2;}
        }

     if (c=='H')
        {
        if (flag_chislo==0)
            {
            flag_metka=1;
            goto m2;        // разбор метки или идентификатора
            }

        flag_hex=1;
        i++;
        re=i;
        goto m1;
        }

     if ((c==' ')||(c=='\t'))
        {
        if (flag_chislo!=0) {re=i; goto m1;}
        if (flag_metka!=0) {re=i; goto m2;}
        if (i<len) {i++; goto m0;}
            else goto m1;
        }


     if ((flag_chislo==0) && (flag_metka==0) && ((c<'0')||(c>'9')) && (c!='-') && (c!=',') && (c!='.'))
        {
        flag_metka=1;
        re=i;
        goto m2;        // разбор метки или идентификатора
        }

     if ( ((c>='0')&&(c<='9')) || ((c>='A')&&(c<='F')) || (c=='-') ||(c==',') ||(c=='.'))
        {
        if (c=='-')
            {
            if (flag_zap==1) flag_eeminus=1; else flag_minus=1;
            }
        if ((c==',')||(c=='.')) flag_zap=1;
        if ((c>='0')&&(c<='9')) flag_chislo=1;
        temp[j]=c;
        j++;
        temp[j]=0;
        if (i<len) {i++; goto m0;}
            else goto m1;
        }
        else goto m1;

m1:  // разбор перенесенного в temp числа
    if (j==0) {re=0; goto end;} // ошибка

    if ((flag_zap==0)&&(flag_hex!=0))
        {   // обычное целое число, но шестнадцатиричное

        comp_chislo=0;

        for (i=0; i<j; i++)
            {
            comp_chislo*=16;

            switch (temp[i])
                {
                case '0': break;
                case '1': comp_chislo+=1; break;
                case '2': comp_chislo+=2; break;
                case '3': comp_chislo+=3; break;
                case '4': comp_chislo+=4; break;
                case '5': comp_chislo+=5; break;
                case '6': comp_chislo+=6; break;
                case '7': comp_chislo+=7; break;
                case '8': comp_chislo+=8; break;
                case '9': comp_chislo+=9; break;
                case 'A': comp_chislo+=10; break;
                case 'B': comp_chislo+=11; break;
                case 'C': comp_chislo+=12; break;
                case 'D': comp_chislo+=13; break;
                case 'E': comp_chislo+=14; break;
                case 'F': comp_chislo+=15; break;
                default: break;
                } // end switch temp[i]
            } //end for i

        sprintf(temp,"%d",comp_chislo);
        j=strlen(temp);
        } // конец разбора шестнадцатиричного числа,
            //оно перенесено в temp в десятичном виде

        // разбор десятичных чисел из temp
        // в т.ч. подготовленных ранее шестнадцатиричных
        // и чисел с плавающей запятой
    for (i=0; i<j; i++)
        {
        switch (temp[i])
            {
            case '0': comp_add_mkbufp(0); break;
            case '1': comp_add_mkbufp(1); break;
            case '2': comp_add_mkbufp(2); break;
            case '3': comp_add_mkbufp(3); break;
            case '4': comp_add_mkbufp(4); break;
            case '5': comp_add_mkbufp(5); break;
            case '6': comp_add_mkbufp(6); break;
            case '7': comp_add_mkbufp(7); break;
            case '8': comp_add_mkbufp(8); break;
            case '9': comp_add_mkbufp(9); break;

            case ',':
            case '.': comp_add_mkbufp(10); break;
            case 'E': if (flag_minus!=0) {flag_minus=0; comp_add_mkbufp(11);}   // +/-
                        comp_add_mkbufp(12);
                        flag_minus=flag_eeminus;
                       break; // ВП
            default: break;
            } // end switch temp[i]

        }

        if (flag_minus!=0) {flag_minus=0; comp_add_mkbufp(11);} // минус мантиссы либо порядка

        goto end;


m2:     // разбор метки проще выполнить специализированной подпрограммой, чем начинать заново

        comp_chislo=-1;
            i=comp_get_adres(sss,n,3); // z=3 число идентификатор или метка
            if (i<=n)
                {
                comp_set_error(7,comp_stroka,""); // значение идентификатора не определено
                re=n;
                goto end;
                }
            if (comp_chislo<0)
                {
                // значение метки не обнаружено
                // оставить на второй проход, а пока вставить временное значение в mkbufp,
                // в структуру metka метка уже д.б. вставлена с use=2
                comp_add_mkbufp(comp_const+516); // число-адрес в RX
                comp_add_mkbufp(comp_const+512); // метка 2 байта
                comp_add_mkbufp(comp_const+512); // байт не используется
                comp_add_mkbufp(comp_const+512); // байт не используется
                re=i;
                goto end;
                }
            if (comp_chislo>9999)
                {
                // недопустимый адрес
                sprintf(ctemptoa,"%d",comp_chislo);
                comp_set_error(6,comp_stroka,ctemptoa);

                comp_add_mkbufp(comp_const+517); // ошибка числа-адреса в RX
                comp_add_mkbufp(comp_const+512); // метка 2 байта
                comp_add_mkbufp(comp_const+512); // байт не используется
                comp_add_mkbufp(comp_const+512); // байт не используется
                re=i;
                goto end;
                }

            // comp_chislo>=0 <=9999
            // записать число в программу
            j=comp_chislo;
            j1=(j/1000)%10;
            if (j1!=0) comp_add_mkbufp(j1);
            j%=1000;
            j2=(j/100)%10;
            if ((j1!=0)||(j2!=0)) comp_add_mkbufp(j2);
            j%=100;
            j3=(j/10)%10;
            if ((j1!=0)||(j2!=0)||(j3!=0)) comp_add_mkbufp(j3);
            j%=10;
            comp_add_mkbufp(j%10);  // ноль тоже записать
            re=i;
            goto end;


end: return(re);
}
/*----------------------------------------------------------------- v0.15, изм v0.21 ---*/
int comp_get_num_hl(char* sss, int n, int z)
{
int i,j;
int j1,j2,j3;
int re=0;
// разбор псевдооператора NUMH (при z=1) и NUML (z=0)
// разбор псевдооператора NUMT (при z=2) - v0.21
// разбор псевдооператоров NUMD (при z=3) и NUMI (z=4) - v0.24

comp_chislo=-1;

            i=comp_get_adres(sss,n,3); // =3 число идентификатор или метка
            if (i<=n)
                {
                comp_set_error(7,comp_stroka,""); // значение идентификатора не определено
                re=n;
                goto end;
                }
            if (comp_chislo<0)
                {
                // значение метки не обнаружено
                // оставить на второй проход, а пока вставить временное значение в mkbufp,
                // в структуру metka метка уже д.б. вставлена с use=2
                if (z==3)
                    {
                    comp_add_mkbufp(comp_const+527); // С v0.24 число-адрес с декрементом NUMD
                    comp_add_mkbufp(comp_const+512);
                    comp_add_mkbufp(comp_const+512);
                    }

                if (z==4)
                    {
                    comp_add_mkbufp(comp_const+528); // С v0.24 число-адрес с инкрементом NUMI
                    comp_add_mkbufp(comp_const+512);
                    comp_add_mkbufp(comp_const+512);
                    }

                if (z==2)
                    {
                    comp_add_mkbufp(comp_const+525); // С v0.21 число-адрес 3 байта в RX
                    comp_add_mkbufp(comp_const+512);
                    }
                if (z==1) comp_add_mkbufp(comp_const+518); // число-адрес в RX
                if (z==0) comp_add_mkbufp(comp_const+519);
                if ((z!=0) && (z!=1) && (z!=2)) comp_add_mkbufp(comp_const+517); //ошибка в z
                comp_add_mkbufp(comp_const+512); // метка 2 байта при z==0 или 1
                                // v0.21 - 3 байта при z==2
                re=i;
                goto end;
                }

            if (comp_chislo>9999)
                {
                // недопустимый адрес
                sprintf(ctemptoa,"%d",comp_chislo);
                comp_set_error(6,comp_stroka,ctemptoa);

                comp_add_mkbufp(comp_const+517); // ошибка числа-адреса в RX
                comp_add_mkbufp(comp_const+512); // метка 2 байта
                re=i;
                goto end;
                }

            // comp_chislo>=0 <=9999
            // записать число в программу
            if ((z==0)||(z==1)) // v0.21
                {
                j=comp_chislo;
                j1=(j/1000)%10;
                if ((j1!=0)&&(z==1)) comp_add_mkbufp(j1);
                j%=1000;
                j2=(j/100)%10;
                if (z==1) comp_add_mkbufp(j2);  // ноль тоже записать NUMH z==1
                j%=100;
                j3=(j/10)%10;
                if ((j3!=0)&& (z==0)) comp_add_mkbufp(j3);
                j%=10;
                if (z==0) comp_add_mkbufp(j%10);    // ноль тоже записать NUML z==0
                re=i;
                goto end;
                }

            if (z==2)   // добавлено с v0.21 - 3 или 4 байта
                {
                j=comp_chislo;

                j1=(j/1000)%10;
                if (j1!=0)
                    {comp_add_mkbufp(j1);   // записать тысячи, если они всё же есть
                    // и выдать предупреждение W003
                    sprintf(ctemptoa,"%d",comp_chislo);
                    comp_set_error(mkerror.w+3,comp_stroka,ctemptoa);
                    }
                j%=1000;

                j1=(j/100)%10;
                if (j1!=0) comp_add_mkbufp(j1);
                j%=100;
                j2=(j/10)%10;
                if (j2!=0) comp_add_mkbufp(j2);
                j%=10;
                comp_add_mkbufp(j); // ноль тоже записать
                re=i;
                goto end;
                }

            if ((z==3)||(z==4))
                { // v0.24 - NUMD и NUMI
                j=comp_chislo;
                if (z==3) j--;
                if (z==4) j++;
                if ((j<0)||(j>9999))
                    {
                    sprintf(ctemptoa,"%d",comp_chislo);
                    comp_set_error(6,comp_stroka,ctemptoa);

                    comp_add_mkbufp(comp_const+517); // ошибка числа-адреса в RX
                    comp_add_mkbufp(comp_const+512); // метка 2 байта
                    re=i;
                    goto end;
                    }
                j1=(j/1000)%10;
                if (j1!=0) comp_add_mkbufp(j1);
                j%=1000;
                j2=(j/100)%10;
                if ((j1!=0)||(j2!=0)) comp_add_mkbufp(j2);
                j%=100;
                j3=(j/10)%10;
                if ((j1!=0)||(j2!=0)||(j3!=0)) comp_add_mkbufp(j3);
                j%=10;
                comp_add_mkbufp(j%10);  // ноль тоже записать
                re=i;
                goto end;
                }

end: return(re);
}

/*-----------------------------------*/
int comp_set_oper(char* sss, int z)
{
// принимает и разбирает мнемонику одного оператора начиная с начала строки
// z=0 - если есть оканчивающийся в этом месте оператор из нескольких (например +/- и +) то вставить его
// z=1 - не разрыв и не конец строки

int re=-1;
// -2: ошибка нет такого оператора
// -1: возможны варианты - продолжить разбор
// 0: оператор разобран параметров нет
// 1-4: есть дополнительные параметры
// 1,2 - адрес 1 или 2 байта,
// 3,4 - номер регистра 1 или 2 байта.

int i,j,k,len,noper,ioper;
int flag_sovp;


len=strlen(sss);

// инициализация
if (mkkom.init==0) init_mkkom();

noper=0; // число найденных операторов
ioper=0;

// поиск в таблице на точное совпадение при z==0
if (z==0)
    {
    for (i=0; i<256; i++)
        {
        k=strlen(mkkom.name2[i]);
        if (k==len)
            {
            flag_sovp=1;
            for (j=0; j<k; j++)
                {
                if (mkkom.name2[i][j]!=sss[j]) flag_sovp=0;
                }
            if (flag_sovp==1) {noper++; ioper=i;}// точное совпадение
            }

        }

    if ((noper==1)&&(ioper>=0)&&(ioper<=255))
        {
        // вставить найденный оператор
        comp_add_mkbufp(ioper);
        re=mkkom.par[ioper];
        goto end;
        }
    re=-1;
    goto end;
    } // end z==0

// поиск в таблице на совпадение при z=1 (может быть несколько совпадений)
for (i=0; i<256; i++)
    {
    k=strlen(mkkom.name2[i]);
        if (k>=len)
            { // более короткие операторы не проверять
            flag_sovp=1;
            for (j=0; j<len; j++)
                {
                if (mkkom.name2[i][j]!=sss[j]) flag_sovp=0;
                }
            if (flag_sovp==1)
                {noper++;
                if (k==len) ioper=i; else ioper=-1;
                // если более одного совпадения ioper будет не нужен
                }
            }

    }

if ((noper==1)&&(ioper>=0)&&(ioper<=255))
        {
        // одно точное совпадение
        // вставить найденный оператор
        comp_add_mkbufp(ioper);
        re=mkkom.par[ioper];
        goto end;
        }

if (noper>1) {re=-1; goto end;} // более одного совпадения
if (noper==0) {re=-2; goto end;} // нет совпадений

end: return(re);
}
/*-----------------------------------*/
int comp_get_oper(char* sss, int n)
{
int i,i1,j,k;
int re=0;
// Считывание оператора из строки sss с позиции n
// Заполнение структур metka и массива mkbufp[]
// Оператор - {пусто | псевдооператор | оператор} [м.б. адрес]
// Адрес - {число от 0 до 9999 (или шестнадцатиричное) | идентификатор}
int flag_oper;
char c;
char temp[256];

flag_oper=0;

k=strlen(sss);

if ((sss[n]==' ')||(sss[n]=='\t'))
    { // пусто
     re=n+1;
     goto end;
    }

if (sss[n]=='.')
    { // псевдооператор
    /*
    .END конец программы
    .EQU устанавливает значение идентификатора равное числу
        (здесь игнорировать оба, уже разобрано!)
    .CHARSET число 866 1251 влияет на разбор текста .TEXT содержимое в comp_charset
    .DB число 0-255
    .DA число 0-9999 (или FFFF=16665 при comp_flag_adres!=0) BCD адрес 2 байта старший /100 младший %100
    .DAH только старшая часть адреса 1 байт
    .DAL только младшая часть адреса 1 байт

    .DD число 0-9999 в десятичном виде по байтам v0.18
    .DDH старшая часть - байт = /100 v0.18
    .DDL младшая часть - байт = %100 v0.18

    .TEXT "строка" в т ч \n \r \t \\ \"
    .NUM вставляет число или адрес метки в программу в виде числа в RX

    .NUMH старшая часть числа или адреса (номер страницы) в RX (с v0.15)
    .NUML младшая часть числа или адреса в RX (с v0.15)
    .NUMT число или адрес от 0 до 999 в RX (с v0.21)
    .NUMD и .NUMI число или адрес в RX а декрементом и инкрементом (с v0.24)

    .ORG число 0-9999 или определенный ранее идентификатор
    .PAGE = ORG с новой страницы кратной 100
    .OPT число 0-нет, 1 (бит0) -использовать шестн адреса,

    .BASE - установка текущего адреса как базы для вычисления смещения в .DBB (v0.23)
    .DBB - смещение 0-255 относительно базы (вставляется метка-база), можно через запятую (v0.23)

    .DW -  число или адрес 2 байта HEX ст-мл (v0.25)
    .DWB - адрес по базе 2 байта HEX ст-мл (v0.25)


    .FORM - форма ГИ == .BASE + DB + 7*DBB (v0.27)
    .TAB - нач позиция == .DB 96 + 2*DB (v0.27)
    .LDX - позиция по X из регистра == .DB 104 + DA1 (v0.27)
    .LDY - позиция по Y из регистра == .DB 105 + DA1 (v0.27)
    .TYPE - шрифт == .DB 101 + DB
    .ATYPE
	.LDADR
	.SQU
	.FRAME
	.DRAWR
	.DRAWB
	.TXTP
	.TXTB
	.TXTK
	.PREXP
	.PRFORM
	.PRHEX
	.MCALL
	.RETUM
	.LCALL
	.RETUL
	.JNEZ
	.JM
	.END - сейчас вставляет 0xFF (255) - старая команда .ENDP

    */

    if (n+7<k)
        {
        if ((sss[n+1]=='C')&&(sss[n+2]=='H')&&(sss[n+3]=='A')&&(sss[n+4]=='R')&&
            (sss[n+5]=='S')&&(sss[n+6]=='E')&&(sss[n+7]=='T'))
            { // .CHARSET
            comp_chislo=-1;
            i=comp_get_chislo(sss,n+8);
            if (i>n+8)
                {
                if (comp_chislo==866) comp_charset=866;
                if (comp_chislo==1251) comp_charset=1251;
                if ((comp_chislo!=866)&&(comp_chislo!=1251))
                    {
                    // значение charset не определено
                    sprintf(ctemptoa,"%d",comp_chislo);
                    comp_set_error(8,comp_stroka,ctemptoa); // неверное значение
                    }
                re=i;
                goto end;
                }
            else
                {
                comp_set_error(8,comp_stroka,""); // неверное значение
                re=n+8;
                goto end;
                }

            } // end .CHARSET

        } // end len 7

   if (n+6<k)
	{ // len 6
	if ((sss[n+1]=='P')&&(sss[n+2]=='R')&&(sss[n+3]=='F')&&(sss[n+4]=='O')
						&&(sss[n+5]=='R')&&(sss[n+6]=='M'))
            { // .PRFORM v0.27
            comp_add_mkbufp(349); // = 93 (PRFORM) + 256 (HEX)
            re=comp_get_dbz(sss,n+7,1); // считать 1 байт атрибутов
		i=re;
		if (i<=0) goto end;
	    re=comp_get_da1(sss,i,0); // считать 1 адрес BCD, параметр z=0
		i=re;
		if (i<=0) goto end;
            re=comp_get_dbz(sss,i,2); // считать 2 байта M,N
            goto end;
            } // end .PRFORM


	} // end len 6


    if (n+5<k)
	{ // len 5

	// ГИ (v0.27)
        if ((sss[n+1]=='A')&&(sss[n+2]=='T')&&(sss[n+3]=='Y')&&(sss[n+4]=='P')&&(sss[n+5]=='E'))
            { // .ATYPE v0.27
            comp_add_mkbufp(334); // = 78 (ATYPE) + 256 (HEX)
            re=comp_get_da1(sss,n+6,0); // считать 1 адрес BCD, параметр z=0
            goto end;
            } // end .ATYPE


	if ((sss[n+1]=='L')&&(sss[n+2]=='D')&&(sss[n+3]=='A')&&(sss[n+4]=='D')&&(sss[n+5]=='R'))
            { // .LDADR v0.27
            comp_add_mkbufp(333); // = 77 (LDADR) + 256 (HEX)
            re=comp_get_da1(sss,n+6,0); // считать 1 адрес BCD, параметр z=0
            goto end;
            } // end .LDADR

	if ((sss[n+1]=='F')&&(sss[n+2]=='R')&&(sss[n+3]=='A')&&(sss[n+4]=='M')&&(sss[n+5]=='E'))
            { // .FRAME v0.27
            comp_add_mkbufp(347); // = 91 (FRAME) + 256 (HEX)
            re=comp_get_dbz(sss,n+6,3); // считать 3 байта
            goto end;
            } // end .FRAME

	if ((sss[n+1]=='D')&&(sss[n+2]=='R')&&(sss[n+3]=='A')&&(sss[n+4]=='W')&&(sss[n+5]=='R'))
            { // .DRAWR v0.27
            comp_add_mkbufp(340); // = 84 (DRAWR) + 256 (HEX)
            re=comp_get_dbz(sss,n+6,1); // считать 1 байт атрибутов
		i=re;
		if (i<=0) goto end;
	    re=comp_get_da1(sss,i,0); // считать 1 адрес BCD, параметр z=0
            goto end;
            } // end .DRAWR

	if ((sss[n+1]=='D')&&(sss[n+2]=='R')&&(sss[n+3]=='A')&&(sss[n+4]=='W')&&(sss[n+5]=='B'))
            { // .DRAWB v0.27
            comp_add_mkbufp(342); // = 86 (DRAWB) + 256 (HEX)
            re=comp_get_dbz(sss,n+6,1); // считать 1 байт атрибутов
		i=re;
		if (i<=0) goto end;
	    re=comp_get_da1(sss,i,0); // считать 1 адрес BCD, параметр z=0
            goto end;
            } // end .DRAWB

	if ((sss[n+1]=='P')&&(sss[n+2]=='R')&&(sss[n+3]=='E')&&(sss[n+4]=='X')&&(sss[n+5]=='P'))
            { // .PREXP v0.27
            comp_add_mkbufp(348); // = 92 (PREXP) + 256 (HEX)
            re=comp_get_dbz(sss,n+6,1); // считать 1 байт атрибутов
		i=re;
		if (i<=0) goto end;
	    re=comp_get_da1(sss,i,0); // считать 1 адрес BCD, параметр z=0
            goto end;
            } // end .PREXP

	if ((sss[n+1]=='P')&&(sss[n+2]=='R')&&(sss[n+3]=='H')&&(sss[n+4]=='E')&&(sss[n+5]=='X'))
            { // .PRHEX v0.27
            comp_add_mkbufp(350); // = 94 (PRHEX) + 256 (HEX)
            re=comp_get_dbz(sss,n+6,1); // считать 1 байт атрибутов
		i=re;
		if (i<=0) goto end;
	    re=comp_get_da1(sss,i,0); // считать 1 адрес BCD, параметр z=0
            goto end;
            } // end .PRHEX

	if ((sss[n+1]=='M')&&(sss[n+2]=='C')&&(sss[n+3]=='A')&&(sss[n+4]=='L')&&(sss[n+5]=='L'))
            { // .MCALL v0.27
            comp_add_mkbufp(358); // = 102 (MCALL) + 256 (HEX)
            re=comp_get_dbbz(sss,n+6,1); // считать 1 байт смещения
            goto end;
            } // end .MCALL

	if ((sss[n+1]=='R')&&(sss[n+2]=='E')&&(sss[n+3]=='T')&&(sss[n+4]=='U')&&(sss[n+5]=='M'))
            { // .RETUM v0.27
            comp_add_mkbufp(359); // = 103 (RETUM) + 256 (HEX)
            re=n+6;
            goto end;
            } // end .RETUM

	if ((sss[n+1]=='L')&&(sss[n+2]=='C')&&(sss[n+3]=='A')&&(sss[n+4]=='L')&&(sss[n+5]=='L'))
            { // .LCALL v0.27
            comp_add_mkbufp(368); // = 112 (LCALL) + 256 (HEX)
            re=comp_get_da1(sss,n+6,0); // считать BCD адрес z=0 - 2 байта
            goto end;
            } // end .LCALL

	if ((sss[n+1]=='R')&&(sss[n+2]=='E')&&(sss[n+3]=='T')&&(sss[n+4]=='U')&&(sss[n+5]=='L'))
            { // .RETUL v0.27
            comp_add_mkbufp(369); // = 113 (RETUL) + 256 (HEX)
	    re=n+6;
            goto end;
            } // end .RETUL


	} // end len 5

    if (n+4<k)
        {
        if ((sss[n+1]=='P')&&(sss[n+2]=='A')&&(sss[n+3]=='G')&&(sss[n+4]=='E'))
            { // .PAGE
            // проверить существование метки указывающей на адрес до изменения
            j=100*((comp_adres+99)/100);    // целые сотни
            if ((j>=0)&&(j<10000)&&(j%100==0))
                {
                if (metka.vsego>0)
                    {
                    if ((metka.val[metka.vsego-1]==comp_adres)&&((metka.use[metka.vsego-1]&1)!=0) )
                        {
                        // если последняя установленная метка существует
                        // и указывает на ORG - заменить ее значение
                        metka.val[metka.vsego-1]=j;
                        }
                    }

                comp_adres=j; // заменить счетчик адреса
                }
            else
                {
                sprintf(ctemptoa,"%d",j);
                comp_set_error(6,comp_stroka,ctemptoa); // недопустимый адрес
                }
            re=n+5;
            goto end;
            } // end .PAGE


        if ((sss[n+1]=='T')&&(sss[n+2]=='E')&&(sss[n+3]=='X')&&(sss[n+4]=='T'))
            { // .TEXT
            re=comp_get_text(sss,n+5);
            comp_set_text();
            goto end;
            } // end .TEXT

        // NUMH и NUML введены с версии 0.15

        if ((sss[n+1]=='N')&&(sss[n+2]=='U')&&(sss[n+3]=='M')&&(sss[n+4]=='H'))
            { // .NUMH
            re=comp_get_num_hl(sss,n+5,1);  // z==1 - NUMH
            goto end;
            } // end .NUMH


        if ((sss[n+1]=='N')&&(sss[n+2]=='U')&&(sss[n+3]=='M')&&(sss[n+4]=='L'))
            { // .NUML
            re=comp_get_num_hl(sss,n+5,0);  // z==0 - NUML
            goto end;
            } // end .NUML

        // NUMT введен с версии 0.21

        if ((sss[n+1]=='N')&&(sss[n+2]=='U')&&(sss[n+3]=='M')&&(sss[n+4]=='T'))
            { // .NUMT
            re=comp_get_num_hl(sss,n+5,2);  // z==2 - NUMT
            goto end;
            } // end .NUMT

        if ((sss[n+1]=='B')&&(sss[n+2]=='A')&&(sss[n+3]=='S')&&(sss[n+4]=='E'))
            { // .BASE (v0.23)
            re=comp_set_base(sss,n+5,0);    // z==0
            goto end;
            } // end .BASE

        // NUMD и NUMI введены с версии v0.24

        if ((sss[n+1]=='N')&&(sss[n+2]=='U')&&(sss[n+3]=='M')&&(sss[n+4]=='D'))
            { // .NUMD
            re=comp_get_num_hl(sss,n+5,3);  // z==3 - NUMD
            goto end;
            } // end .NUMD

        if ((sss[n+1]=='N')&&(sss[n+2]=='U')&&(sss[n+3]=='M')&&(sss[n+4]=='I'))
            { // .NUMI
            re=comp_get_num_hl(sss,n+5,4);  // z==4 - NUMI
            goto end;
            } // end .NUMI

	// v0.27 - операторы ГИ

        if ((sss[n+1]=='F')&&(sss[n+2]=='O')&&(sss[n+3]=='R')&&(sss[n+4]=='M'))
            { // .FORM v0.27
            re=comp_set_base(sss,n+5,0);    // z==0
		if (re<=0) goto end;
            re=comp_get_dbz(sss,n+5,1); // считать байт, параметр z - количество байт =1
		if (re<=0) goto end;
		i=re;
	    re=comp_get_dbbz(sss,i,7); // считать смещения по уст. базе, параметр z - количество байт =7
            goto end;
            } // end .FORM

        if ((sss[n+1]=='T')&&(sss[n+2]=='Y')&&(sss[n+3]=='P')&&(sss[n+4]=='E'))
            { // .TYPE v0.27
            comp_add_mkbufp(357); // = 101 (TYPE) + 256 (HEX)
            re=comp_get_dbz(sss,n+5,1); // считать байт, параметр z - количество байт =1
            goto end;
            } // end .TYPE

        if ((sss[n+1]=='T')&&(sss[n+2]=='X')&&(sss[n+3]=='T')&&(sss[n+4]=='P'))
            { // .TXTP v0.27
            comp_add_mkbufp(339); // = 83 (TXTP) + 256 (HEX)
            re=comp_get_dbz(sss,n+5,1); // считать 1 байт атрибутов
		i=re;
		if (i<=0) goto end;
	    re=comp_get_da1(sss,i,0); // считать 1 адрес BCD, параметр z=0
            goto end;
            } // end .TXTP

        if ((sss[n+1]=='T')&&(sss[n+2]=='X')&&(sss[n+3]=='T')&&(sss[n+4]=='B'))
            { // .TXTB v0.27
            comp_add_mkbufp(341); // = 85 (TXTB) + 256 (HEX)
            re=comp_get_dbz(sss,n+5,1); // считать 1 байт атрибутов
		i=re;
		if (i<=0) goto end;
	    re=comp_get_da1(sss,i,0); // считать 1 адрес BCD, параметр z=0
            goto end;
            } // end .TXTB

        if ((sss[n+1]=='T')&&(sss[n+2]=='X')&&(sss[n+3]=='T')&&(sss[n+4]=='K'))
            { // .TXTK v0.27
            comp_add_mkbufp(343); // = 87 (TXTK) + 256 (HEX)
            re=comp_get_dbz(sss,n+5,1); // считать 1 байт атрибутов
		i=re;
		if (i<=0) goto end;
	    re=comp_get_da1(sss,i,0); // считать 1 адрес BCD, параметр z=0
            goto end;
            } // end .TXTK

        if ((sss[n+1]=='J')&&(sss[n+2]=='N')&&(sss[n+3]=='E')&&(sss[n+4]=='Z'))
            { // .JNEZ v0.27
            comp_add_mkbufp(257); // = 1 (JNEZ) + 256 (HEX)
	    re=comp_get_da1(sss,n+5,0); // считать 1 адрес BCD, параметр z=0
		i=re;
		if (i<=0) goto end;
            re=comp_get_dbbz(sss,i,1); // считать 1 байт смещение
            goto end;
            } // end .JNEZ

        if ((sss[n+1]=='E')&&(sss[n+2]=='N')&&(sss[n+3]=='D')&&(sss[n+4]=='P'))
            { // .ENDP v0.27
	    re=n+5;	// пока - игнорировать
            goto end;
            } // end .ENDP
        } // end len 4


    if (n+3<k)
        {
        if ((sss[n+1]=='E')&&(sss[n+2]=='Q')&&(sss[n+3]=='U'))
            { // .EQU
            re=n+4; // игнорировать
            goto end;
            } // end .EQU

        /* изменено с v0.27 (заменена на .ENDP)
	if ((sss[n+1]=='E')&&(sss[n+2]=='N')&&(sss[n+3]=='D'))
            { // .END
            re=n+4; // игнорировать
            goto end;
            } // end .END
	*/

        if ((sss[n+1]=='O')&&(sss[n+2]=='R')&&(sss[n+3]=='G'))
            { // .ORG
            i=comp_get_adres(sss,n+4,2); // z=2 только число или идентификатор!
            if ((i>n+4)&&(comp_chislo>=0)&&(comp_chislo<=9999))
                {
                // проверить существование метки указывающей на адрес до изменения
                if (metka.vsego>0)
                    {
                    if ((metka.val[metka.vsego-1]==comp_adres)&&((metka.use[metka.vsego-1]&1)!=0) )
                        {
                        // если последняя установленная метка существует
                        // и указывает на ORG - заменить ее значение
                        metka.val[metka.vsego-1]=comp_chislo;
                        }
                    }

                comp_adres=comp_chislo; // заменить счетчик адреса
                }
            else
                {
                sprintf(ctemptoa,"%d",comp_chislo);
                comp_set_error(6,comp_stroka,ctemptoa); // недопустимый адрес
                if (i<n+4) i=n+4;
                }
            re=i;
            goto end;
            } // end .ORG



        if ((sss[n+1]=='N')&&(sss[n+2]=='U')&&(sss[n+3]=='M'))
            { // .NUM
            i=comp_get_num(sss,n+4);
            if (i<=n+4)
                {
                comp_set_error(8,comp_stroka,""); // неверное значение
                re=n+4;
                goto end;
                }
            else
                {
                re=i;
                goto end;
                }

            } // end .NUM


        if ((sss[n+1]=='O')&&(sss[n+2]=='P')&&(sss[n+3]=='T'))
            { // .OPT
            comp_chislo=-1;
            i=comp_get_chislo(sss,n+4);
            if ((i<=n+4)||(comp_chislo<0))
                {
                comp_set_error(8,comp_stroka,""); // неверное значение
                re=n+4;
                goto end;
                }
            else
                {
                if ((comp_chislo & 1)!=0) comp_flag_adres=1;
                    else comp_flag_adres=0;


                re=i;
                goto end;
                }

            } // end .OPT


        if ((sss[n+1]=='D')&&(sss[n+2]=='A')&&(sss[n+3]=='H'))
            { // .DAH
            re=comp_get_da(sss,n+4,2);
            goto end;
            }


        if ((sss[n+1]=='D')&&(sss[n+2]=='A')&&(sss[n+3]=='L'))
            { // .DAH
            re=comp_get_da(sss,n+4,1);
            goto end;
            }

        if ((sss[n+1]=='D')&&(sss[n+2]=='D')&&(sss[n+3]=='H'))
            { // .DDH v0.18
            re=comp_get_dd(sss,n+4,2);
            goto end;
            }


        if ((sss[n+1]=='D')&&(sss[n+2]=='D')&&(sss[n+3]=='L'))
            { // .DDL v0.18
            re=comp_get_dd(sss,n+4,1);
            goto end;
            }


        if ((sss[n+1]=='D')&&(sss[n+2]=='B')&&(sss[n+3]=='B'))
            { // .DBB v0.23 (v1.18 mk.exe)
            re=comp_get_dbb(sss,n+4,1);
            goto end;
            }

        if ((sss[n+1]=='D')&&(sss[n+2]=='W')&&(sss[n+3]=='B'))
            { // .DWB v0.25 (v1.23 mk.exe)
            re=comp_get_dw(sss,n+4,1);
            goto end;
            }

	// ГИ (v0.27)
        if ((sss[n+1]=='T')&&(sss[n+2]=='A')&&(sss[n+3]=='B'))
            { // .TAB v0.27
            comp_add_mkbufp(352); // = 96 (TAB) + 256 (HEX)
            re=comp_get_dbz(sss,n+4,2); // считать байт, параметр z - количество байт =2
            goto end;
            }

        if ((sss[n+1]=='L')&&(sss[n+2]=='D')&&(sss[n+3]=='X'))
            { // .LDX v0.27
            comp_add_mkbufp(360); // = 104 (LDX) + 256 (HEX)
            re=comp_get_da1(sss,n+4,0); // считать адрес, параметр z=0 - 2 байта BCD
            goto end;
            }

        if ((sss[n+1]=='L')&&(sss[n+2]=='D')&&(sss[n+3]=='Y'))
            { // .LDY v0.27
            comp_add_mkbufp(361); // = 105 (LDY) + 256 (HEX)
            re=comp_get_da1(sss,n+4,0); // считать адрес, параметр z=0 - 2 байта BCD
            goto end;
            }

       if ((sss[n+1]=='S')&&(sss[n+2]=='Q')&&(sss[n+3]=='U'))
            { // .SQU v0.27
            comp_add_mkbufp(345); // = 89 (SQU) + 256 (HEX)
            re=comp_get_dbz(sss,n+4,3); // считать 3 байта
            goto end;
            }

        if ((sss[n+1]=='E')&&(sss[n+2]=='N')&&(sss[n+3]=='D'))
            { // .END v0.27
		// Действие команды изменено - старая команда окончания программы = .ENDP
            comp_add_mkbufp(511); // = 255 (SQU) + 256 (HEX)
            re=n+4; // игнорировать
            goto end;
            } // end .END

        } // end len 3

    if (n+2<k)
        {
        if ((sss[n+1]=='D')&&(sss[n+2]=='B'))
            {
            // числа через запятую
            re=comp_get_db(sss,n+3,1); // параметр z - разбор по байтам
		// v0.27 исправлена i на re
            goto end;
            } // end .DB

        if ((sss[n+1]=='D')&&(sss[n+2]=='A'))
            { // .DA
            re=comp_get_da(sss,n+3,0);
            goto end;
            } // end .DA

        if ((sss[n+1]=='D')&&(sss[n+2]=='D'))
            { // .DD v0.18
            re=comp_get_dd(sss,n+3,0);
            goto end;
            } // end .DD

        if ((sss[n+1]=='D')&&(sss[n+2]=='W'))
            { // .DW v0.25 (v1.23 mk.exe)
            re=comp_get_dw(sss,n+3,0);
            goto end;
            } // end .DW

	// ГИ - v0.27
       if ((sss[n+1]=='J')&&(sss[n+2]=='M'))
            { // .JM v0.27
            comp_add_mkbufp(312); // = 56 (JM) + 256 (HEX)
            re=comp_get_dbb(sss,n+3,1);	// считать 1 байт смещение
            goto end;
            } // end .JM

        } // end len 2

    } // end псевдооператор ('.')


    // разбор операторов языка sss[n]!=' ' и sss[n]!='.'
    i=n;
    j=0;
    flag_oper=0;
    i1=-1;  // флаг разбора оператора
    temp[0]=0;

m0: c=sss[i];

    if ((c==0)||(c=='\r')||(c=='\n')||(c==';'))
        {
        if (flag_oper==1)
            {
            i1=comp_set_oper(temp,0);   // проверить с учетом конца строки
            if (i1<0) comp_set_error(9,comp_stroka,temp); // неопознан оператор
            if (i1>0) comp_set_error(10,comp_stroka,temp); // отсутствуют параметры оператора
            // если i1==0 - оператор опознан параметров нет
            }
        re=i;
        goto end;
        }

    if (((c==' ')||(c=='\t')) && (flag_oper==0))
        {if (i<k) {i++; goto m0;} // пробел в тексте вне оператора - игнорировать на всякий случай...
         //хотя уже был выход в этом случае
         else {re=i; goto end;}
        }

    if (((c==' ')||(c=='\t')) && (flag_oper==1))
        {
        // пробел в тексте внутри оператора - перейти к разбору
        i1=comp_set_oper(temp,0); // проверить, можно ли здесь окончить оператор
        if ((i1<-1)||(i1>=0)) goto m01; // завершить разбор оператора
        // иначе (i1==-1) продолжить
         if (i<k) {i++; goto m0;} // пробел в тексте внутри оператора
         else {re=i; goto end;}
        }

    flag_oper=1;
    temp[j]=c;
    if (j<255) j++;
    temp[j]=0;

    i1=comp_set_oper(temp,1);

    if (i1==-1)
        {
        // пока еще есть альтернативные варианты разбора
        if (i<k) {i++; goto m0;}
         else {re=i; goto end;}
        }

m01:

    if (i1<-1)
        {
        // ситуация может возникнуть только если раньше была альтернатива,
        // а теперь ее нет совсем
        // проверить что будет, если убрать последний символ
        if (j>1)
            {
            temp[j-1]=0;
            i1=comp_set_oper(temp,0);   // и считать это концом оператора
            if ((i1>=0)&&(i>1)) {i-=2;  goto m02;}
            // а так все нормально, значит снять символ и продолжить разбор c текущего
            }
        comp_set_error(9,comp_stroka,temp); // иначе неопознан оператор

        if (i<k) i++;
        re=i;
        goto end;
        }

m02:

    if (i1==0)
        {
        // ==0 OK оператор разобран без параметров
        if (i<k) i++;
        re=i;
        goto end;
        }

    if ((i1>0)&&(i1<5))
        {
        // оператор опознан и успешно разобран в i1 признак наличия адреса или номера регистра
        // 0-нет, 1-адрес 1 байт, 2 - адрес 2 б, 3 - регистр 1 б, 4 - регистр 2 б
        if (i<k) i++;
        i1=comp_set_adres(sss,i,i1);
        if (i1<=i) re=i;
            else re=i1;
        goto end;
        }

    // i1>=5    непонятно что это, но выйдем корректно
    if (i<k) i++;
    re=i;


end: return (re);
}
/*------------------------------------------- COMP PASS 2 ----*/
int comp_pass2(void)
{
// на этом проходе происходит заполнение меток еще не определенных при разборе
// т.е. адреса переходов вперед

int re=0; // 0-OK re!=0 - есть ошибки на втором проходе

int i,j,k,k1,ia,ia1,ia2;
int i1; // v0.23 (v1.18 mk.exe) для =526
int progmin, progmax; // начало и конец программы

progmin=-1;
progmax=-1;

for (i=0; i<10000; i++)
    {
    j=mkbufp[i]; // содержимое программы
    /* буфер для компиляции программы
        Девятый бит (256-511) использовать под признак HEX
        С v0.21 (512-767) - признак TEXT

        С v0.21 -=comp_const
        =512 - пустой буфер до компиляции
        =513 - метка из структуры mkmetka вставить по адресу 1 байт
        =514 - метка из структуры mkmetka вставить по адресу 2 байта
        =515 - ошибка при использовании короткого адреса (недопустимое значение)
        =516 - адрес в виде числа в программе 4 байта
        =517 - ошибка при использовании адреса в виде числа

        =518 (v0.15) - адрес в виде числа (NUMH = ст. часть) 2 байта
        =519 (v0.15) - адрес в виде числа (NUML = мл. часть) 2 байта
        =520 (v0.15) - метка из mkmetka - 1 байт - ст.часть, исправлено в v0.16

        =521 (v0.18)    - вставить байт из структуры mkmetka - DEC DDH
        =522 (v0.18)    - вставить байт из структуры mkmetka - DEC DDL
        =523 (v0.18)    - вставить 2 байта из структуры mkmetka - DEC DD
        =524 (v0.18)    - вставить 1 байт из структуры mkmetka BCD DAL

        =525 (v0.21)    - адрес в виде числа (NUMT) 3 байта
        =526 (v0.23)    - для dbb

        =527 (v0.24)    - NUMD (адрес с декрементом)
        =528 (v0.24)    - NUMI (адрес с инкрементом)

        =529 (v0.25)    - DW    (2 байта 16-ричное число ст-мл, в тч адрес)
        =530 (v0.25)    - DWB   (2 байта 16-ричное число ст-мл - адрес по базе)

    */

    if ((j>=0)&&(j<comp_const)) {progmax=i; goto next_i;}   // c v0.21 обычный байт: команда, hex, text

    j-=comp_const;  // v0.21
    if (j==512)
        {
        // =512 - не использованный байт
        if (progmax<0) progmin=i;   // если программа еще не начиналась увеличить progmin
        mkbufp[i]=511; // и заменить байтом HEX 0FFh (=511)
        goto next_i;
        }


    if ((j==513)||(j==514)||(j==520))
        {
        // =513 - метка из структуры mkmetka вставить по адресу 1 байт
        // =514 - метка из структуры mkmetka вставить по адресу 2 байтa
        // =520 - метка вставить 1 байт старший v0.15 исправлено в 0.16
        for (k=0; k<metka.vsego; k++)
            {
            if ((metka.val[k]==i)&&(metka.use[k]==2))
                {
                // метка содержит ссылку на этот текущий адрес
                k1=comp_find_metka(metka.name[k]);
                if ((k1<0)||(k1>=metka.vsego))
                    {
                    // ошибка - метка не определена
                    comp_set_error(11,metka.stroka[k],metka.name[k]);
                    if (re<256) re++;
                    goto next_i;
                    }
                // иначе вставить в программу значение метки
                comp_stroka=metka.stroka[k];
                comp_adres=i;
                //v0.16
                if ((j==513)||(j==514))
                    {
                    // как в версиях до v0.15
                    comp_chislo=metka.val[k1];  // число взять из определенной метки
                    comp_set_adres(ctemp,0,j-508);  // фиктивная строка и n, z=5 или 6 адрес из comp_chislo
                    }
                if (j==520)
                    {//v0.16
                    comp_chislo=metka.val[k1]/100;
                    comp_set_adres(ctemp,0,7);  // фиктивная строка и n, z=7 - один байт без проверки comp_adres
                    }

                goto next_i;    // одного совпадения достаточно
                }
            }
        // нет ни одного совпадения - ??
        if (re<256) re++;
        comp_set_error(11,0,"");
        }

    if ((j==515)||(j==517))
        {
        // =515 - ошибка при использовании короткого адреса (недопустимое значение)
        // =517 - ошибка при использовании адреса в виде числа
        re++;
        }

    if ((j==516)||(j==518)||(j==519))
        {
        // =516 - адрес в виде числа в программе 4 байта
        // =518 только старшая или =519 младшая часть адреса v0.15
        // записать число в программу
        for (k=0; k<metka.vsego; k++)
            {
            if ((metka.val[k]==i)&&(metka.use[k]==2))
                {
                // метка содержит ссылку на этот текущий адрес
                k1=comp_find_metka(metka.name[k]);
                if ((k1<0)||(k1>=metka.vsego))
                    {
                    // ошибка - метка не определена
                    comp_set_error(11,metka.stroka[k],metka.name[k]);
                    if (re<256) re++;
                    goto next_i;
                    }
                // иначе вставить в программу значение метки
                comp_stroka=metka.stroka[k];
                comp_adres=i;
                ia=metka.val[k1];   // число взять из определенной метки
                // j=516 записать 4 байта, (с v0.15) 518 и 519 - 2 байта
                ia1=(ia/1000)%10;
                if ((j==516)||(j==518)) comp_add_mkbufp(ia1);
                ia%=1000;
                ia1=(ia/100)%10;
                if ((j==516)||(j==518)) comp_add_mkbufp(ia1);
                ia%=100;
                ia1=(ia/10)%10;
                if ((j==516)||(j==519)) comp_add_mkbufp(ia1);
                ia%=10;
                if ((j==516)||(j==519)) comp_add_mkbufp(ia%10);
                goto next_i;    // одного совпадения достаточно
                }
            }
        // нет ни одного совпадения - ??
        if (re<256) re++;
        comp_set_error(11,0,"");
        }

    if ((j==521)||(j==522)||(j==523)||(j==524))
        {
        //=521 (v0.18)  - вставить байт из структуры mkmetka - DEC DDH
        //=522 (v0.18)  - вставить байт из структуры mkmetka - DEC DDL
        //=523 (v0.18)  - вставить 2 байта из структуры mkmetka - DEC DD
        //=524 (v0.18)  - вставить 1 байт из структуры mkmetka BCD DAL

        for (k=0; k<metka.vsego; k++)
            {
            if ((metka.val[k]==i)&&(metka.use[k]==2))
                {
                // метка содержит ссылку на этот текущий адрес
                k1=comp_find_metka(metka.name[k]);
                if ((k1<0)||(k1>=metka.vsego))
                    {
                    // ошибка - метка не определена
                    comp_set_error(11,metka.stroka[k],metka.name[k]);
                    if (re<256) re++;
                    goto next_i;
                    }
                // иначе вставить в программу значение метки
                comp_stroka=metka.stroka[k];
                comp_adres=i;
            if (j==521)
                    {
                    comp_chislo=metka.val[k1];  // число взять из определенной метки
                    comp_chislo/=100;
                    comp_set_adres(ctemp,0,8);  // фиктивная строка и n, z=8 число из comp_chislo
                    }

            if (j==522)
                    {
                    comp_chislo=metka.val[k1];  // число взять из определенной метки
                    comp_chislo%=100;
                    comp_set_adres(ctemp,0,8);  // фиктивная строка и n, z=8 число из comp_chislo
                    }
            if (j==523)
                    {
                    comp_chislo=metka.val[k1];  // число взять из определенной метки
                    comp_set_adres(ctemp,0,9);  // фиктивная строка и n, z=9 число 2 байта из comp_chislo
                    }
            if (j==524)
                    {
                    comp_chislo=metka.val[k1];  // число взять из определенной метки
                    comp_chislo%=100;
                    comp_set_adres(ctemp,0,7);  // фиктивная строка и n, z=7 число BCD из comp_chislo
                    }

                goto next_i;    // одного совпадения достаточно
                }
            }
        // нет ни одного совпадения - ??
        if (re<256) re++;
        comp_set_error(11,0,"");
        }


    if (j==525)
        {
        // =525 - адрес в виде числа в программе 3 байта, v0.21
        // записать число в программу
        for (k=0; k<metka.vsego; k++)
            {
            if ((metka.val[k]==i)&&(metka.use[k]==2))
                {
                // метка содержит ссылку на этот текущий адрес
                k1=comp_find_metka(metka.name[k]);
                if ((k1<0)||(k1>=metka.vsego))
                    {
                    // ошибка - метка не определена
                    comp_set_error(11,metka.stroka[k],metka.name[k]);
                    if (re<256) re++;
                    goto next_i;
                    }
                // иначе вставить в программу значение метки
                comp_stroka=metka.stroka[k];
                comp_adres=i;
                ia=metka.val[k1];   // число взять из определенной метки
                if (ia>999)
                    {
                    // ошибка - неверный адрес
                    comp_set_error(6,metka.stroka[k],metka.name[k]);
                    if (re<256) re++;
                    goto next_i;
                    }

                ia1=(ia/100)%10;
                comp_add_mkbufp(ia1);
                ia%=100;
                ia1=(ia/10)%10;
                comp_add_mkbufp(ia1);
                ia%=10;
                comp_add_mkbufp(ia%10);
                goto next_i;    // одного совпадения достаточно
                }
            }
        // нет ни одного совпадения - ??
        if (re<256) re++;
        comp_set_error(11,0,"");
        }

    if (j==526)
        {   // v0.23
        // =526 - метка из структуры mkmetka вставить по адресу 1 байт
        // с учётом ближайшей базы
        for (k=0; k<metka.vsego; k++)
            {
            if ((metka.val[k]==i)&&(metka.use[k]==2))
                {
                // метка содержит ссылку на этот текущий адрес
                k1=comp_find_metka(metka.name[k]);
                if ((k1<0)||(k1>=metka.vsego))
                    {
                    // ошибка - метка не определена
                    comp_set_error(11,metka.stroka[k],metka.name[k]);
                    if (re<256) re++;
                    goto next_i;
                    }
                // иначе вставить в программу значение метки
                comp_stroka=metka.stroka[k];
                comp_adres=i;

                comp_chislo=metka.val[k1];  // число взять из определенной метки

                // поиск ближайшей установленной базы
                if (mbase.vsego<=0) goto m1; // нет базы - не вычитать, считать базу равной 0.

                for (i1=mbase.vsego-1; i1>=0; i1--)
                    {
                    if (mbase.val[i1]<=comp_chislo)
                        {   // найдена первая не превышающая адреса база
                        comp_chislo-=mbase.val[i1];
                        if ((comp_chislo<0)||(comp_chislo>255))
                            {
                            comp_set_error(6,metka.stroka[k],metka.name[k]);
                            goto next_i;
                            }
                        goto m1;
                        }
                    }

m1:             comp_set_adres(ctemp,0,8);  // фиктивная строка и n, z=8 байт %256 из comp_chislo
                goto next_i;    // одного совпадения достаточно
                }
            }
        // нет ни одного совпадения - ??
        if (re<256) re++;
        comp_set_error(11,0,"");
        }

    if ((j==527)||(j==528))
        {
        // =527 - адрес в виде числа в программе 4 байта c декрементом
        // =528 - с инкрементом
        // v0.24 - записать число в программу
        for (k=0; k<metka.vsego; k++)
            {
            if ((metka.val[k]==i)&&(metka.use[k]==2))
                {
                // метка содержит ссылку на этот текущий адрес
                k1=comp_find_metka(metka.name[k]);
                if ((k1<0)||(k1>=metka.vsego))
                    {
                    // ошибка - метка не определена
                    comp_set_error(11,metka.stroka[k],metka.name[k]);
                    if (re<256) re++;
                    goto next_i;
                    }
                // иначе вставить в программу значение метки
                comp_stroka=metka.stroka[k];
                comp_adres=i;
                ia=metka.val[k1];   // число взять из определенной метки
                if (j==527) {
                            ia--;
                            if (ia<0)
                                {ia=0;
                                comp_set_error(14,metka.stroka[k],metka.name[k]);
                                if (re<256) re++;
                                goto next_i;
                                }
                            }
                if (j==528) {
                            ia++;
                            if (ia>9999)
                                {ia=9999;
                                comp_set_error(14,metka.stroka[k],metka.name[k]);
                                if (re<256) re++;
                                goto next_i;
                                }
                            }

                // записать 4 байта
                ia1=(ia/1000)%10;
                comp_add_mkbufp(ia1);
                ia%=1000;
                ia1=(ia/100)%10;
                comp_add_mkbufp(ia1);
                ia%=100;
                ia1=(ia/10)%10;
                comp_add_mkbufp(ia1);
                ia%=10;
                comp_add_mkbufp(ia%10);
                goto next_i;    // одного совпадения достаточно
                }
            }
        // нет ни одного совпадения - ??
        if (re<256) re++;
        comp_set_error(11,0,"");
        }

    if ((j==529)||(j==530))
        {
        //=529 (v0.25)  - вставить 2 байта из структуры mkmetka - HEX DW
        //=530 (v0.25)  - вставить 2 байта из структуры mkmetka по базе - HEX DWB

        for (k=0; k<metka.vsego; k++)
            {
            if ((metka.val[k]==i)&&(metka.use[k]==2))
                {
                // метка содержит ссылку на этот текущий адрес
                k1=comp_find_metka(metka.name[k]);
                if ((k1<0)||(k1>=metka.vsego))
                    {
                    // ошибка - метка не определена
                    comp_set_error(11,metka.stroka[k],metka.name[k]);
                    if (re<256) re++;
                    goto next_i;
                    }
                // иначе вставить в программу значение метки
                comp_stroka=metka.stroka[k];
                comp_adres=i;

            if (j==529)
                    {
                    comp_chislo=metka.val[k1];  // число взять из определенной метки
                    comp_set_adres(ctemp,0,10); // фиктивная строка и n, z=10 2б HEX число из comp_chislo
                    }

            if (j==530)
                    {
                    comp_chislo=metka.val[k1];  // число взять из определенной метки
                    // поиск ближайшей установленной базы
                    if (mbase.vsego<=0) goto m2; // нет базы - не вычитать, считать базу равной 0.

                    for (i1=mbase.vsego-1; i1>=0; i1--)
                        {
                        if (mbase.val[i1]<=comp_chislo)
                            {   // найдена первая не превышающая адреса база
                            comp_chislo-=mbase.val[i1];
                            if (comp_chislo<0)
                                {
                                comp_set_error(6,metka.stroka[k],metka.name[k]);
                                goto next_i;
                                }
                            goto m2;
                            }
                        }
m2:                 comp_set_adres(ctemp,0,10); // фиктивная строка и n, z=10 2б HEX число из comp_chislo
                    }
                goto next_i;    // одного совпадения достаточно
                }
            }
        // нет ни одного совпадения - ??
        if (re<256) re++;
        comp_set_error(11,0,"");
        }

next_i: ;

    }

comp_progmin=(progmin+1)/100;   // начало программы в страницах
if (progmax<=progmin) comp_proglen=0;
    else {
         // длина программы в страницах
        comp_proglen=(progmax/100)-comp_progmin+1;
         }



return(re);
}

/*------------------------------------------ COMPIL L -------------*/
void compil_l(void)
{
// подпрограмма копиляции исходного текста программы MKL

int i,j,k,ic;
int flag_metka, flag_probel, flag_err2, flag_equ, flag_end;
int flag_tz, flag_k;
long l;
//extern int gmkbufp[10020];
//extern int gcomp_proglen, gcomp_progmin, gflag_mkbufp;

    mkerror.vsego=0;
    metka.vsego=0;
    ident.vsego=0;
    flag_end=0;

    mbase.vsego=0; //v0.23 (v1.18 mk.exe)


// очистка
    for (l=0; l<10000; l++) mkbufp[l]=comp_const+512; // v0.21 для последующей проверки длины программы

    //text1[0]=0;
    //flag_text1=0;

    l=0;
    comp_adres=0; // текущий адрес в программе mkbufp
    comp_stroka=0;  // первая строка

    comp_flag_adres=0; // по умолчанию только десятичные адреса

ml00:
    if (flag_end!=0) goto ml2;

    ctemp[0]=0;

    // считать строку из входного файла в ctemp[]

    if (fp1!=NULL)
        {
        if (fgets(ctemp,256,fp1)==NULL) goto ml2;   // конец текста
            else goto ml01; // разбор строки

        }
    else goto end;


ml01:
    // В ctemp - очередная строка текста
    comp_stroka++;  // номер строки увеличить на 1
    comp_strupr(); // просто strupr(ctemp) не подойдет

    flag_metka=0;
    flag_equ=0;
    flag_tz=0;
    flag_k=0;

    j=strlen(ctemp);
    if (j==0) goto ml1;



    for (i=0; i<j; i++)
        {
        // проверить, есть ли метка адреса, псевдооператор EQU и заодно удалить комментарии
        if ((ctemp[i]==':')&&(flag_tz==0)&&(flag_k==0)) flag_metka=1;
        if ((ctemp[i]=='"')&&(flag_tz==0))
            {
            if (flag_k==0) flag_k=1; else flag_k=0;
            }
        if (ctemp[i]==';') {ctemp[i]=0; flag_tz=1;}
        if (ctemp[i]=='.')
            {
            if (i+3<j)
                {
                if ((ctemp[i+1]=='E')&&(ctemp[i+2]=='Q')&&(ctemp[i+3]=='U')) flag_equ=1;
                // if ((ctemp[i+1]=='E')&&(ctemp[i+2]=='N')&&(ctemp[i+3]=='D')) flag_end=1; удалено с v0.27
                }

            if (i+4<j)
                {
		// .ENDP с v0.27
                if ((ctemp[i+1]=='E')&&(ctemp[i+2]=='N')&&(ctemp[i+3]=='D')&&(ctemp[i+4]=='P')) flag_end=1;
                }

            }

        }


    // преобразовать строку в ctemp1: убрать первые пробелы
    // и сокращать цепочки пробелов до одного
    k=strlen(ctemp);
    j=0; // индекс в ctemp1
    ctemp1[0]=0;
    flag_probel=1; // начальные пробелы убрать
    flag_err2=0; // только по одной ошибке "недопустимый символ" на строку
    for (i=0; i<k; i++)
        {
        if ( ((ctemp[i]==' ')||(ctemp[i]=='\t')) && (flag_probel==0))
            {
            ctemp1[j]=' ';
            if (j<256) j++;
            ctemp1[j]=0;
            flag_probel=1;
            }
        if ((ctemp[i]!=' ')&&(ctemp[i]!='\t'))
            {
            flag_probel=0;
            ic=(int)ctemp[i];
            if (ic<0) ic+=256;
            if (ic<0) ic=0;

            if (ctemp[i]=='"')
                { // в кавычках могут быть любые символы
                if (flag_err2==0) flag_err2=1;
                    else { if (flag_err2==1) flag_err2=0;}

                }

            if ((ic>=128)&&(flag_err2==0))
                {
                // ошибка 2 - недопустимый символ в строке
                ctemp[0]=ic; ctemp[1]=0;
                comp_set_error(2,comp_stroka,ctemp);
                flag_err2=2;    // теперь не будет второго сообщения
                }

            ctemp1[j]=ctemp[i];
            if (j<256) j++;
            ctemp1[j]=0;
            }
        }

    strcpy(ctemp,ctemp1);   // скопировать полученную строку в ctemp

ml1:    // Здесь пробелы убраны, строка ctemp выровнена влево

    i=0; // i= указатель на первый не разобранный символ
    if (flag_metka!=0)
        {
        // считать метку и поместить ее в таблицу со значением текущего адреса
        i=comp_get_metka(ctemp);
        if (i>0)
            {
            comp_set_metka(comp_metka,comp_adres,1);
            }
        else i=-i; // иначе пропустить метку
        }


    if (flag_equ!=0)
        {
        // для этого псевдооператора сначала идет идентификатор,
        // поэтому и обработать его отдельно
        i=comp_get_ident(ctemp,0);
        if (i>0)
            {
            j=i;
            i=comp_get_equ(ctemp,j);
            if (i>j)
                {
                j=i;
                i=comp_get_chislo(ctemp,j);
                if ((comp_chislo>=0)&&(comp_chislo<10000))
                    {
                    comp_set_ident(comp_ident,comp_chislo,1);
                    // установлен идентификатор равный указанному в EQU числу
                    }
                else
                    {
                    sprintf(ctemptoa,"%d",comp_chislo);
                    comp_set_error(5,comp_stroka,ctemptoa);
                    // недопустимое значение идентификатора
                    }

                } else goto ml00; // ошибка в операторе EQU ??

            }
            else goto ml00; // ошибка в идентификаторе - разобрать след строку

        // i - указатель на последний разобранный элемент
        }



    k=strlen(ctemp);

ml11:   // i= указатель на первый не разобранный символ после метки или {идент .EQU число}
    if ((ctemp[i]==0)||(ctemp[i]=='\r')||(ctemp[i]=='\n')||(i>=k))
        {

        goto ml00;  // новая строка
        }

    j=comp_get_oper(ctemp,i);
    if (j>i) {i=j; goto ml11;} // повторить разбор оператора
    if (flag_end!=0) goto ml2;  // конец программы
    goto ml00;


ml2: // конец разбора текста программы

    comp_pass2();   //Проход 2 - проверить mkbufp и заменить метки на их значения

    if (fp2==NULL) goto end;
    // записать файл MKP
    if (comp_progmin<0) comp_progmin=0;
    if (comp_progmin>99) comp_progmin=99;
    if (comp_proglen<1) goto end;       // программы нет
    if (comp_progmin+comp_proglen>100) comp_proglen=100-comp_progmin;   // ограничить длину

    if (fputc((char)(comp_progmin%256),fp2)==EOF) goto end; // v0.17 - корректное преобразование в char
    for (l=100*comp_progmin; l<100*(comp_progmin+comp_proglen); l++)
        {
        // запись программы побайтно в MKP-файл
        i=mkbufp[l];
        if (fputc((char)(i%256),fp2)==EOF) goto end;    // v0.17 - исправлен порядок следования
        if (fputc((char)(i/256),fp2)==EOF) goto end;    // на младший - старший
        }

// вывести сообщения об ошибках

    for (l=0; l<mkerror.vsego; l++)
        {
        printf("%s: %d: %s: %s %s",fname[1], mkerror.stroka[l], mkerror.name[mkerror.n[l]],mkerror.sss[l],mes[0]);
        }

    if (kluch_i!=0) // v0.21
        {
        printf("%s %d %s",mes[9],comp_progmin,mes[0]);
        printf("%s %d %s",mes[10],comp_proglen,mes[0]);
        if (mkerror.vsego==0) printf(mes[11]);
        }

end:
    if (mkerror.vsego > 0)
    {
        compilation_error = 1;
    }
    return;
}


/*----------------------------------------------------*/
int comp_get_dd(char *sss, int n, int z)
{

int i,i1,j,k;
int re=0;
// v0.18
// считать из строки и записать в программу ряд чисел, меток или идентификаторов
// через запятую отсюда и до конца строки в десятичном виде

// z = 0 - 2 байта, =1 - мл часть =2 - ст. часть числа - все в десятичном виде

i=n;
k=strlen(sss);


m1:
        j=comp_get_adres(sss,i,3);  // считать число, метку или т.п.
        if ((j>i)&&(comp_chislo>=0)&&(comp_chislo<10000))
                {
                if ((z==0)||(z==2))
                    {
                    if ((comp_adres>=0)&&(comp_adres<10000))
                        {
                        comp_add_mkbufp(comp_chislo/100+256);
                        }
                    else {
                        sprintf(ctemptoa,"%d",comp_adres);
                        comp_set_error(6,comp_stroka,ctemptoa);
                         }
                    }

                if ((z==0)||(z==1))
                    {
                    if ((comp_adres>=0)&&(comp_adres<10000))
                        {
                        comp_add_mkbufp(comp_chislo%100+256);
                        }
                    else {  sprintf(ctemptoa,"%d",comp_adres);
                        comp_set_error(6,comp_stroka,ctemptoa);
                         }
                    }

                }
        else
                {
                if ((j>i)&&(z>=0)&&(z<=2))
                    {
                        // метка уже в структуре после comp_get_adres
                    if (z==0)
                        {
                        comp_add_mkbufp(comp_const+523);
                        comp_add_mkbufp(comp_const+522);
                        }
                    if (z==1)
                        {
                        comp_add_mkbufp(comp_const+522);
                        }

                    if (z==2)
                        {
                        comp_add_mkbufp(comp_const+521);    // метка - ст часть
                        }


                    }
                else
                    {sprintf(ctemptoa,"%d",comp_chislo);
                    comp_set_error(8,comp_stroka,ctemptoa); // неверное значение
                    }
                }


    if ((j<=0)||(j<=n))
        {
        re=j;       // ошибка или конец
        goto end;
        }

    for (i1=j; i1<k; i1++)
        {
        if (sss[i1]==',')
            {
            i=i1+1;
            goto m1;    // повтор разбора числа
            }
        if ((sss[i1]==';')||(sss[i1]=='\r')||(sss[i1]=='\n')||(sss[i1]==0))
            {
            re=0;
            goto end;
            }
        }


re=0;
end:
return(re);
}

/*---------------------- v0.25 --------------------------*/
int comp_get_dw(char *sss, int n, int z)
{
int i,i1,i2,j,k;
char c;
int m;
int re=0;
// 14.11.11 - v0.25
// считать из строки и записать в программу ряд чисел, меток или идентификаторов
// через запятую отсюда и до конца строки в шестнадцатиричном виде по 2 байта
// z = 0 - число или адрес DW, =1 - адрес по базе DWB

i=n;
k=strlen(sss);


m1:
        j=comp_get_adres(sss,i,3);  // считать число, метку или т.п.
        if ((j>i)&&(comp_chislo>=0))
                {
                if (z==0)
                    {
                    if ((comp_adres>=0)&&(comp_adres<9999))
                        {
                        comp_add_mkbufp((comp_chislo/256)%256+256);
                        comp_add_mkbufp(comp_chislo%256+256);
                        }
                    else {
                        sprintf(ctemptoa,"%d",comp_adres);
                        comp_set_error(6,comp_stroka,ctemptoa);
                         }
                    }

                if (z==1)
                    {
                    i2=comp_chislo-comp_base;   // вычитание базы
                    if ((i2>=0)&&(comp_adres>=0)&&(comp_adres<9999))
                        {
                        comp_add_mkbufp((i2/256)%256+256);
                        comp_add_mkbufp(i2%256+256);
                        }

                    else
                        {
                        sprintf(ctemptoa,"%d",comp_adres);
                        comp_set_error(8,comp_stroka,ctemptoa);
                        }
                    }
                }
        else
                {
                if ((j>i)&&(z>=0)&&(z<=1))
                    {
                        // метка уже в структуре после comp_get_adres
                    if (z==0)
                        {
                        comp_add_mkbufp(comp_const+529);
                        comp_add_mkbufp(comp_const+512);
                        }
                    if (z==1)
                        {
                        comp_add_mkbufp(comp_const+530);
                        comp_add_mkbufp(comp_const+512);
                        }

                    }
                else
                    {
                    sprintf(ctemptoa,"%d",comp_chislo);
                    comp_set_error(8,comp_stroka,ctemptoa); // неверное значение
                    }
                }


    if ((j<=0)||(j<=n))
        {
        re=j;       // ошибка или конец
        goto end;
        }

    // v1.18 - проверка наличия чего-либо до конца строки или комментария
    m=0;
    for (i1=j; i1<k; i1++)
        {
        c=sss[i1];
        if ((c==';')||(c=='\r')||(c=='\n')||(c==0)) goto m2;
        if ((c!=' ')&&(c!=',')&&(c!='\t')) m=1;
        }
m2: if (m==0) goto end; // нет в конце ничего

    for (i1=j; i1<k; i1++)
        {
        if (sss[i1]==',')
            {
            i=i1+1;
            goto m1;    // повтор разбора числа
            }
        if ((sss[i1]==';')||(sss[i1]=='\r')||(sss[i1]=='\n')||(sss[i1]==0))
            {
            re=0;
            goto end;
            }
        }


re=0;
end:
return(re);
}


////////////////////////////////////////////////////////////////////////////////////


/*================================================ Вывод листинга TXT fp3 =*/
void buf2txt()
{

int i,j,ic;
long l,l1;
char ics[4];    // v0.21

    // вычислить контрольную сумму
    comp_progks1=0;
    comp_progks2=0;

    for (i=0; i<100*comp_proglen; i++)
        {
        ic=(int)(mkbufp[100*comp_progmin+i] % 256); // мл часть только
        if (ic<0) ic+=256;
        comp_progks1+=ic;
        comp_progks2^=ic;
        }


    // простой дизассемблер
    l=0; // текущий адрес относительно начала файла
    l1=100*comp_progmin; // текущий адрес относительно начала файла в памяти
    j=0; // 0-интерпретировать байт как команду, 1 и 2 - как адрес или номер регистра.


mp0:    // вывести адрес и шестнадцатиричное значение байта
    ic=(int)mkbufp[l1];
        if (ic<0) ic+=256;
        if (ic<0) ic=0;

    sprintf(ctemp,"%04d \t%02Xh \t",l1,ic);
    strcpy(temp,ctemp);

    // мнемоника ic

    if ((ic>=512)&&(ic<768))
        {
        // TEXT, v0.22
        ic-=512;
        if ( ((ic>=32)&&(ic<176))  || ((ic>=224)&&(ic<242)) )
            {
            ics[0]=ic; ics[1]=0;
            sprintf(ctemp,"'%s'",ics);
            }
            else sprintf(ctemp,"%02Xh ",ic);

        strcat(temp, ctemp);
        goto mp01;
        }

    if ((ic>=256)&&(ic<512))
        {
        // HEX
        sprintf(ctemp,"%02Xh",ic%256);
        strcat(temp, ctemp);
        goto mp01;
        }

    if (ic<256)
        {
        if (j==0)
            {
            // команда
            strcat(temp, mkkom.name[ic]);
            j=mkkom.par[ic];
            if (j<0) j=0;
            if (j==3)j=1;
            if (j>=4) j=2;
            // адрес или регистр если он есть
            if (j>0)
                {
                ic=(int)mkbufp[l1+1]%256;
                    if (ic<0) ic+=256;
                    if (ic<0) ic=0;
                sprintf(ctemp," %02X",ic);
                strcat(temp, ctemp);
                }
            if (j>1)
                {
                ic=(int)mkbufp[l1+2]%256;
                    if (ic<0) ic+=256;
                    if (ic<0) ic=0;

                sprintf(ctemp," %02X",ic);
                strcat(temp, ctemp);
                }
            }
        else
            {
            // адрес или регистр - нет мнемоники
            if (j>0) j--;
            if (j<0) j=0;
            }
        goto mp01;
    } // end ic<256

    if (ic>=comp_const)
        {
        // ошибка v0.21
        ic%=256;
        sprintf(ctemp,"%Xh ?",ic);
        strcat(temp, ctemp);
        }
mp01:
    strcat(temp, " \t;");
    // символ
    ic=(int)mkbufp[l1]%256;
    if (ic<0) ic+=256;
    if (ic<0) ic=0;
    ctemp[0]=(char)ic;
    ctemp[1]=0;
    mkstrcpy(ctemp1,ctemp,mkiocharset,0);
    strcat(temp,ctemp1);

mp1:    strcat(temp,mes[0]);

    if (fputs(temp,fp3)==EOF) goto endp;

    l++;
    l1++;

    if (l<100*comp_proglen) goto mp0;

    // Добавить комментарии с КС
    sprintf(temp,"; %s ADD %d %s",mes[12],comp_progks1,mes[0]);
    if (fputs(temp,fp3)==EOF) goto endp;
    sprintf(temp,"; %s XOR %d %s",mes[12],comp_progks2,mes[0]);
    if (fputs(temp,fp3)==EOF) goto endp;

endp:   return;
}

/*------------------------------------------------------------------- HTML ------*/
void buf2htm()
{
// запись в файл sp4 в формате HTML
int i,j,ic;
long l,l1;
char ics[4];    //v0.21

    // вычислить контрольную сумму
    comp_progks1=0;
    comp_progks2=0;

    for (i=0; i<100*comp_proglen; i++)
        {
        ic=(int)(mkbufp[100*comp_progmin+i] % 256); // мл часть только
        if (ic<0) ic+=256;
        comp_progks1+=ic;
        comp_progks2^=ic;   // исправлено в v0.24
        }

    // простой дизассемблер
    l=0; // текущий адрес относительно начала файла
    l1=100*comp_progmin; // текущий адрес относительно начала файла в памяти
    j=0; // 0-интерпретировать байт как команду, 1 и 2 - как адрес или номер регистра.


    strcpy(temp, "<HTML>\r\n<HEAD><META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=windows-1251\"></HEAD>\r\n");
    if (fputs(temp,fp4)==EOF) goto endp;
    strcpy(temp,"<BODY>\r\n<CENTER>\r\n<TABLE BORDER=1 WIDTH=80%><TBODY>\r\n");
    if (fputs(temp,fp4)==EOF) goto endp;
    strcpy(temp,"<TR>\r\n<TD>&nbsp;</TD>\r\n<TD>0</TD>\r\n<TD>1</TD>\r\n<TD>2</TD>\r\n<TD>3</TD>\r\n");
    if (fputs(temp,fp4)==EOF) goto endp;
    strcpy(temp,"<TD>4</TD>\r\n<TD>5</TD>\r\n<TD>6</TD>\r\n<TD>7</TD>\r\n<TD>8</TD>\r\n<TD>9</TD>\r\n</TR>\r\n\r\n");
    if (fputs(temp,fp4)==EOF) goto endp;

mph0:   if ((l%10)==0)
        {
        sprintf(temp,"<TR>\r\n<TD>%04d</TD>\r\n",l1);
        }

    strcat(temp, "<TD>");
    // мнемоника ic
    ic=(int)mkbufp[l1];
        if (ic<0) ic+=256;
        if (ic<0) ic=0;

    if ((ic>=256)&&(ic<512))
        {
        // HEX
        ic-=256;
        sprintf(ctemp,"%02Xh",ic);
        strcat(temp,ctemp);
        goto mph01;
        }

    if ((ic>=512)&&(ic<768))
        {
        // TEXT v0.21
        ic-=512;
        if ( ((ic>=32)&&(ic<176))  || ((ic>=224)&&(ic<242)) )   //v0.22
            {
            ics[0]=ic; ics[1]=0;
            sprintf(ctemp,"%02Xh '%s'",ic,ics);
            }
            else sprintf(ctemp,"%02Xh",ic); // убран пробел в v0.24
        mkstrcpy(ctemp1,ctemp,1,0);     // преобразовать ctemp из 866 в 1251
        strcat(temp,ctemp1);
        goto mph01;
        }


    if (ic<256)
        { // обычный байт
        if  (j==0)
            {
            // команда
            strcat(temp, mkkom.name1[ic]); // name1 - HTML формат кодировка cp1251
            j=mkkom.par[ic];
            if (j<0) j=0;
            if (j==3) j=1;
            if (j>=4) j=2;
            }
        else
            {
            // адрес или регистр если он есть
            sprintf(ctemp,"%02X",ic);
            strcat(temp, ctemp);
            if (j>0) j--;
            if (j<0) j=0;
            }
        goto mph01;
        }

    if (ic>=comp_const)
        { // ошибка v0.21
        ic%=256;
        sprintf(ctemp,"%02Xh ?",ic);
        strcat(temp, ctemp);
        }

mph01:
    strcat(temp, "</TD>\r\n");

    l++;
    l1++;

    if ((l%10)==0)
        {strcat(temp, "</TR>\r\n\r\n");
        if (fputs(temp,fp4)==EOF) goto endp;
        }

    if (l<100*comp_proglen) goto mph0;

    strcpy(temp, "</TBODY></TABLE>\r\n\r\n");
    if (fputs(temp,fp4)==EOF) goto endp;

    // добавить к файлу HTML сообщение о контрольной сумме
    // Добавить комментарии с КС
    sprintf(temp,"<P>%s ADD %d\r\n</P>\r\n",mes[12],comp_progks1);  // убран пробел в v0.24
    if (fputs(temp,fp4)==EOF) goto endp;
    sprintf(temp,"<P>%s XOR %d\r\n</P>\r\n",mes[12],comp_progks2);  // убран пробел в v0.24
    if (fputs(temp,fp4)==EOF) goto endp;
    strcpy(temp, "</BODY></HTML>\r\n");
    if (fputs(temp,fp4)==EOF) goto endp;


endp:   return;

}
/*======================================================================== INIT ============*/


/*============================================ Инициализация таблицы команд */
int init_mkkom()
{
int re=0;
int i;

if (mkkom.init!=0) {re=1; goto end;}

mkkom.init=1;


for (i=0; i<256; i++)
    {
    mkkom.len[i]=0;
    mkkom.par[i]=0;
    strcpy(mkkom.name[i],"");
    strcpy(mkkom.name1[i],"");
    strcpy(mkkom.name2[i],"");
    }

 strcpy(mkkom.name[0],"0"); strcpy(mkkom.name1[0],"0");
strcpy(mkkom.name2[0],"0");
 strcpy(mkkom.name[1],"1");  strcpy(mkkom.name1[1],"1");
strcpy(mkkom.name2[1],"1");
 strcpy(mkkom.name[2],"2"); strcpy(mkkom.name1[2],"2");
strcpy(mkkom.name2[2],"2");
 strcpy(mkkom.name[3],"3"); strcpy(mkkom.name1[3],"3");
strcpy(mkkom.name2[3],"3");
 strcpy(mkkom.name[4],"4"); strcpy(mkkom.name1[4],"4");
strcpy(mkkom.name2[4],"4");
 strcpy(mkkom.name[5],"5"); strcpy(mkkom.name1[5],"5");
strcpy(mkkom.name2[5],"5");
 strcpy(mkkom.name[6],"6"); strcpy(mkkom.name1[6],"6");
strcpy(mkkom.name2[6],"6");
 strcpy(mkkom.name[7],"7"); strcpy(mkkom.name1[7],"7");
strcpy(mkkom.name2[7],"7");
 strcpy(mkkom.name[8],"8"); strcpy(mkkom.name1[8],"8");
strcpy(mkkom.name2[8],"8");
 strcpy(mkkom.name[9],"9"); strcpy(mkkom.name1[9],"9");
strcpy(mkkom.name2[9],"9");
 strcpy(mkkom.name[10],","); strcpy(mkkom.name1[10],",");
strcpy(mkkom.name2[10],",");
 strcpy(mkkom.name[11],"+/-");  strcpy(mkkom.name1[11],"/-/");
strcpy(mkkom.name2[11],"+/-");
 strcpy(mkkom.name[12],"EE");   strcpy(mkkom.name1[12],"ВП");// ВП
strcpy(mkkom.name2[12],"EE");
 strcpy(mkkom.name[13],"CX");   strcpy(mkkom.name1[13],"Cx"); // Cx
strcpy(mkkom.name2[13],"CX");
 strcpy(mkkom.name[14],"ENT");  strcpy(mkkom.name1[14],"B&uarr;"); // B^
strcpy(mkkom.name2[14],"ENT");
 strcpy(mkkom.name[15],"F ANS"); strcpy(mkkom.name1[15],"F Bx"); // F Bx
strcpy(mkkom.name2[15],"FANS");

 strcpy(mkkom.name[16],"+");        strcpy(mkkom.name1[16],"+");
strcpy(mkkom.name2[16],"+");
 strcpy(mkkom.name[17],"-");        strcpy(mkkom.name1[17],"-");
strcpy(mkkom.name2[17],"-");
 strcpy(mkkom.name[18],"*");        strcpy(mkkom.name1[18],"&times;");
strcpy(mkkom.name2[18],"*");
 strcpy(mkkom.name[19],"/");        strcpy(mkkom.name1[19],"&divide;");
strcpy(mkkom.name2[19],"/");
 strcpy(mkkom.name[20],"<->");  strcpy(mkkom.name1[20],"&harr;");
strcpy(mkkom.name2[20],"<->");
 strcpy(mkkom.name[21],"F 10^X");   strcpy(mkkom.name1[21],"F 10<sup>x</sup>");
strcpy(mkkom.name2[21],"F10^X");
 strcpy(mkkom.name[22],"F EXP");        strcpy(mkkom.name1[22],"F e<sup>x</sup>");
strcpy(mkkom.name2[22],"FEXP");
 strcpy(mkkom.name[23],"F LG");     strcpy(mkkom.name1[23],"F lg");
strcpy(mkkom.name2[23],"FLG");
 strcpy(mkkom.name[24],"F LN");     strcpy(mkkom.name1[24],"F ln");
strcpy(mkkom.name2[24],"FLN");
 strcpy(mkkom.name[25],"F ARCSIN"); strcpy(mkkom.name1[25],"F arcsin");
strcpy(mkkom.name2[25],"FARCSIN");
 strcpy(mkkom.name[26],"F ARCCOS"); strcpy(mkkom.name1[26],"F arccos");
strcpy(mkkom.name2[26],"FARCCOS");
 strcpy(mkkom.name[27],"F ARCTG");  strcpy(mkkom.name1[27],"F arctg");
strcpy(mkkom.name2[27],"FARCTG");
 strcpy(mkkom.name[28],"F SIN");        strcpy(mkkom.name1[28],"F sin");
strcpy(mkkom.name2[28],"FSIN");
 strcpy(mkkom.name[29],"F COS");        strcpy(mkkom.name1[29],"F cos");
strcpy(mkkom.name2[29],"FCOS");
 strcpy(mkkom.name[30],"F TG");     strcpy(mkkom.name1[30],"F tg");
strcpy(mkkom.name2[30],"FTG");
 strcpy(mkkom.name[31],".DB 1Fh");      strcpy(mkkom.name1[31],"1Fh");
strcpy(mkkom.name2[31],"?");

 strcpy(mkkom.name[32],"F PI");     strcpy(mkkom.name1[32],"F &pi;");
strcpy(mkkom.name2[32],"FPI");
 strcpy(mkkom.name[33],"F SQRT");   strcpy(mkkom.name1[33],"F &radic;");
strcpy(mkkom.name2[33],"FSQRT");
 strcpy(mkkom.name[34],"F X^2");        strcpy(mkkom.name1[34],"F x<sup>2</sup>");
strcpy(mkkom.name2[34],"FX^2");
 strcpy(mkkom.name[35],"F 1/X");        strcpy(mkkom.name1[35],"F 1/x");
strcpy(mkkom.name2[35],"F1/X");
 strcpy(mkkom.name[36],"F X^Y");        strcpy(mkkom.name1[36],"F x<sup>y</sup>");
strcpy(mkkom.name2[36],"FX^Y");
 strcpy(mkkom.name[37],"F R");      strcpy(mkkom.name1[37],"F O");
strcpy(mkkom.name2[37],"FR");
 strcpy(mkkom.name[38],"K M->D");   strcpy(mkkom.name1[38],"K М&rarr;Г");// К М-Г
strcpy(mkkom.name2[38],"KM->D");
 strcpy(mkkom.name[39],"K -");      strcpy(mkkom.name1[39],"K -");
strcpy(mkkom.name2[39],"K-");
 strcpy(mkkom.name[40],"K PRGM");   strcpy(mkkom.name1[40],"K ИПРГ");// К ИПРГ
strcpy(mkkom.name2[40],"KPRGM");
 strcpy(mkkom.name[41],"K /");      strcpy(mkkom.name1[41],"K &divide;");
strcpy(mkkom.name2[41],"K/");
 strcpy(mkkom.name[42],"K MS->D");  strcpy(mkkom.name1[42],"K МС&rarr;Г");
strcpy(mkkom.name2[42],"KMS->D");
 strcpy(mkkom.name[43],".DB 2Bh");      strcpy(mkkom.name1[43],"2Bh");
strcpy(mkkom.name2[43],"?");
 strcpy(mkkom.name[44],".DB 2Ch");      strcpy(mkkom.name1[44],"2Ch");
strcpy(mkkom.name2[44],"?");
 strcpy(mkkom.name[45],".DB 2Dh");      strcpy(mkkom.name1[45],"2Dh");
strcpy(mkkom.name2[45],"?");
 strcpy(mkkom.name[46],".DB 2Eh");      strcpy(mkkom.name1[46],"2Eh");
strcpy(mkkom.name2[46],"?");
 strcpy(mkkom.name[47],".DB 2Fh");      strcpy(mkkom.name1[47],"2Fh");
strcpy(mkkom.name2[47],"?");

 strcpy(mkkom.name[48],"K D->MS");  strcpy(mkkom.name1[48],"K Г&rarr;МС");
strcpy(mkkom.name2[48],"KD->MS");
 strcpy(mkkom.name[49],"K ABS");        strcpy(mkkom.name1[49],"K |x|");// K|x|
strcpy(mkkom.name2[49],"KABS");
 strcpy(mkkom.name[50],"K SGN");        strcpy(mkkom.name1[50],"K ЗН");// К ЗН
strcpy(mkkom.name2[50],"KSGN");
 strcpy(mkkom.name[51],"K D->M");   strcpy(mkkom.name1[51],"K Г&rarr;М");
strcpy(mkkom.name2[51],"KD->M");
 strcpy(mkkom.name[52],"K INT");        strcpy(mkkom.name1[52],"K [x]");// K[x]
strcpy(mkkom.name2[52],"KINT");
 strcpy(mkkom.name[53],"K FRAC");   strcpy(mkkom.name1[53],"K {x}");// K{x}
strcpy(mkkom.name2[53],"KFRAC");
 strcpy(mkkom.name[54],"K MAX");        strcpy(mkkom.name1[54],"K max");
strcpy(mkkom.name2[54],"KMAX");
 strcpy(mkkom.name[55],"K AND");        strcpy(mkkom.name1[55],"K AND");
strcpy(mkkom.name2[55],"KAND");
 strcpy(mkkom.name[56],"K OR");     strcpy(mkkom.name1[56],"K OR");
strcpy(mkkom.name2[56],"KOR");
 strcpy(mkkom.name[57],"K XOR");        strcpy(mkkom.name1[57],"K XOR");
strcpy(mkkom.name2[57],"KXOR");
 strcpy(mkkom.name[58],"K NOT");        strcpy(mkkom.name1[58],"K NOT");
strcpy(mkkom.name2[58],"KNOT");
 strcpy(mkkom.name[59],"K RAN");        strcpy(mkkom.name1[59],"K СЧ");// KСЧ
strcpy(mkkom.name2[59],"KRAN");
 strcpy(mkkom.name[60],".DB 3Ch");      strcpy(mkkom.name1[60],"3Ch");
strcpy(mkkom.name2[60],"?");
 strcpy(mkkom.name[61],".DB 3Dh");      strcpy(mkkom.name1[61],"3Dh");
strcpy(mkkom.name2[61],"?");
 strcpy(mkkom.name[62],".DB 3Eh");      strcpy(mkkom.name1[62],"3Eh");
strcpy(mkkom.name2[62],"?");
 strcpy(mkkom.name[63],".DB 3Fh");      strcpy(mkkom.name1[63],"3Fh");
strcpy(mkkom.name2[63],"?");

 strcpy(mkkom.name[64],"M 0");      strcpy(mkkom.name1[64],"П 0");
strcpy(mkkom.name2[64],"M0");
 strcpy(mkkom.name[65],"M 1");      strcpy(mkkom.name1[65],"П 1");
strcpy(mkkom.name2[65],"M1");
 strcpy(mkkom.name[66],"M 2");      strcpy(mkkom.name1[66],"П 2");
strcpy(mkkom.name2[66],"M2");
 strcpy(mkkom.name[67],"M 3");      strcpy(mkkom.name1[67],"П 3");
strcpy(mkkom.name2[67],"M3");
 strcpy(mkkom.name[68],"M 4");      strcpy(mkkom.name1[68],"П 4");
strcpy(mkkom.name2[68],"M4");
 strcpy(mkkom.name[69],"M 5");      strcpy(mkkom.name1[69],"П 5");
strcpy(mkkom.name2[69],"M5");
 strcpy(mkkom.name[70],"M 6");      strcpy(mkkom.name1[70],"П 6");
strcpy(mkkom.name2[70],"M6");
 strcpy(mkkom.name[71],"M 7");      strcpy(mkkom.name1[71],"П 7");
strcpy(mkkom.name2[71],"M7");
 strcpy(mkkom.name[72],"M 8");      strcpy(mkkom.name1[72],"П 8");
strcpy(mkkom.name2[72],"M8");
 strcpy(mkkom.name[73],"M 9");      strcpy(mkkom.name1[73],"П 9");
strcpy(mkkom.name2[73],"M9");
 strcpy(mkkom.name[74],"M A");      strcpy(mkkom.name1[74],"П A");
strcpy(mkkom.name2[74],"MA");
 strcpy(mkkom.name[75],"M B");      strcpy(mkkom.name1[75],"П B");
strcpy(mkkom.name2[75],"MB");
 strcpy(mkkom.name[76],"M C");      strcpy(mkkom.name1[76],"П C");
strcpy(mkkom.name2[76],"MC");
 strcpy(mkkom.name[77],"M D");      strcpy(mkkom.name1[77],"П D");
strcpy(mkkom.name2[77],"MD");
 strcpy(mkkom.name[78],"M E");      strcpy(mkkom.name1[78],"П E");
strcpy(mkkom.name2[78],"ME");
 strcpy(mkkom.name[79],"P M");      strcpy(mkkom.name1[79],"P П"); mkkom.len[79]=1;
strcpy(mkkom.name2[79],"PM");       mkkom.par[79]=3;

 strcpy(mkkom.name[80],"R/S");      strcpy(mkkom.name1[80],"С/П");// С/П
strcpy(mkkom.name2[80],"R/S");
 strcpy(mkkom.name[81],"GOTO");     strcpy(mkkom.name1[81],"БП"); mkkom.len[81]=1; // БП
strcpy(mkkom.name2[81],"GOTO");     mkkom.par[81]=1;
 strcpy(mkkom.name[82],"RTN");      strcpy(mkkom.name1[82],"В/О");//В/О
strcpy(mkkom.name2[82],"RTN");
 strcpy(mkkom.name[83],"GSB");      strcpy(mkkom.name1[83],"ПП"); mkkom.len[83]=1; // ПП
strcpy(mkkom.name2[83],"GSB");      mkkom.par[83]=1;
 strcpy(mkkom.name[84],"K NOP");        strcpy(mkkom.name1[84],"K НОП");
strcpy(mkkom.name2[84],"KNOP");
 strcpy(mkkom.name[85],"K SCR");        strcpy(mkkom.name1[85],"K ЭКР"); // KЭКР
strcpy(mkkom.name2[85],"KSCR");
 strcpy(mkkom.name[86],"K GRPH");   strcpy(mkkom.name1[86],"K ГРФ");// KГРФ
strcpy(mkkom.name2[86],"KGRPH");
 strcpy(mkkom.name[87],"F X!=0");   strcpy(mkkom.name1[87],"F x&ne;0"); mkkom.len[87]=1;
strcpy(mkkom.name2[87],"FX!=0");    mkkom.par[87]=1;
 strcpy(mkkom.name[88],"F L2");     strcpy(mkkom.name1[88],"F L2"); mkkom.len[88]=1;
strcpy(mkkom.name2[88],"FL2");      mkkom.par[88]=1;
 strcpy(mkkom.name[89],"F X>=0");   strcpy(mkkom.name1[89],"F x&ge;0"); mkkom.len[89]=1;
strcpy(mkkom.name2[89],"FX>=0");    mkkom.par[89]=1;
 strcpy(mkkom.name[90],"F L3");     strcpy(mkkom.name1[90],"F L3"); mkkom.len[90]=1;
strcpy(mkkom.name2[90],"FL3");      mkkom.par[90]=1;
 strcpy(mkkom.name[91],"F L1");     strcpy(mkkom.name1[91],"F L1"); mkkom.len[91]=1;
strcpy(mkkom.name2[91],"FL1");      mkkom.par[91]=1;
 strcpy(mkkom.name[92],"F X<0");        strcpy(mkkom.name1[92],"F x&lt;0"); mkkom.len[92]=1;
strcpy(mkkom.name2[92],"FX<0");     mkkom.par[92]=1;
 strcpy(mkkom.name[93],"F L0");     strcpy(mkkom.name1[93],"F L0"); mkkom.len[93]=1;
strcpy(mkkom.name2[93],"FL0");      mkkom.par[93]=1;
 strcpy(mkkom.name[94],"F X=0");        strcpy(mkkom.name1[94],"F x=0"); mkkom.len[94]=1;
strcpy(mkkom.name2[94],"FX=0");     mkkom.par[94]=1;
 strcpy(mkkom.name[95],".DB 5Fh");
strcpy(mkkom.name2[95],"?");

 strcpy(mkkom.name[96],"RM 0");     strcpy(mkkom.name1[96],"ИП 0");
strcpy(mkkom.name2[96],"RM0");
 strcpy(mkkom.name[97],"RM 1");     strcpy(mkkom.name1[97],"ИП 1");
strcpy(mkkom.name2[97],"RM1");
 strcpy(mkkom.name[98],"RM 2");     strcpy(mkkom.name1[98],"ИП 2");
strcpy(mkkom.name2[98],"RM2");
 strcpy(mkkom.name[99],"RM 3");     strcpy(mkkom.name1[99],"ИП 3");
strcpy(mkkom.name2[99],"RM3");
 strcpy(mkkom.name[100],"RM 4");        strcpy(mkkom.name1[100],"ИП 4");
strcpy(mkkom.name2[100],"RM4");
 strcpy(mkkom.name[101],"RM 5");        strcpy(mkkom.name1[101],"ИП 5");
strcpy(mkkom.name2[101],"RM5");
 strcpy(mkkom.name[102],"RM 6");        strcpy(mkkom.name1[102],"ИП 6");
strcpy(mkkom.name2[102],"RM6");
 strcpy(mkkom.name[103],"RM 7");        strcpy(mkkom.name1[103],"ИП 7");
strcpy(mkkom.name2[103],"RM7");
 strcpy(mkkom.name[104],"RM 8");        strcpy(mkkom.name1[104],"ИП 8");
strcpy(mkkom.name2[104],"RM8");
 strcpy(mkkom.name[105],"RM 9");        strcpy(mkkom.name1[105],"ИП 9");
strcpy(mkkom.name2[105],"RM9");
 strcpy(mkkom.name[106],"RM A");        strcpy(mkkom.name1[106],"ИП A");
strcpy(mkkom.name2[106],"RMA");
 strcpy(mkkom.name[107],"RM B");        strcpy(mkkom.name1[107],"ИП B");
strcpy(mkkom.name2[107],"RMB");
 strcpy(mkkom.name[108],"RM C");        strcpy(mkkom.name1[108],"ИП C");
strcpy(mkkom.name2[108],"RMC");
 strcpy(mkkom.name[109],"RM D");        strcpy(mkkom.name1[109],"ИП D");
strcpy(mkkom.name2[109],"RMD");
 strcpy(mkkom.name[110],"RM E");        strcpy(mkkom.name1[110],"ИП E");
strcpy(mkkom.name2[110],"RME");
 strcpy(mkkom.name[111],"P RM");        strcpy(mkkom.name1[111],"P ИП"); mkkom.len[111]=1;
strcpy(mkkom.name2[111],"PRM");     mkkom.par[111]=3;

 strcpy(mkkom.name[112],"K X!=0 0");    strcpy(mkkom.name1[112],"K x&ne;0 0");
strcpy(mkkom.name2[112],"KX!=00");
 strcpy(mkkom.name[113],"K X!=0 1");    strcpy(mkkom.name1[113],"K x&ne;0 1");
strcpy(mkkom.name2[113],"KX!=01");
 strcpy(mkkom.name[114],"K X!=0 2");    strcpy(mkkom.name1[114],"K x&ne;0 2");
strcpy(mkkom.name2[114],"KX!=02");
 strcpy(mkkom.name[115],"K X!=0 3");    strcpy(mkkom.name1[115],"K x&ne;0 3");
strcpy(mkkom.name2[115],"KX!=03");
 strcpy(mkkom.name[116],"K X!=0 4");    strcpy(mkkom.name1[116],"K x&ne;0 4");
strcpy(mkkom.name2[116],"KX!=04");
 strcpy(mkkom.name[117],"K X!=0 5");    strcpy(mkkom.name1[117],"K x&ne;0 5");
strcpy(mkkom.name2[117],"KX!=05");
 strcpy(mkkom.name[118],"K X!=0 6");    strcpy(mkkom.name1[118],"K x&ne;0 6");
strcpy(mkkom.name2[118],"KX!=06");
 strcpy(mkkom.name[119],"K X!=0 7");    strcpy(mkkom.name1[119],"K x&ne;0 7");
strcpy(mkkom.name2[119],"KX!=07");
 strcpy(mkkom.name[120],"K X!=0 8");    strcpy(mkkom.name1[120],"K x&ne;0 8");
strcpy(mkkom.name2[120],"KX!=08");
 strcpy(mkkom.name[121],"K X!=0 9");    strcpy(mkkom.name1[121],"K x&ne;0 9");
strcpy(mkkom.name2[121],"KX!=09");
 strcpy(mkkom.name[122],"K X!=0 A");    strcpy(mkkom.name1[122],"K x&ne;0 A");
strcpy(mkkom.name2[122],"KX!=0A");
 strcpy(mkkom.name[123],"K X!=0 B");    strcpy(mkkom.name1[123],"K x&ne;0 B");
strcpy(mkkom.name2[123],"KX!=0B");
 strcpy(mkkom.name[124],"K X!=0 C");    strcpy(mkkom.name1[124],"K x&ne;0 C");
strcpy(mkkom.name2[124],"KX!=0C");
 strcpy(mkkom.name[125],"K X!=0 D");    strcpy(mkkom.name1[125],"K x&ne;0 D");
strcpy(mkkom.name2[125],"KX!=0D");
 strcpy(mkkom.name[126],"K X!=0 E");    strcpy(mkkom.name1[126],"K x&ne;0 E");
strcpy(mkkom.name2[126],"KX!=0E");
 strcpy(mkkom.name[127],"PK X!=0");     strcpy(mkkom.name1[127],"PK x&ne;0"); mkkom.len[127]=1;
strcpy(mkkom.name2[127],"PKX!=0");  mkkom.par[127]=3;

 strcpy(mkkom.name[128],"K GOTO 0"); strcpy(mkkom.name1[128],"K БП 0"); //KБП0
strcpy(mkkom.name2[128],"KGOTO0");
 strcpy(mkkom.name[129],"K GOTO 1"); strcpy(mkkom.name1[129],"K БП 1");
strcpy(mkkom.name2[129],"KGOTO1");
 strcpy(mkkom.name[130],"K GOTO 2"); strcpy(mkkom.name1[130],"K БП 2");
strcpy(mkkom.name2[130],"KGOTO2");
 strcpy(mkkom.name[131],"K GOTO 3"); strcpy(mkkom.name1[131],"K БП 3");
strcpy(mkkom.name2[131],"KGOTO3");
 strcpy(mkkom.name[132],"K GOTO 4"); strcpy(mkkom.name1[132],"K БП 4");
strcpy(mkkom.name2[132],"KGOTO4");
 strcpy(mkkom.name[133],"K GOTO 5"); strcpy(mkkom.name1[133],"K БП 5");
strcpy(mkkom.name2[133],"KGOTO5");
 strcpy(mkkom.name[134],"K GOTO 6"); strcpy(mkkom.name1[134],"K БП 6");
strcpy(mkkom.name2[134],"KGOTO6");
 strcpy(mkkom.name[135],"K GOTO 7"); strcpy(mkkom.name1[135],"K БП 7");
strcpy(mkkom.name2[135],"KGOTO7");
 strcpy(mkkom.name[136],"K GOTO 8"); strcpy(mkkom.name1[136],"K БП 8");
strcpy(mkkom.name2[136],"KGOTO8");
 strcpy(mkkom.name[137],"K GOTO 9"); strcpy(mkkom.name1[137],"K БП 9");
strcpy(mkkom.name2[137],"KGOTO9");
 strcpy(mkkom.name[138],"K GOTO A"); strcpy(mkkom.name1[138],"K БП A");
strcpy(mkkom.name2[138],"KGOTOA");
 strcpy(mkkom.name[139],"K GOTO B"); strcpy(mkkom.name1[139],"K БП B");
strcpy(mkkom.name2[139],"KGOTOB");
 strcpy(mkkom.name[140],"K GOTO C"); strcpy(mkkom.name1[140],"K БП C");
strcpy(mkkom.name2[140],"KGOTOC");
 strcpy(mkkom.name[141],"K GOTO D"); strcpy(mkkom.name1[141],"K БП D");
strcpy(mkkom.name2[141],"KGOTOD");
 strcpy(mkkom.name[142],"K GOTO E"); strcpy(mkkom.name1[142],"K БП E");
strcpy(mkkom.name2[142],"KGOTOE");
 strcpy(mkkom.name[143],"PK GOTO");  strcpy(mkkom.name1[143],"PK БП"); mkkom.len[143]=1;
strcpy(mkkom.name2[143],"PKGOTO");  mkkom.par[143]=3;

 strcpy(mkkom.name[144],"K X>=0 0");    strcpy(mkkom.name1[144],"K x&ge;0 0");
strcpy(mkkom.name2[144],"KX>=00");
 strcpy(mkkom.name[145],"K X>=0 1");    strcpy(mkkom.name1[145],"K x&ge;0 1");
strcpy(mkkom.name2[145],"KX>=01");
 strcpy(mkkom.name[146],"K X>=0 2");    strcpy(mkkom.name1[146],"K x&ge;0 2");
strcpy(mkkom.name2[146],"KX>=02");
 strcpy(mkkom.name[147],"K X>=0 3");    strcpy(mkkom.name1[147],"K x&ge;0 3");
strcpy(mkkom.name2[147],"KX>=03");
 strcpy(mkkom.name[148],"K X>=0 4");    strcpy(mkkom.name1[148],"K x&ge;0 4");
strcpy(mkkom.name2[148],"KX>=04");
 strcpy(mkkom.name[149],"K X>=0 5");    strcpy(mkkom.name1[149],"K x&ge;0 5");
strcpy(mkkom.name2[149],"KX>=05");
 strcpy(mkkom.name[150],"K X>=0 6");    strcpy(mkkom.name1[150],"K x&ge;0 6");
strcpy(mkkom.name2[150],"KX>=06");
 strcpy(mkkom.name[151],"K X>=0 7");    strcpy(mkkom.name1[151],"K x&ge;0 7");
strcpy(mkkom.name2[151],"KX>=07");
 strcpy(mkkom.name[152],"K X>=0 8");    strcpy(mkkom.name1[152],"K x&ge;0 8");
strcpy(mkkom.name2[152],"KX>=08");
 strcpy(mkkom.name[153],"K X>=0 9");    strcpy(mkkom.name1[153],"K x&ge;0 9");
strcpy(mkkom.name2[153],"KX>=09");
 strcpy(mkkom.name[154],"K X>=0 A");    strcpy(mkkom.name1[154],"K x&ge;0 A");
strcpy(mkkom.name2[154],"KX>=0A");
 strcpy(mkkom.name[155],"K X>=0 B");    strcpy(mkkom.name1[155],"K x&ge;0 B");
strcpy(mkkom.name2[155],"KX>=0B");
 strcpy(mkkom.name[156],"K X>=0 C");    strcpy(mkkom.name1[156],"K x&ge;0 C");
strcpy(mkkom.name2[156],"KX>=0C");
 strcpy(mkkom.name[157],"K X>=0 D");    strcpy(mkkom.name1[157],"K x&ge;0 D");
strcpy(mkkom.name2[157],"KX>=0D");
 strcpy(mkkom.name[158],"K X>=0 E");    strcpy(mkkom.name1[158],"K x&ge;0 E");
strcpy(mkkom.name2[158],"KX>=0E");
 strcpy(mkkom.name[159],"PK X>=0");     strcpy(mkkom.name1[159],"PK x&ge;0"); mkkom.len[159]=1;
strcpy(mkkom.name2[159],"PKX>=0");  mkkom.par[159]=3;

 strcpy(mkkom.name[160],"K GSB 0"); strcpy(mkkom.name1[160],"K ПП 0");//KПП0
strcpy(mkkom.name2[160],"KGSB0");
 strcpy(mkkom.name[161],"K GSB 1"); strcpy(mkkom.name1[161],"K ПП 1");
strcpy(mkkom.name2[161],"KGSB1");
 strcpy(mkkom.name[162],"K GSB 2"); strcpy(mkkom.name1[162],"K ПП 2");
strcpy(mkkom.name2[162],"KGSB2");
 strcpy(mkkom.name[163],"K GSB 3"); strcpy(mkkom.name1[163],"K ПП 3");
strcpy(mkkom.name2[163],"KGSB3");
 strcpy(mkkom.name[164],"K GSB 4"); strcpy(mkkom.name1[164],"K ПП 4");
strcpy(mkkom.name2[164],"KGSB4");
 strcpy(mkkom.name[165],"K GSB 5"); strcpy(mkkom.name1[165],"K ПП 5");
strcpy(mkkom.name2[165],"KGSB5");
 strcpy(mkkom.name[166],"K GSB 6"); strcpy(mkkom.name1[166],"K ПП 6");
strcpy(mkkom.name2[166],"KGSB6");
 strcpy(mkkom.name[167],"K GSB 7"); strcpy(mkkom.name1[167],"K ПП 7");
strcpy(mkkom.name2[167],"KGSB7");
 strcpy(mkkom.name[168],"K GSB 8"); strcpy(mkkom.name1[168],"K ПП 8");
strcpy(mkkom.name2[168],"KGSB8");
 strcpy(mkkom.name[169],"K GSB 9"); strcpy(mkkom.name1[169],"K ПП 9");
strcpy(mkkom.name2[169],"KGSB9");
 strcpy(mkkom.name[170],"K GSB A"); strcpy(mkkom.name1[170],"K ПП A");
strcpy(mkkom.name2[170],"KGSBA");
 strcpy(mkkom.name[171],"K GSB B"); strcpy(mkkom.name1[171],"K ПП B");
strcpy(mkkom.name2[171],"KGSBB");
 strcpy(mkkom.name[172],"K GSB C"); strcpy(mkkom.name1[172],"K ПП C");
strcpy(mkkom.name2[172],"KGSBC");
 strcpy(mkkom.name[173],"K GSB D"); strcpy(mkkom.name1[173],"K ПП D");
strcpy(mkkom.name2[173],"KGSBD");
 strcpy(mkkom.name[174],"K GSB E"); strcpy(mkkom.name1[174],"K ПП E");
strcpy(mkkom.name2[174],"KGSBE");
 strcpy(mkkom.name[175],"PK GSB");  strcpy(mkkom.name1[175],"PK ПП"); mkkom.len[175]=1;
strcpy(mkkom.name2[175],"PKGSB");   mkkom.par[175]=3;

 strcpy(mkkom.name[176],"K M 0");   strcpy(mkkom.name1[176],"K П 0");
strcpy(mkkom.name2[176],"KM0");
 strcpy(mkkom.name[177],"K M 1");   strcpy(mkkom.name1[177],"K П 1");
strcpy(mkkom.name2[177],"KM1");
 strcpy(mkkom.name[178],"K M 2");   strcpy(mkkom.name1[178],"K П 2");
strcpy(mkkom.name2[178],"KM2");
 strcpy(mkkom.name[179],"K M 3");   strcpy(mkkom.name1[179],"K П 3");
strcpy(mkkom.name2[179],"KM3");
 strcpy(mkkom.name[180],"K M 4");   strcpy(mkkom.name1[180],"K П 4");
strcpy(mkkom.name2[180],"KM4");
 strcpy(mkkom.name[181],"K M 5");   strcpy(mkkom.name1[181],"K П 5");
strcpy(mkkom.name2[181],"KM5");
 strcpy(mkkom.name[182],"K M 6");   strcpy(mkkom.name1[182],"K П 6");
strcpy(mkkom.name2[182],"KM6");
 strcpy(mkkom.name[183],"K M 7");   strcpy(mkkom.name1[183],"K П 7");
strcpy(mkkom.name2[183],"KM7");
 strcpy(mkkom.name[184],"K M 8");   strcpy(mkkom.name1[184],"K П 8");
strcpy(mkkom.name2[184],"KM8");
 strcpy(mkkom.name[185],"K M 9");   strcpy(mkkom.name1[185],"K П 9");
strcpy(mkkom.name2[185],"KM9");
 strcpy(mkkom.name[186],"K M A");   strcpy(mkkom.name1[186],"K П A");
strcpy(mkkom.name2[186],"KMA");
 strcpy(mkkom.name[187],"K M B");   strcpy(mkkom.name1[187],"K П B");
strcpy(mkkom.name2[187],"KMB");
 strcpy(mkkom.name[188],"K M C");   strcpy(mkkom.name1[188],"K П C");
strcpy(mkkom.name2[188],"KMC");
 strcpy(mkkom.name[189],"K M D");   strcpy(mkkom.name1[189],"K П D");
strcpy(mkkom.name2[189],"KMD");
 strcpy(mkkom.name[190],"K M E");   strcpy(mkkom.name1[190],"K П E");
strcpy(mkkom.name2[190],"KME");
 strcpy(mkkom.name[191],"PK M");    strcpy(mkkom.name1[191],"PK П"); mkkom.len[191]=1;
strcpy(mkkom.name2[191],"PKM");     mkkom.par[191]=3;

 strcpy(mkkom.name[192],"K X<0 0"); strcpy(mkkom.name1[192],"K x&lt;0 0");
strcpy(mkkom.name2[192],"KX<00");
 strcpy(mkkom.name[193],"K X<0 1"); strcpy(mkkom.name1[193],"K x&lt;0 1");
strcpy(mkkom.name2[193],"KX<01");
 strcpy(mkkom.name[194],"K X<0 2"); strcpy(mkkom.name1[194],"K x&lt;0 2");
strcpy(mkkom.name2[194],"KX<02");
 strcpy(mkkom.name[195],"K X<0 3"); strcpy(mkkom.name1[195],"K x&lt;0 3");
strcpy(mkkom.name2[195],"KX<03");
 strcpy(mkkom.name[196],"K X<0 4"); strcpy(mkkom.name1[196],"K x&lt;0 4");
strcpy(mkkom.name2[196],"KX<04");
 strcpy(mkkom.name[197],"K X<0 5"); strcpy(mkkom.name1[197],"K x&lt;0 5");
strcpy(mkkom.name2[197],"KX<05");
 strcpy(mkkom.name[198],"K X<0 6"); strcpy(mkkom.name1[198],"K x&lt;0 6");
strcpy(mkkom.name2[198],"KX<06");
 strcpy(mkkom.name[199],"K X<0 7"); strcpy(mkkom.name1[199],"K x&lt;0 7");
strcpy(mkkom.name2[199],"KX<07");
 strcpy(mkkom.name[200],"K X<0 8"); strcpy(mkkom.name1[200],"K x&lt;0 8");
strcpy(mkkom.name2[200],"KX<08");
 strcpy(mkkom.name[201],"K X<0 9"); strcpy(mkkom.name1[201],"K x&lt;0 9");
strcpy(mkkom.name2[201],"KX<09");
 strcpy(mkkom.name[202],"K X<0 A"); strcpy(mkkom.name1[202],"K x&lt;0 A");
strcpy(mkkom.name2[202],"KX<0A");
 strcpy(mkkom.name[203],"K X<0 B"); strcpy(mkkom.name1[203],"K x&lt;0 B");
strcpy(mkkom.name2[203],"KX<0B");
 strcpy(mkkom.name[204],"K X<0 C"); strcpy(mkkom.name1[204],"K x&lt;0 C");
strcpy(mkkom.name2[204],"KX<0C");
 strcpy(mkkom.name[205],"K X<0 D"); strcpy(mkkom.name1[205],"K x&lt;0 D");
strcpy(mkkom.name2[205],"KX<0D");
 strcpy(mkkom.name[206],"K X<0 E"); strcpy(mkkom.name1[206],"K x&lt;0 E");
strcpy(mkkom.name2[206],"KX<0E");
 strcpy(mkkom.name[207],"PK X<0");  strcpy(mkkom.name1[207],"PK x&lt;0"); mkkom.len[207]=1;
strcpy(mkkom.name2[207],"PKX<0");   mkkom.par[207]=3;

 strcpy(mkkom.name[208],"K RM 0");  strcpy(mkkom.name1[208],"K ИП 0");  //KИП0
strcpy(mkkom.name2[208],"KRM0");
 strcpy(mkkom.name[209],"K RM 1");  strcpy(mkkom.name1[209],"K ИП 1");
strcpy(mkkom.name2[209],"KRM1");
 strcpy(mkkom.name[210],"K RM 2");  strcpy(mkkom.name1[210],"K ИП 2");
strcpy(mkkom.name2[210],"KRM2");
 strcpy(mkkom.name[211],"K RM 3");  strcpy(mkkom.name1[211],"K ИП 3");
strcpy(mkkom.name2[211],"KRM3");
 strcpy(mkkom.name[212],"K RM 4");  strcpy(mkkom.name1[212],"K ИП 4");
strcpy(mkkom.name2[212],"KRM4");
 strcpy(mkkom.name[213],"K RM 5");  strcpy(mkkom.name1[213],"K ИП 5");
strcpy(mkkom.name2[213],"KRM5");
 strcpy(mkkom.name[214],"K RM 6");  strcpy(mkkom.name1[214],"K ИП 6");
strcpy(mkkom.name2[214],"KRM6");
 strcpy(mkkom.name[215],"K RM 7");  strcpy(mkkom.name1[215],"K ИП 7");
strcpy(mkkom.name2[215],"KRM7");
 strcpy(mkkom.name[216],"K RM 8");  strcpy(mkkom.name1[216],"K ИП 8");
strcpy(mkkom.name2[216],"KRM8");
 strcpy(mkkom.name[217],"K RM 9");  strcpy(mkkom.name1[217],"K ИП 9");
strcpy(mkkom.name2[217],"KRM9");
 strcpy(mkkom.name[218],"K RM A");  strcpy(mkkom.name1[218],"K ИП A");
strcpy(mkkom.name2[218],"KRMA");
 strcpy(mkkom.name[219],"K RM B");  strcpy(mkkom.name1[219],"K ИП B");
strcpy(mkkom.name2[219],"KRMB");
 strcpy(mkkom.name[220],"K RM C");  strcpy(mkkom.name1[220],"K ИП C");
strcpy(mkkom.name2[220],"KRMC");
 strcpy(mkkom.name[221],"K RM D");  strcpy(mkkom.name1[221],"K ИП D");
strcpy(mkkom.name2[221],"KRMD");
 strcpy(mkkom.name[222],"K RM E");  strcpy(mkkom.name1[222],"K ИП E");
strcpy(mkkom.name2[222],"KRME");
 strcpy(mkkom.name[223],"PK RM");   strcpy(mkkom.name1[223],"PK ИП"); mkkom.len[223]=1;
strcpy(mkkom.name2[223],"PKRM");    mkkom.par[223]=3;

 strcpy(mkkom.name[224],"K X=0 0"); strcpy(mkkom.name1[224],"K x=0 0");
strcpy(mkkom.name2[224],"KX=00");
 strcpy(mkkom.name[225],"K X=0 1"); strcpy(mkkom.name1[225],"K x=0 1");
strcpy(mkkom.name2[225],"KX=01");
 strcpy(mkkom.name[226],"K X=0 2"); strcpy(mkkom.name1[226],"K x=0 2");
strcpy(mkkom.name2[226],"KX=02");
 strcpy(mkkom.name[227],"K X=0 3"); strcpy(mkkom.name1[227],"K x=0 3");
strcpy(mkkom.name2[227],"KX=03");
 strcpy(mkkom.name[228],"K X=0 4"); strcpy(mkkom.name1[228],"K x=0 4");
strcpy(mkkom.name2[228],"KX=04");
 strcpy(mkkom.name[229],"K X=0 5"); strcpy(mkkom.name1[229],"K x=0 5");
strcpy(mkkom.name2[229],"KX=05");
 strcpy(mkkom.name[230],"K X=0 6"); strcpy(mkkom.name1[230],"K x=0 6");
strcpy(mkkom.name2[230],"KX=06");
 strcpy(mkkom.name[231],"K X=0 7"); strcpy(mkkom.name1[231],"K x=0 7");
strcpy(mkkom.name2[231],"KX=07");
 strcpy(mkkom.name[232],"K X=0 8"); strcpy(mkkom.name1[232],"K x=0 8");
strcpy(mkkom.name2[232],"KX=08");
 strcpy(mkkom.name[233],"K X=0 9"); strcpy(mkkom.name1[233],"K x=0 9");
strcpy(mkkom.name2[233],"KX=09");
 strcpy(mkkom.name[234],"K X=0 A"); strcpy(mkkom.name1[234],"K x=0 A");
strcpy(mkkom.name2[234],"KX=0A");
 strcpy(mkkom.name[235],"K X=0 B"); strcpy(mkkom.name1[235],"K x=0 B");
strcpy(mkkom.name2[235],"KX=0B");
 strcpy(mkkom.name[236],"K X=0 C"); strcpy(mkkom.name1[236],"K x=0 C");
strcpy(mkkom.name2[236],"KX=0C");
 strcpy(mkkom.name[237],"K X=0 D"); strcpy(mkkom.name1[237],"K x=0 D");
strcpy(mkkom.name2[237],"KX=0D");
 strcpy(mkkom.name[238],"K X=0 E"); strcpy(mkkom.name1[238],"K x=0 E");
strcpy(mkkom.name2[238],"KX=0E");
 strcpy(mkkom.name[239],"PK X=0");  strcpy(mkkom.name1[239],"PK x=0"); mkkom.len[239]=1;
strcpy(mkkom.name2[239],"PKX=0");   mkkom.par[239]=3;

 strcpy(mkkom.name[240],".DB 0F0h");        strcpy(mkkom.name1[240],"F0h");
strcpy(mkkom.name2[240],"?");
 strcpy(mkkom.name[241],"P GOTO");  strcpy(mkkom.name1[241],"P БП"); mkkom.len[241]=2; //PБП
strcpy(mkkom.name2[241],"PGOTO");   mkkom.par[241]=2;
 strcpy(mkkom.name[242],"PP RTN");      strcpy(mkkom.name1[242],"PP В/О");
strcpy(mkkom.name2[242],"PPRTN");
 strcpy(mkkom.name[243],"P GSB");   strcpy(mkkom.name1[243],"P ПП"); mkkom.len[243]=2; //PПП
strcpy(mkkom.name2[243],"PGSB");    mkkom.par[243]=2;
 strcpy(mkkom.name[244],"PP M");        strcpy(mkkom.name1[244],"PP П"); mkkom.len[244]=2; //PPП
strcpy(mkkom.name2[244],"PPM");     mkkom.par[244]=4;
 strcpy(mkkom.name[245],".DB 0F5h");        strcpy(mkkom.name1[245],"F5h");
strcpy(mkkom.name2[245],"?");
 strcpy(mkkom.name[246],"PP RM");   strcpy(mkkom.name1[246],"PP ИП"); mkkom.len[246]=2;
strcpy(mkkom.name2[246],"PPRM");    mkkom.par[246]=4;
 strcpy(mkkom.name[247],"P X!=0");  strcpy(mkkom.name1[247],"P x&ne;0"); mkkom.len[247]=2;
strcpy(mkkom.name2[247],"PX!=0");   mkkom.par[247]=2;
 strcpy(mkkom.name[248],"PF L2");   strcpy(mkkom.name1[248],"PF L2"); mkkom.len[248]=2;
strcpy(mkkom.name2[248],"PFL2");    mkkom.par[248]=2;
 strcpy(mkkom.name[249],"P X>=0");  strcpy(mkkom.name1[249],"P x&ge;0"); mkkom.len[249]=2;
strcpy(mkkom.name2[249],"PX>=0");   mkkom.par[249]=2;
 strcpy(mkkom.name[250],"PF L3");   strcpy(mkkom.name1[250],"PF L3"); mkkom.len[250]=2;
strcpy(mkkom.name2[250],"PFL3");    mkkom.par[250]=2;
 strcpy(mkkom.name[251],"PF L1");   strcpy(mkkom.name1[251],"PF L1"); mkkom.len[251]=2;
strcpy(mkkom.name2[251],"PFL1");    mkkom.par[251]=2;
 strcpy(mkkom.name[252],"P X<0");   strcpy(mkkom.name1[252],"P x&lt;0"); mkkom.len[252]=2;
strcpy(mkkom.name2[252],"PX<0");    mkkom.par[252]=2;
 strcpy(mkkom.name[253],"PF L0");   strcpy(mkkom.name1[253],"PF L0"); mkkom.len[253]=2;
strcpy(mkkom.name2[253],"PFL0");    mkkom.par[253]=2;
 strcpy(mkkom.name[254],"P X=0");   strcpy(mkkom.name1[254],"P x=0"); mkkom.len[254]=2;
strcpy(mkkom.name2[254],"PX=0");    mkkom.par[254]=2;
 strcpy(mkkom.name[255],".DB 0FFh");        strcpy(mkkom.name1[255],"FFh");
strcpy(mkkom.name2[255],"?");


for (i=0; i<256; i++)
    {
    strcpy(temp,mkkom.name1[i]);
    mkstrcpy(mkkom.name1[i],temp,1,0);  // перекодировать в cp1251 для html
    }

end: return(re);
}

/*---------------- Инициализация таблицы ошибок компиляции ----------------*/

int init_mkerror()
{
int i;
int re=0;
compilation_error = 0;

if (mkerror.init!=0) {re=1; goto end;}

mkerror.init=1;
mkerror.initvsego=20;

mkerror.vsego=0;

for (i=0; i<mkerror.initvsego; i++)
    {
    strcpy(mkerror.name[i],"");
    }

//mkstrcpy(mkerror.name[0],"E000 Нет ошибок",mkiocharset,0);
mkstrcpy(mkerror.name[0],"E000 No errors",mkiocharset,0);

//mkstrcpy(mkerror.name[1],"E001 Переполнение таблицы ошибок (слишком много ошибок)",mkiocharset,0);
mkstrcpy(mkerror.name[1],"E001 Errors table overflow (too many errors)",mkiocharset,0);

//mkstrcpy(mkerror.name[2],"E002 Недопустимые символы",mkiocharset,0);
mkstrcpy(mkerror.name[2],"E002 Invalid characters",mkiocharset,0);

//mkstrcpy(mkerror.name[3],"E003 Переполнение таблицы идентификаторов",mkiocharset,0);
mkstrcpy(mkerror.name[3],"E003 Identificators table overflow",mkiocharset,0);

//mkstrcpy(mkerror.name[4],"E004 Недопустимое имя идентификатора",mkiocharset,0);
mkstrcpy(mkerror.name[4],"E004 Invalid name of identificator",mkiocharset,0);

//mkstrcpy(mkerror.name[5],"E005 Недопустимое значение идентификатора",mkiocharset,0);
mkstrcpy(mkerror.name[5],"E005 Invalid value of identificator",mkiocharset,0);

//mkstrcpy(mkerror.name[6],"E006 Недопустимый адрес",mkiocharset,0);
mkstrcpy(mkerror.name[6],"E006 Ivalid address",mkiocharset,0);

//mkstrcpy(mkerror.name[7],"E007 Значение идентификатора не определено",mkiocharset,0);
mkstrcpy(mkerror.name[7],"E007 The value of identificator is not defined",mkiocharset,0);

//mkstrcpy(mkerror.name[8],"E008 Неверное значение",mkiocharset,0);
mkstrcpy(mkerror.name[8],"E008 Incorrect value",mkiocharset,0);

//mkstrcpy(mkerror.name[9],"E009 Неопознаный оператор",mkiocharset,0);
mkstrcpy(mkerror.name[9],"E009 Unknown statement",mkiocharset,0);

//mkstrcpy(mkerror.name[10],"E010 Параметры команды отсутствуют",mkiocharset,0);
mkstrcpy(mkerror.name[10],"E010 Command parameters are absent",mkiocharset,0);

//mkstrcpy(mkerror.name[11],"E011 Значение метки не определено",mkiocharset,0);
mkstrcpy(mkerror.name[11],"E011 The value of symbolic reference is not defined",mkiocharset,0);

//mkstrcpy(mkerror.name[12],"E012 Переполнение таблицы меток",mkiocharset,0);
mkstrcpy(mkerror.name[12],"E012 Overflow of symbolic reference table",mkiocharset,0);

//mkstrcpy(mkerror.name[13],"E013 Недопустимое имя метки",mkiocharset,0);
mkstrcpy(mkerror.name[13],"E013 Invalid name of symbolic reference",mkiocharset,0);

//mkstrcpy(mkerror.name[14],"E014 Недопустимое значение метки",mkiocharset,0);
mkstrcpy(mkerror.name[14],"E014 Invalid value of symbolic reference",mkiocharset,0);

//mkstrcpy(mkerror.name[15],"E015 Совпадение имени метки и идентификатора",mkiocharset,0); //v
mkstrcpy(mkerror.name[15],"E015 Name of symbolic reference and identificator are the same",mkiocharset,0); //v

mkerror.w=15;
//mkstrcpy(mkerror.name[mkerror.w+1],"W001 Используется расширенная адресация в пределах страницы",mkiocharset,0);
mkstrcpy(mkerror.name[mkerror.w+1],"W001 Extended addressing is used within a page",mkiocharset,0);

//mkstrcpy(mkerror.name[mkerror.w+2],"W002 Текст отсутствует",mkiocharset,0);
mkstrcpy(mkerror.name[mkerror.w+2],"W002 Text is absent",mkiocharset,0);

//mkstrcpy(mkerror.name[mkerror.w+3],"W003 Черырёхзначное число в .NUMT",mkiocharset,0);
mkstrcpy(mkerror.name[mkerror.w+3],"W003 .NUMT contains four-digit number",mkiocharset,0);

end: return(re);
}

/*--------------------------------------------------------------------------*/
int init_msg(void)
    { // инициализация списка сообщений 0-по умолчанию
int re=0;

    if (dos==1)strcpy(mes[0],"\r\n");
        else strcpy(mes[0],"\n");

    mkstrcpy(mes[1],"",mkiocharset,0);
    mkstrcpy(mes[2],"",mkiocharset,0);
    mkstrcpy(mes[3],"",mkiocharset,0);
    mkstrcpy(mes[4],"mkl2mkp -h for help",mkiocharset,0);

    //mkstrcpy(mes[5],"Файл MKL не найден",mkiocharset,0);
    mkstrcpy(mes[5],"MKL file not found",mkiocharset,0);

    //mkstrcpy(mes[6],"Файл не открыт ",mkiocharset,0);
    mkstrcpy(mes[6],"File not open ",mkiocharset,0);

    //mkstrcpy(mes[7],"Файл MKP ",mkiocharset,0);
    mkstrcpy(mes[7],"MKP file ",mkiocharset,0);

    //mkstrcpy(mes[8]," в строке ",mkiocharset,0);
    mkstrcpy(mes[8],"in line ",mkiocharset,0);

    //mkstrcpy(mes[9],"Начальная страница: ",mkiocharset,0);
    mkstrcpy(mes[9],"First page: ",mkiocharset,0);

    //mkstrcpy(mes[10],"Длина (страниц): ",mkiocharset,0);
    mkstrcpy(mes[10],"Length (pages): ",mkiocharset,0);

    //mkstrcpy(mes[11],"Компиляция завершена",mkiocharset,0);
    mkstrcpy(mes[11],"Compilation completed",mkiocharset,0);

    //mkstrcpy(mes[12],"Контрольная сумма",mkiocharset,0);
    mkstrcpy(mes[12],"Check sum",mkiocharset,0);

    //v0.22
    //mkstrcpy(mes[13],"mkl2mkp - компилятор MKL-файлов для ЭКВМ Электроника МК",mkiocharset,0);
    mkstrcpy(mes[13],"mkl2mkp - Compiler of MKL-files for EKVM Electronica MK",mkiocharset,0);

    //mkstrcpy(mes[14],"mkl2mkp [ключи] MKL-файл [MKP-файл]",mkiocharset,0);
    mkstrcpy(mes[14],"Usage:\n  mkl2mkp [options] MKL-file [MKP-file]",mkiocharset,0);

    //mkstrcpy(mes[15],"-h,-H,-?\tэта подсказка;",mkiocharset,0);
    mkstrcpy(mes[15],"-h,-H,-?\tthis help;",mkiocharset,0);

    //mkstrcpy(mes[16],"-v,-V\tверсия программы;",mkiocharset,0);
    mkstrcpy(mes[16],"-v,-V\t\tprogram version;",mkiocharset,0);

    //mkstrcpy(mes[17],"-w,-W\tлицензия (GPL2);",mkiocharset,0);
    mkstrcpy(mes[17],"-w,-W\t\tGPL2 license;",mkiocharset,0);

    //mkstrcpy(mes[18],"-i,-I\tвывод информации.",mkiocharset,0);
    mkstrcpy(mes[18],"-i,-I\t\tInformation view (verbose mode).",mkiocharset,0);

    //mkstrcpy(mes[19],"Кодировка ",mkiocharset,0);
    mkstrcpy(mes[19],"Encoding ",mkiocharset,0);

    //mkstrcpy(mes[20],"mkl2mkp -h для вывода подсказки",mkiocharset,0);
    mkstrcpy(mes[20],"mkl2mkp -h for help",mkiocharset,0);


end: return(re);
    }

/*------------------------------------------------------ Перекодировка --------------*/
void mkstrcpy (char* str2, char* str1, int chset2, int chset1)
{
// перенос из str1 в str2 в кодировках chset1 chset2 соответственно
//chset= 0=CP866|1=CP1251|2=KOI8R|3=UTF-8|4=UCS-2
// v0.13 - п/п переделана

int i,j,k,m;
long l,l1,l2;

if (chset1==chset2) {strcpy(str2,str1); goto end;}

k=strlen(str1);
if ((k==0) ||(chset1<0)||(chset1>4)||(chset2<0)||(chset2>4))
    {str2[0]=0;
    goto end;
    }

i=0;
j=0;

if (chset1==4) i=2;
if (chset2==4) {str2[0]=0xFF;
        str2[1]=0xFE;
        j=2;
        }

ci:

if ((chset1==0)||(chset1==1)||(chset1==2))
    {l=str1[i];
    i++;
    if (l<0) l+=256;
    goto ci1;
    }
if (chset1==3)
    {l1=str1[i];
    l2=str1[i+1];
    if (l1<0) l1+=256;
    if (l1<128) {i+=1; l=l1;}
    else    {if (l2<0) l2+=256;
        i+=2;
        l=l1*256+l2;
        }
    goto ci1;
    }
if (chset1==4)
    {l1=str1[i];
    l2=str1[i+1];
    if (l1<0) l1+=256;
    if (l2<0) l2+=256;
    i+=2;
    l=l1+l2*256;
    goto ci1;
    }

ci1:
l2=l;
// поиск по массиву. если ничего не будет найдено - не перекодировать
for (m=0; m<66; m++)
    {
    l1=mkcharset[m][chset1];
    if (l1==l) l2=mkcharset[m][chset2];
    }

if ((chset2==0)||(chset2==1)||(chset2==2))
    {
    // вставить один байт в строку
    str2[j]=(char)(l2%256);
    j++;
    goto cinext;
    }
if (chset2==3)
    {
    if (l2<128)
        {
        // вставить один байт в строку
        str2[j]=(char)(l2%256);
        j++;
        goto cinext;
        }
    str2[j]=(char)((l2/256)%256);
    str2[j+1]=(char)(l2%256);
    j+=2;
    goto cinext;
    }
if (chset2==4)
    {
    str2[j]=(char)(l2%256);
    str2[j+1]=(char)((l2/256)%256);
    j+=2;
    }
cinext:
if ((i>=k)||(j>254))
    {
    str2[j]=0;
    str2[j+1]=0;
    goto end;
    }
else goto ci;

end: return;
}


/*===================================================== MAIN ======================*/

int main(int argc, char *argv[])
{
int i,j,k;
char c;
//int flag_charset;  del v0.18
int re=EXIT_SUCCESS;

fp0=fp1=fp2=fp3=fp4=NULL;

fname_vsego=0;
//flag_charset=0; del v0.18
mkiocharset=0;
kluch_i=0;  // v0.21
j=0;

// считывание файла конфигурации - если он есть
//fp0=fopen("mkl2mkp.cfg","rb");    // открыть файл CFG для чтения del v0.18
fp0=fopen("mk.cfg","rb");   // открыть файл CFG для чтения v0.18
if (fp0!=NULL)
    {
    i=fgetc(fp0);
    //if (i!=EOF) mkiocharset=i-48; // байт 0 - кодировка del v0.18
    //if (i!=EOF) mkiocharset=i;    // байт 0 - кодировка v0.18 del v0.20

    if (i!=EOF)
        {   // v0.20
        if (i%256=='m')
            {
            k=fseek(fp0,32,SEEK_SET);
            if (k==0) i=fgetc(fp0); // байт 32 - кодировка для CFG v0.11
            }
        mkiocharset=i;  // байт 0 - кодировка для CFG v0.1
        }
    if ((mkiocharset<0)||(mkiocharset>4)) mkiocharset=0;
    fclose(fp0);
    }

init_msg(); // кодировка установлена - инициализировать сообщения повторно

// разбор строки аргументов
if (argc<=1) goto nach;
for (i=1; i<argc; i++)
    {
    if ((argv[i][0]!='/')&&(argv[i][0]!='-'))
        {
        // C v0.18 удалено ветвление по flag_charset и блок разбора кодировки для ключа -c
        // file
        j=fname_vsego;
        if (j<0) j=0;
        if (j>4) j=4;
        strncpy(fname[j+1],argv[i],255);
        if (j<4) fname_vsego++;
        }
    else    {
        c=argv[i][1];
        if ((c=='h')||(c=='H')||(c=='?'))
            { /* help */
            // v0.22
            printf("%s",mes[0]);
            printf("%s %s",mes[13],mes[0]);
            printf("%s %s",mes[14],mes[0]);
            printf("\t%s %s",mes[15],mes[0]);
            printf("\t%s %s",mes[16],mes[0]);
            printf("\t%s %s",mes[17],mes[0]);
            printf("\t%s %s",mes[18],mes[0]);
            printf("%s ",mes[19]);
            switch(mkiocharset)
                {case 0: printf("CP866"); break;
                case 1: printf("CP1251"); break;
                case 2: printf("KOI8R"); break;
                case 3: printf("UTF8"); break;
                case 4: printf("UCS2"); break;
                default: printf("?");
                }
            printf("%s",mes[0]);

            //v0.13
            printf("Russian charset is ");
            switch(mkiocharset)
                {case 0: printf("CP866"); break;
                case 1: printf("CP1251"); break;
                case 2: printf("KOI8R"); break;
                case 3: printf("UTF8"); break;
                case 4: printf("UCS2"); break;
                default: printf("?");
                }

            goto end;
            }
        if ((c=='v')||(c=='V'))
            { /* version */
            printf(mkversion);
            if (dos==0) printf(" (GNU/Linux)");
            if (dos==1) printf(" (DOS)");
            if (dos==2) printf(" (Win32)"); //v0.17
            goto end;
            }

        /* del v0.18
        if ((c=='c')||(c=='C'))
            {
            flag_charset=1;
            goto nextargv;
            }
        */

        if ((c=='w')||(c=='W'))
            { /* copyleft & warranty */
            printf("Copyright (C) 2007-2010 by NPP SEMIKO (Russia, Novosibirsk) %s%s",mes[0],mes[0]);
            printf("This program is free software; you can redistribute it and/or modify %s",mes[0]);
            printf("it under the terms of the GNU General Public License as published by %s",mes[0]);
            printf("the Free Software Foundation; either version 2 of the License, or %s",mes[0]);
            printf("(at your option) any later version. %s%s",mes[0],mes[0]);

            printf("This program is distributed in the hope that it will be useful, %s",mes[0]);
            printf("but WITHOUT ANY WARRANTY; without even the implied warranty of %s",mes[0]);
            printf("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the %s",mes[0]);
            printf("GNU General Public License for more details. %s%s",mes[0],mes[0]);

            printf("You should have received a copy of the GNU General Public License %s",mes[0]);
            printf("along with this program; if not, write to the Free Software %s",mes[0]);
            printf("Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA");
            goto end;
            }

        if ((c=='i')||(c=='I'))
            { // v0.21
            kluch_i=1;  /* выводить информацию */
            goto nextargv;
            }

        //if (flag_charset!=2) printf(mes[4]); goto end; //del v.018
        }
nextargv:;
    }

nach:

//if (fname_vsego<=0) {printf("%s %s %s %s",mes[4],mes[0],mes[4],mes[0]); re=EX_USAGE; goto end;}   // нет файла v0.22
if (fname_vsego<=0) {printf("%s %s %s",mes[0],mes[4],mes[0]); re=EX_USAGE; goto end;}   // нет файла v0.22

fp1=fopen(fname[1],"rt");   // открыть MKL как текст для чтения

if (fp1==NULL) {printf(mes[5]); re=EX_NOINPUT; goto end;}   // файл MKL не открыт


k=strlen(fname[1]);
for (i=k; i>0; i--)
    {
    if ( (fname[1][i]=='.')&&(i<k-3)&& ((fname[1][i+1]=='m') || (fname[1][i+1]=='M')) && ((fname[1][i+2]=='k') || (fname[1][i+2]=='K')) && ((fname[1][i+3]=='l') || (fname[1][i+3]=='L')) ) goto m1;
        }
    i=k;
m1: strncpy(fname[0],fname[1],248);
    if ((i>0)&&(i<strlen(fname[0]))) fname[0][i]=0;
    if (dos==1) fname[0][8]=0;  // в ДОС не более 8 символов в имени

if (fname_vsego<=1)
    {
    // сделать имя для файла MKP
    strcpy(fname[2],fname[0]);
    strcat(fname[2],".mkp");
    if (kluch_i!=0) printf("%s %s %s",mes[7],fname[1],mes[0]);
    }

if (fname_vsego<=2)
    {
    // сделать имя для файла MKP.TXT
    strcpy(fname[3],fname[0]);
    if ((dos==0)||(dos==2)) strcat(fname[3],".mkp"); // v0.17 - dos==2
    strcat(fname[3],".txt");
    }

if (fname_vsego<=3)
    {
    // сделать имя для файла MKP.HTM
    strcpy(fname[4],fname[0]);
    if ((dos==0)||(dos==2)) strcat(fname[4],".mkp"); // v0.17 - dos==2
    strcat(fname[4],".htm");
    }

    // fname[2] существует
fp2=fopen(fname[2],"wb");   // открыть MKP как двоичный для записи
if (fp2==NULL) {printf("%s %s",mes[6],fname[2]); re=EX_CANTCREAT; goto end;}

fp3=fopen(fname[3],"wt");   // открыть MKP.TXT как текстовый для записи
if (fp3==NULL) {printf("%s %s",mes[6],fname[3]); re=EX_CANTCREAT;  goto end;}

fp4=fopen(fname[4],"wt");   // открыть MKP.HTM как текстовый для записи
if (fp4==NULL) {printf("%s %s",mes[6],fname[4]); re=EX_CANTCREAT; goto end;}

init_mkerror(); // кодировка установлена - инициализировать структуры
init_mkkom();

compil_l(); // подпрограмма компиляции и запись MKP
buf2txt();  // в листинг TXT
buf2htm();  // в HTML

fclose(fp4);
fclose(fp3);
fclose(fp2);
fclose(fp1);

end:
printf(mes[0]);
if (compilation_error > 0)
{
    re = EX_COMPILATION;
}

return re;
}
