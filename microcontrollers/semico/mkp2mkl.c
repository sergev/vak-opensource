/***************************************************************************
 *   Copyright (C) 2011 by NPP SEMIKO                                      *
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Коды выхода BSD from @(#)sysexits.h	8.1 (Berkeley) 6/2/93 */
/* Copyright (c) 1987, 1993 */
/* The Regents of the University of California.  All rights reserved. */
#define EX_USAGE	64	/* command line usage error */
#define EX_DATAERR	65	/* data format error */
#define EX_NOINPUT	66	/* cannot open input */
#define EX_NOUSER	67	/* addressee unknown */
#define EX_NOHOST	68	/* host name unknown */
#define EX_UNAVAILABLE	69	/* service unavailable */
#define EX_SOFTWARE	70	/* internal software error */
#define EX_OSERR	71	/* system error (e.g., can't fork) */
#define EX_OSFILE	72	/* critical OS file missing */
#define EX_CANTCREAT	73	/* can't create (user) output file */
#define EX_IOERR	74	/* input/output error */
#define EX_TEMPFAIL	75	/* temp failure; user is invited to retry */
#define EX_PROTOCOL	76	/* remote error in protocol */
#define EX_NOPERM	77	/* permission denied */
#define EX_CONFIG	78	/* configuration error */

char version[]="Version 0.11";
/*
v0.1
v0.11 - 01.04.11
*/
int dos=0;	/* 0-GNU/Linux 1-DOS 2-Win32 */

char fname[5][256];	/* 0-MKP без расширения 1-MKP исходный 2-MKL 3-TXT 4-HTM */
int fname_vsego;	/* всего файлов в строке аргументов */
FILE *fp1, *fp2, *fp3, *fp4;	/* потоки 1-MKP 2-MKL 3-TXT 4-HTM */

char mes0[8];	/* ВК ПС для DOS или ВК для Linux */
char str[1024];
char str1[1024];
char str2[1024];
char strerr[256];	/* сообщение об ошибке */
char ctemp[64];

int kluch_e;		/* вывод сообщений 0-нет, 1 -есть*/


struct MKKOM
	{ // команды МК
	int len[256];	// количество доп байт команды 0-нет
	int par[256];  // параметр команды 0-нет; 1,2 - адрес; 3,4 - номер регистра 1 или 2 байта
	char name[256][24]; // мнемоника команды ЛАТ
	char name1[256][24]; // мнемоника команды РУС HTML
	char name2[256][24]; // мнемоника команды LAT комп (упакованная без пробелов)
	int init; // признак инициализации структуры 0-нет 1-есть
	} mkkom;

struct MKFUNK
	{ // Регистры функций ЭКВМ
	char rname[100][256];	// раздел для функции - наименование
	char name[1000][256];	// наименование функции с номером n
	char opis[1000][256];	// описание функции с номером n
	int r[1000];
	int init;
	} mkfunk;

struct DMETKA
	{
	// структура меток и регистров для декомпиляции файла mkp
	long adr[20000];	// адрес оператора в котором стоит метка или регистр
	long val[20000];	// значение
	int reg[20000];		// метка =0, регистр =1

	long vsego;			// всего найдено
	} dmetka;


int mkbufp[10020];

int gcomp_proglen, gcomp_progmin, gflag_mkbufp;
long gmkbufn_beg,gmkbufn_end;
long gcomp_progks1=0;	/* ADD - вся программа */
int gcomp_progks2=0;	/* XOR - вся программа */
long ks1[100];		/* ADD - постранично */
int ks2[100];		/* XOR - постранично */

/* функции */

int init_mkkom(void);
int init_mkfunk(void);
int str2file(int p);
int read_mkp(void);
int ks_mkp(void);
int dos2win(char* );
int mkp2text(void);
int compil_p(void);

/*==================================================== DOS2WIN =================*/

int dos2win(char* sss)
{
int i;
unsigned ui;
int re=0;

for (ui=0; ui<strlen(sss); ui++)
	{
	i=sss[ui];
	if (i<0) i+=256;	// если char знаковый

	// из кодировки DOS (866) в Windows (1251)
        if (i>=128)
		{if ((i>=128)&&(i<176)) {i+=64; goto m2;}
		if ((i>=224)&&(i<240)) {i+=16; goto m2;}
		if (i==240) {i=168; goto m2;}   /* jo */
		if (i==241) {i=184; goto m2;}   /* JO */
		if (i==250) {i=183; goto m2;}   // точка в центре
		if (i==252) {i=185; goto m2;}	// номер №
		if (i==253) {i=164; goto m2;} // клоп - солнышко
 		i=32;
                }
m2:	sss[ui]=(char)i;
     }

return(re);
}
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

 strcpy(mkkom.name[0],"0");	strcpy(mkkom.name1[0],"0");
strcpy(mkkom.name2[0],"0");
 strcpy(mkkom.name[1],"1");  	strcpy(mkkom.name1[1],"1");
strcpy(mkkom.name2[1],"1");
 strcpy(mkkom.name[2],"2");	strcpy(mkkom.name1[2],"2");
strcpy(mkkom.name2[2],"2");
 strcpy(mkkom.name[3],"3");	strcpy(mkkom.name1[3],"3");
strcpy(mkkom.name2[3],"3");
 strcpy(mkkom.name[4],"4");	strcpy(mkkom.name1[4],"4");
strcpy(mkkom.name2[4],"4");
 strcpy(mkkom.name[5],"5");	strcpy(mkkom.name1[5],"5");
strcpy(mkkom.name2[5],"5");
 strcpy(mkkom.name[6],"6");	strcpy(mkkom.name1[6],"6");
strcpy(mkkom.name2[6],"6");
 strcpy(mkkom.name[7],"7");	strcpy(mkkom.name1[7],"7");
strcpy(mkkom.name2[7],"7");
 strcpy(mkkom.name[8],"8");	strcpy(mkkom.name1[8],"8");
strcpy(mkkom.name2[8],"8");
 strcpy(mkkom.name[9],"9");	strcpy(mkkom.name1[9],"9");
strcpy(mkkom.name2[9],"9");
 strcpy(mkkom.name[10],","); 	strcpy(mkkom.name1[10],",");
strcpy(mkkom.name2[10],",");
 strcpy(mkkom.name[11],"+/-");	strcpy(mkkom.name1[11],"/-/");
strcpy(mkkom.name2[11],"+/-");
 strcpy(mkkom.name[12],"EE");	strcpy(mkkom.name1[12],"ВП");// ВП
strcpy(mkkom.name2[12],"EE");
 strcpy(mkkom.name[13],"CX");	strcpy(mkkom.name1[13],"Cx"); // Cx
strcpy(mkkom.name2[13],"CX");
 strcpy(mkkom.name[14],"ENT");	strcpy(mkkom.name1[14],"B&uarr;"); // B^
strcpy(mkkom.name2[14],"ENT");
 strcpy(mkkom.name[15],"F ANS"); strcpy(mkkom.name1[15],"F Bx"); // F Bx
strcpy(mkkom.name2[15],"FANS");

 strcpy(mkkom.name[16],"+");	strcpy(mkkom.name1[16],"+");
strcpy(mkkom.name2[16],"+");
 strcpy(mkkom.name[17],"-");	strcpy(mkkom.name1[17],"-");
strcpy(mkkom.name2[17],"-");
 strcpy(mkkom.name[18],"*");	strcpy(mkkom.name1[18],"&times;");
strcpy(mkkom.name2[18],"*");
 strcpy(mkkom.name[19],"/");	strcpy(mkkom.name1[19],"&divide;");
strcpy(mkkom.name2[19],"/");
 strcpy(mkkom.name[20],"<->");	strcpy(mkkom.name1[20],"&harr;");
strcpy(mkkom.name2[20],"<->");
 strcpy(mkkom.name[21],"F 10^X");	strcpy(mkkom.name1[21],"F 10<sup>x</sup>");
strcpy(mkkom.name2[21],"F10^X");
 strcpy(mkkom.name[22],"F EXP");	strcpy(mkkom.name1[22],"F e<sup>x</sup>");
strcpy(mkkom.name2[22],"FEXP");
 strcpy(mkkom.name[23],"F LG");		strcpy(mkkom.name1[23],"F lg");
strcpy(mkkom.name2[23],"FLG");
 strcpy(mkkom.name[24],"F LN");		strcpy(mkkom.name1[24],"F ln");
strcpy(mkkom.name2[24],"FLN");
 strcpy(mkkom.name[25],"F ARCSIN");	strcpy(mkkom.name1[25],"F arcsin");
strcpy(mkkom.name2[25],"FARCSIN");
 strcpy(mkkom.name[26],"F ARCCOS");	strcpy(mkkom.name1[26],"F arccos");
strcpy(mkkom.name2[26],"FARCCOS");
 strcpy(mkkom.name[27],"F ARCTG");	strcpy(mkkom.name1[27],"F arctg");
strcpy(mkkom.name2[27],"FARCTG");
 strcpy(mkkom.name[28],"F SIN");	strcpy(mkkom.name1[28],"F sin");
strcpy(mkkom.name2[28],"FSIN");
 strcpy(mkkom.name[29],"F COS");	strcpy(mkkom.name1[29],"F cos");
strcpy(mkkom.name2[29],"FCOS");
 strcpy(mkkom.name[30],"F TG");		strcpy(mkkom.name1[30],"F tg");
strcpy(mkkom.name2[30],"FTG");
 strcpy(mkkom.name[31],".DB 1Fh");	strcpy(mkkom.name1[31],"1Fh");
strcpy(mkkom.name2[31],"?");

 strcpy(mkkom.name[32],"F PI");		strcpy(mkkom.name1[32],"F &pi;");
strcpy(mkkom.name2[32],"FPI");
 strcpy(mkkom.name[33],"F SQRT");	strcpy(mkkom.name1[33],"F &radic;");
strcpy(mkkom.name2[33],"FSQRT");
 strcpy(mkkom.name[34],"F X^2");	strcpy(mkkom.name1[34],"F x<sup>2</sup>");
strcpy(mkkom.name2[34],"FX^2");
 strcpy(mkkom.name[35],"F 1/X");	strcpy(mkkom.name1[35],"F 1/x");
strcpy(mkkom.name2[35],"F1/X");
 strcpy(mkkom.name[36],"F X^Y");	strcpy(mkkom.name1[36],"F x<sup>y</sup>");
strcpy(mkkom.name2[36],"FX^Y");
 strcpy(mkkom.name[37],"F R");		strcpy(mkkom.name1[37],"F O");
strcpy(mkkom.name2[37],"FR");
 strcpy(mkkom.name[38],"K M->D");	strcpy(mkkom.name1[38],"K М&rarr;Г");// К М-Г
strcpy(mkkom.name2[38],"KM->D");
 strcpy(mkkom.name[39],"K -");		strcpy(mkkom.name1[39],"K -");
strcpy(mkkom.name2[39],"K-");
 strcpy(mkkom.name[40],"K PRGM");	strcpy(mkkom.name1[40],"K ИПРГ");// К ИПРГ
strcpy(mkkom.name2[40],"KPRGM");
 strcpy(mkkom.name[41],"K /");		strcpy(mkkom.name1[41],"K &divide;");
strcpy(mkkom.name2[41],"K/");
 strcpy(mkkom.name[42],"K MS->D");	strcpy(mkkom.name1[42],"K МС&rarr;Г");
strcpy(mkkom.name2[42],"KMS->D");
 strcpy(mkkom.name[43],".DB 2Bh");	strcpy(mkkom.name1[43],"2Bh");
strcpy(mkkom.name2[43],"?");
 strcpy(mkkom.name[44],".DB 2Ch");	strcpy(mkkom.name1[44],"2Ch");
strcpy(mkkom.name2[44],"?");
 strcpy(mkkom.name[45],".DB 2Dh");	strcpy(mkkom.name1[45],"2Dh");
strcpy(mkkom.name2[45],"?");
 strcpy(mkkom.name[46],".DB 2Eh");	strcpy(mkkom.name1[46],"2Eh");
strcpy(mkkom.name2[46],"?");
 strcpy(mkkom.name[47],".DB 2Fh");	strcpy(mkkom.name1[47],"2Fh");
strcpy(mkkom.name2[47],"?");

 strcpy(mkkom.name[48],"K D->MS");	strcpy(mkkom.name1[48],"K Г&rarr;МС");
strcpy(mkkom.name2[48],"KD->MS");
 strcpy(mkkom.name[49],"K ABS");	strcpy(mkkom.name1[49],"K |x|");// K|x|
strcpy(mkkom.name2[49],"KABS");
 strcpy(mkkom.name[50],"K SGN");	strcpy(mkkom.name1[50],"K ЗН");// К ЗН
strcpy(mkkom.name2[50],"KSGN");
 strcpy(mkkom.name[51],"K D->M");	strcpy(mkkom.name1[51],"K Г&rarr;М");
strcpy(mkkom.name2[51],"KD->M");
 strcpy(mkkom.name[52],"K INT");	strcpy(mkkom.name1[52],"K [x]");// K[x]
strcpy(mkkom.name2[52],"KINT");
 strcpy(mkkom.name[53],"K FRAC");	strcpy(mkkom.name1[53],"K {x}");// K{x}
strcpy(mkkom.name2[53],"KFRAC");
 strcpy(mkkom.name[54],"K MAX");	strcpy(mkkom.name1[54],"K max");
strcpy(mkkom.name2[54],"KMAX");
 strcpy(mkkom.name[55],"K AND");	strcpy(mkkom.name1[55],"K AND");
strcpy(mkkom.name2[55],"KAND");
 strcpy(mkkom.name[56],"K OR");		strcpy(mkkom.name1[56],"K OR");
strcpy(mkkom.name2[56],"KOR");
 strcpy(mkkom.name[57],"K XOR");	strcpy(mkkom.name1[57],"K XOR");
strcpy(mkkom.name2[57],"KXOR");
 strcpy(mkkom.name[58],"K NOT");	strcpy(mkkom.name1[58],"K NOT");
strcpy(mkkom.name2[58],"KNOT");
 strcpy(mkkom.name[59],"K RAN");	strcpy(mkkom.name1[59],"K СЧ");// KСЧ
strcpy(mkkom.name2[59],"KRAN");
 strcpy(mkkom.name[60],".DB 3Ch");	strcpy(mkkom.name1[60],"3Ch");
strcpy(mkkom.name2[60],"?");
 strcpy(mkkom.name[61],".DB 3Dh");	strcpy(mkkom.name1[61],"3Dh");
strcpy(mkkom.name2[61],"?");
 strcpy(mkkom.name[62],".DB 3Eh");	strcpy(mkkom.name1[62],"3Eh");
strcpy(mkkom.name2[62],"?");
 strcpy(mkkom.name[63],".DB 3Fh");	strcpy(mkkom.name1[63],"3Fh");
strcpy(mkkom.name2[63],"?");

 strcpy(mkkom.name[64],"M 0");		strcpy(mkkom.name1[64],"П 0");
strcpy(mkkom.name2[64],"M0");
 strcpy(mkkom.name[65],"M 1");		strcpy(mkkom.name1[65],"П 1");
strcpy(mkkom.name2[65],"M1");
 strcpy(mkkom.name[66],"M 2");		strcpy(mkkom.name1[66],"П 2");
strcpy(mkkom.name2[66],"M2");
 strcpy(mkkom.name[67],"M 3");		strcpy(mkkom.name1[67],"П 3");
strcpy(mkkom.name2[67],"M3");
 strcpy(mkkom.name[68],"M 4");		strcpy(mkkom.name1[68],"П 4");
strcpy(mkkom.name2[68],"M4");
 strcpy(mkkom.name[69],"M 5");		strcpy(mkkom.name1[69],"П 5");
strcpy(mkkom.name2[69],"M5");
 strcpy(mkkom.name[70],"M 6");		strcpy(mkkom.name1[70],"П 6");
strcpy(mkkom.name2[70],"M6");
 strcpy(mkkom.name[71],"M 7");		strcpy(mkkom.name1[71],"П 7");
strcpy(mkkom.name2[71],"M7");
 strcpy(mkkom.name[72],"M 8");		strcpy(mkkom.name1[72],"П 8");
strcpy(mkkom.name2[72],"M8");
 strcpy(mkkom.name[73],"M 9");		strcpy(mkkom.name1[73],"П 9");
strcpy(mkkom.name2[73],"M9");
 strcpy(mkkom.name[74],"M A");		strcpy(mkkom.name1[74],"П A");
strcpy(mkkom.name2[74],"MA");
 strcpy(mkkom.name[75],"M B");		strcpy(mkkom.name1[75],"П B");
strcpy(mkkom.name2[75],"MB");
 strcpy(mkkom.name[76],"M C");		strcpy(mkkom.name1[76],"П C");
strcpy(mkkom.name2[76],"MC");
 strcpy(mkkom.name[77],"M D");		strcpy(mkkom.name1[77],"П D");
strcpy(mkkom.name2[77],"MD");
 strcpy(mkkom.name[78],"M E");		strcpy(mkkom.name1[78],"П E");
strcpy(mkkom.name2[78],"ME");
 strcpy(mkkom.name[79],"P M"); 		strcpy(mkkom.name1[79],"P П"); mkkom.len[79]=1;
strcpy(mkkom.name2[79],"PM");	    mkkom.par[79]=3;

 strcpy(mkkom.name[80],"R/S");		strcpy(mkkom.name1[80],"С/П");// С/П
strcpy(mkkom.name2[80],"R/S");
 strcpy(mkkom.name[81],"GOTO");		strcpy(mkkom.name1[81],"БП"); mkkom.len[81]=1; // БП
strcpy(mkkom.name2[81],"GOTO");     mkkom.par[81]=1;
 strcpy(mkkom.name[82],"RTN");		strcpy(mkkom.name1[82],"В/О");//В/О
strcpy(mkkom.name2[82],"RTN");
 strcpy(mkkom.name[83],"GSB");		strcpy(mkkom.name1[83],"ПП"); mkkom.len[83]=1; // ПП
strcpy(mkkom.name2[83],"GSB");      mkkom.par[83]=1;
 strcpy(mkkom.name[84],"K NOP");	strcpy(mkkom.name1[84],"K НОП");
strcpy(mkkom.name2[84],"KNOP");
 strcpy(mkkom.name[85],"K SCR");	strcpy(mkkom.name1[85],"K ЭКР"); // KЭКР
strcpy(mkkom.name2[85],"KSCR");
 strcpy(mkkom.name[86],"K GRPH");	strcpy(mkkom.name1[86],"K ГРФ");// KГРФ
strcpy(mkkom.name2[86],"KGRPH");
 strcpy(mkkom.name[87],"F X!=0");	strcpy(mkkom.name1[87],"F x&ne;0"); mkkom.len[87]=1;
strcpy(mkkom.name2[87],"FX!=0");	mkkom.par[87]=1;
 strcpy(mkkom.name[88],"F L2");		strcpy(mkkom.name1[88],"F L2"); mkkom.len[88]=1;
strcpy(mkkom.name2[88],"FL2");		mkkom.par[88]=1;
 strcpy(mkkom.name[89],"F X>=0");	strcpy(mkkom.name1[89],"F x&ge;0"); mkkom.len[89]=1;
strcpy(mkkom.name2[89],"FX>=0");	mkkom.par[89]=1;
 strcpy(mkkom.name[90],"F L3");		strcpy(mkkom.name1[90],"F L3"); mkkom.len[90]=1;
strcpy(mkkom.name2[90],"FL3");		mkkom.par[90]=1;
 strcpy(mkkom.name[91],"F L1");		strcpy(mkkom.name1[91],"F L1"); mkkom.len[91]=1;
strcpy(mkkom.name2[91],"FL1");		mkkom.par[91]=1;
 strcpy(mkkom.name[92],"F X<0");	strcpy(mkkom.name1[92],"F x&lt;0"); mkkom.len[92]=1;
strcpy(mkkom.name2[92],"FX<0");		mkkom.par[92]=1;
 strcpy(mkkom.name[93],"F L0");		strcpy(mkkom.name1[93],"F L0"); mkkom.len[93]=1;
strcpy(mkkom.name2[93],"FL0");		mkkom.par[93]=1;
 strcpy(mkkom.name[94],"F X=0");	strcpy(mkkom.name1[94],"F x=0"); mkkom.len[94]=1;
strcpy(mkkom.name2[94],"FX=0");		mkkom.par[94]=1;
 strcpy(mkkom.name[95],".DB 5Fh");
strcpy(mkkom.name2[95],"?");

 strcpy(mkkom.name[96],"RM 0");		strcpy(mkkom.name1[96],"ИП 0");
strcpy(mkkom.name2[96],"RM0");
 strcpy(mkkom.name[97],"RM 1");		strcpy(mkkom.name1[97],"ИП 1");
strcpy(mkkom.name2[97],"RM1");
 strcpy(mkkom.name[98],"RM 2");		strcpy(mkkom.name1[98],"ИП 2");
strcpy(mkkom.name2[98],"RM2");
 strcpy(mkkom.name[99],"RM 3");		strcpy(mkkom.name1[99],"ИП 3");
strcpy(mkkom.name2[99],"RM3");
 strcpy(mkkom.name[100],"RM 4");	strcpy(mkkom.name1[100],"ИП 4");
strcpy(mkkom.name2[100],"RM4");
 strcpy(mkkom.name[101],"RM 5");	strcpy(mkkom.name1[101],"ИП 5");
strcpy(mkkom.name2[101],"RM5");
 strcpy(mkkom.name[102],"RM 6");	strcpy(mkkom.name1[102],"ИП 6");
strcpy(mkkom.name2[102],"RM6");
 strcpy(mkkom.name[103],"RM 7");	strcpy(mkkom.name1[103],"ИП 7");
strcpy(mkkom.name2[103],"RM7");
 strcpy(mkkom.name[104],"RM 8");	strcpy(mkkom.name1[104],"ИП 8");
strcpy(mkkom.name2[104],"RM8");
 strcpy(mkkom.name[105],"RM 9");	strcpy(mkkom.name1[105],"ИП 9");
strcpy(mkkom.name2[105],"RM9");
 strcpy(mkkom.name[106],"RM A");	strcpy(mkkom.name1[106],"ИП A");
strcpy(mkkom.name2[106],"RMA");
 strcpy(mkkom.name[107],"RM B");	strcpy(mkkom.name1[107],"ИП B");
strcpy(mkkom.name2[107],"RMB");
 strcpy(mkkom.name[108],"RM C");	strcpy(mkkom.name1[108],"ИП C");
strcpy(mkkom.name2[108],"RMC");
 strcpy(mkkom.name[109],"RM D");	strcpy(mkkom.name1[109],"ИП D");
strcpy(mkkom.name2[109],"RMD");
 strcpy(mkkom.name[110],"RM E");	strcpy(mkkom.name1[110],"ИП E");
strcpy(mkkom.name2[110],"RME");
 strcpy(mkkom.name[111],"P RM");	strcpy(mkkom.name1[111],"P ИП"); mkkom.len[111]=1;
strcpy(mkkom.name2[111],"PRM");		mkkom.par[111]=3;

 strcpy(mkkom.name[112],"K X!=0 0");	strcpy(mkkom.name1[112],"K x&ne;0 0");
strcpy(mkkom.name2[112],"KX!=00");
 strcpy(mkkom.name[113],"K X!=0 1");	strcpy(mkkom.name1[113],"K x&ne;0 1");
strcpy(mkkom.name2[113],"KX!=01");
 strcpy(mkkom.name[114],"K X!=0 2");	strcpy(mkkom.name1[114],"K x&ne;0 2");
strcpy(mkkom.name2[114],"KX!=02");
 strcpy(mkkom.name[115],"K X!=0 3");	strcpy(mkkom.name1[115],"K x&ne;0 3");
strcpy(mkkom.name2[115],"KX!=03");
 strcpy(mkkom.name[116],"K X!=0 4");	strcpy(mkkom.name1[116],"K x&ne;0 4");
strcpy(mkkom.name2[116],"KX!=04");
 strcpy(mkkom.name[117],"K X!=0 5");	strcpy(mkkom.name1[117],"K x&ne;0 5");
strcpy(mkkom.name2[117],"KX!=05");
 strcpy(mkkom.name[118],"K X!=0 6");	strcpy(mkkom.name1[118],"K x&ne;0 6");
strcpy(mkkom.name2[118],"KX!=06");
 strcpy(mkkom.name[119],"K X!=0 7");	strcpy(mkkom.name1[119],"K x&ne;0 7");
strcpy(mkkom.name2[119],"KX!=07");
 strcpy(mkkom.name[120],"K X!=0 8");	strcpy(mkkom.name1[120],"K x&ne;0 8");
strcpy(mkkom.name2[120],"KX!=08");
 strcpy(mkkom.name[121],"K X!=0 9");	strcpy(mkkom.name1[121],"K x&ne;0 9");
strcpy(mkkom.name2[121],"KX!=09");
 strcpy(mkkom.name[122],"K X!=0 A");	strcpy(mkkom.name1[122],"K x&ne;0 A");
strcpy(mkkom.name2[122],"KX!=0A");
 strcpy(mkkom.name[123],"K X!=0 B");	strcpy(mkkom.name1[123],"K x&ne;0 B");
strcpy(mkkom.name2[123],"KX!=0B");
 strcpy(mkkom.name[124],"K X!=0 C");	strcpy(mkkom.name1[124],"K x&ne;0 C");
strcpy(mkkom.name2[124],"KX!=0C");
 strcpy(mkkom.name[125],"K X!=0 D");	strcpy(mkkom.name1[125],"K x&ne;0 D");
strcpy(mkkom.name2[125],"KX!=0D");
 strcpy(mkkom.name[126],"K X!=0 E");	strcpy(mkkom.name1[126],"K x&ne;0 E");
strcpy(mkkom.name2[126],"KX!=0E");
 strcpy(mkkom.name[127],"PK X!=0"); 	strcpy(mkkom.name1[127],"PK x&ne;0"); mkkom.len[127]=1;
strcpy(mkkom.name2[127],"PKX!=0");	mkkom.par[127]=3;

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
strcpy(mkkom.name2[143],"PKGOTO");	mkkom.par[143]=3;

 strcpy(mkkom.name[144],"K X>=0 0");	strcpy(mkkom.name1[144],"K x&ge;0 0");
strcpy(mkkom.name2[144],"KX>=00");
 strcpy(mkkom.name[145],"K X>=0 1");	strcpy(mkkom.name1[145],"K x&ge;0 1");
strcpy(mkkom.name2[145],"KX>=01");
 strcpy(mkkom.name[146],"K X>=0 2");	strcpy(mkkom.name1[146],"K x&ge;0 2");
strcpy(mkkom.name2[146],"KX>=02");
 strcpy(mkkom.name[147],"K X>=0 3");	strcpy(mkkom.name1[147],"K x&ge;0 3");
strcpy(mkkom.name2[147],"KX>=03");
 strcpy(mkkom.name[148],"K X>=0 4");	strcpy(mkkom.name1[148],"K x&ge;0 4");
strcpy(mkkom.name2[148],"KX>=04");
 strcpy(mkkom.name[149],"K X>=0 5");	strcpy(mkkom.name1[149],"K x&ge;0 5");
strcpy(mkkom.name2[149],"KX>=05");
 strcpy(mkkom.name[150],"K X>=0 6");	strcpy(mkkom.name1[150],"K x&ge;0 6");
strcpy(mkkom.name2[150],"KX>=06");
 strcpy(mkkom.name[151],"K X>=0 7");	strcpy(mkkom.name1[151],"K x&ge;0 7");
strcpy(mkkom.name2[151],"KX>=07");
 strcpy(mkkom.name[152],"K X>=0 8");	strcpy(mkkom.name1[152],"K x&ge;0 8");
strcpy(mkkom.name2[152],"KX>=08");
 strcpy(mkkom.name[153],"K X>=0 9");	strcpy(mkkom.name1[153],"K x&ge;0 9");
strcpy(mkkom.name2[153],"KX>=09");
 strcpy(mkkom.name[154],"K X>=0 A");	strcpy(mkkom.name1[154],"K x&ge;0 A");
strcpy(mkkom.name2[154],"KX>=0A");
 strcpy(mkkom.name[155],"K X>=0 B");	strcpy(mkkom.name1[155],"K x&ge;0 B");
strcpy(mkkom.name2[155],"KX>=0B");
 strcpy(mkkom.name[156],"K X>=0 C");	strcpy(mkkom.name1[156],"K x&ge;0 C");
strcpy(mkkom.name2[156],"KX>=0C");
 strcpy(mkkom.name[157],"K X>=0 D");	strcpy(mkkom.name1[157],"K x&ge;0 D");
strcpy(mkkom.name2[157],"KX>=0D");
 strcpy(mkkom.name[158],"K X>=0 E");	strcpy(mkkom.name1[158],"K x&ge;0 E");
strcpy(mkkom.name2[158],"KX>=0E");
 strcpy(mkkom.name[159],"PK X>=0"); 	strcpy(mkkom.name1[159],"PK x&ge;0"); mkkom.len[159]=1;
strcpy(mkkom.name2[159],"PKX>=0");	mkkom.par[159]=3;

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
strcpy(mkkom.name2[175],"PKGSB");	mkkom.par[175]=3;

 strcpy(mkkom.name[176],"K M 0");	strcpy(mkkom.name1[176],"K П 0");
strcpy(mkkom.name2[176],"KM0");
 strcpy(mkkom.name[177],"K M 1");	strcpy(mkkom.name1[177],"K П 1");
strcpy(mkkom.name2[177],"KM1");
 strcpy(mkkom.name[178],"K M 2");	strcpy(mkkom.name1[178],"K П 2");
strcpy(mkkom.name2[178],"KM2");
 strcpy(mkkom.name[179],"K M 3");	strcpy(mkkom.name1[179],"K П 3");
strcpy(mkkom.name2[179],"KM3");
 strcpy(mkkom.name[180],"K M 4");	strcpy(mkkom.name1[180],"K П 4");
strcpy(mkkom.name2[180],"KM4");
 strcpy(mkkom.name[181],"K M 5");	strcpy(mkkom.name1[181],"K П 5");
strcpy(mkkom.name2[181],"KM5");
 strcpy(mkkom.name[182],"K M 6");	strcpy(mkkom.name1[182],"K П 6");
strcpy(mkkom.name2[182],"KM6");
 strcpy(mkkom.name[183],"K M 7");	strcpy(mkkom.name1[183],"K П 7");
strcpy(mkkom.name2[183],"KM7");
 strcpy(mkkom.name[184],"K M 8");	strcpy(mkkom.name1[184],"K П 8");
strcpy(mkkom.name2[184],"KM8");
 strcpy(mkkom.name[185],"K M 9");	strcpy(mkkom.name1[185],"K П 9");
strcpy(mkkom.name2[185],"KM9");
 strcpy(mkkom.name[186],"K M A");	strcpy(mkkom.name1[186],"K П A");
strcpy(mkkom.name2[186],"KMA");
 strcpy(mkkom.name[187],"K M B");	strcpy(mkkom.name1[187],"K П B");
strcpy(mkkom.name2[187],"KMB");
 strcpy(mkkom.name[188],"K M C");	strcpy(mkkom.name1[188],"K П C");
strcpy(mkkom.name2[188],"KMC");
 strcpy(mkkom.name[189],"K M D");	strcpy(mkkom.name1[189],"K П D");
strcpy(mkkom.name2[189],"KMD");
 strcpy(mkkom.name[190],"K M E");	strcpy(mkkom.name1[190],"K П E");
strcpy(mkkom.name2[190],"KME");
 strcpy(mkkom.name[191],"PK M"); 	strcpy(mkkom.name1[191],"PK П"); mkkom.len[191]=1;
strcpy(mkkom.name2[191],"PKM");		mkkom.par[191]=3;

 strcpy(mkkom.name[192],"K X<0 0");	strcpy(mkkom.name1[192],"K x&lt;0 0");
strcpy(mkkom.name2[192],"KX<00");
 strcpy(mkkom.name[193],"K X<0 1");	strcpy(mkkom.name1[193],"K x&lt;0 1");
strcpy(mkkom.name2[193],"KX<01");
 strcpy(mkkom.name[194],"K X<0 2");	strcpy(mkkom.name1[194],"K x&lt;0 2");
strcpy(mkkom.name2[194],"KX<02");
 strcpy(mkkom.name[195],"K X<0 3");	strcpy(mkkom.name1[195],"K x&lt;0 3");
strcpy(mkkom.name2[195],"KX<03");
 strcpy(mkkom.name[196],"K X<0 4");	strcpy(mkkom.name1[196],"K x&lt;0 4");
strcpy(mkkom.name2[196],"KX<04");
 strcpy(mkkom.name[197],"K X<0 5");	strcpy(mkkom.name1[197],"K x&lt;0 5");
strcpy(mkkom.name2[197],"KX<05");
 strcpy(mkkom.name[198],"K X<0 6");	strcpy(mkkom.name1[198],"K x&lt;0 6");
strcpy(mkkom.name2[198],"KX<06");
 strcpy(mkkom.name[199],"K X<0 7");	strcpy(mkkom.name1[199],"K x&lt;0 7");
strcpy(mkkom.name2[199],"KX<07");
 strcpy(mkkom.name[200],"K X<0 8");	strcpy(mkkom.name1[200],"K x&lt;0 8");
strcpy(mkkom.name2[200],"KX<08");
 strcpy(mkkom.name[201],"K X<0 9");	strcpy(mkkom.name1[201],"K x&lt;0 9");
strcpy(mkkom.name2[201],"KX<09");
 strcpy(mkkom.name[202],"K X<0 A");	strcpy(mkkom.name1[202],"K x&lt;0 A");
strcpy(mkkom.name2[202],"KX<0A");
 strcpy(mkkom.name[203],"K X<0 B");	strcpy(mkkom.name1[203],"K x&lt;0 B");
strcpy(mkkom.name2[203],"KX<0B");
 strcpy(mkkom.name[204],"K X<0 C");	strcpy(mkkom.name1[204],"K x&lt;0 C");
strcpy(mkkom.name2[204],"KX<0C");
 strcpy(mkkom.name[205],"K X<0 D");	strcpy(mkkom.name1[205],"K x&lt;0 D");
strcpy(mkkom.name2[205],"KX<0D");
 strcpy(mkkom.name[206],"K X<0 E");	strcpy(mkkom.name1[206],"K x&lt;0 E");
strcpy(mkkom.name2[206],"KX<0E");
 strcpy(mkkom.name[207],"PK X<0"); 	strcpy(mkkom.name1[207],"PK x&lt;0"); mkkom.len[207]=1;
strcpy(mkkom.name2[207],"PKX<0");	mkkom.par[207]=3;

 strcpy(mkkom.name[208],"K RM 0");	strcpy(mkkom.name1[208],"K ИП 0");  //KИП0
strcpy(mkkom.name2[208],"KRM0");
 strcpy(mkkom.name[209],"K RM 1");	strcpy(mkkom.name1[209],"K ИП 1");
strcpy(mkkom.name2[209],"KRM1");
 strcpy(mkkom.name[210],"K RM 2");	strcpy(mkkom.name1[210],"K ИП 2");
strcpy(mkkom.name2[210],"KRM2");
 strcpy(mkkom.name[211],"K RM 3");	strcpy(mkkom.name1[211],"K ИП 3");
strcpy(mkkom.name2[211],"KRM3");
 strcpy(mkkom.name[212],"K RM 4");	strcpy(mkkom.name1[212],"K ИП 4");
strcpy(mkkom.name2[212],"KRM4");
 strcpy(mkkom.name[213],"K RM 5");	strcpy(mkkom.name1[213],"K ИП 5");
strcpy(mkkom.name2[213],"KRM5");
 strcpy(mkkom.name[214],"K RM 6");	strcpy(mkkom.name1[214],"K ИП 6");
strcpy(mkkom.name2[214],"KRM6");
 strcpy(mkkom.name[215],"K RM 7");	strcpy(mkkom.name1[215],"K ИП 7");
strcpy(mkkom.name2[215],"KRM7");
 strcpy(mkkom.name[216],"K RM 8");	strcpy(mkkom.name1[216],"K ИП 8");
strcpy(mkkom.name2[216],"KRM8");
 strcpy(mkkom.name[217],"K RM 9");	strcpy(mkkom.name1[217],"K ИП 9");
strcpy(mkkom.name2[217],"KRM9");
 strcpy(mkkom.name[218],"K RM A");	strcpy(mkkom.name1[218],"K ИП A");
strcpy(mkkom.name2[218],"KRMA");
 strcpy(mkkom.name[219],"K RM B");	strcpy(mkkom.name1[219],"K ИП B");
strcpy(mkkom.name2[219],"KRMB");
 strcpy(mkkom.name[220],"K RM C");	strcpy(mkkom.name1[220],"K ИП C");
strcpy(mkkom.name2[220],"KRMC");
 strcpy(mkkom.name[221],"K RM D");	strcpy(mkkom.name1[221],"K ИП D");
strcpy(mkkom.name2[221],"KRMD");
 strcpy(mkkom.name[222],"K RM E");	strcpy(mkkom.name1[222],"K ИП E");
strcpy(mkkom.name2[222],"KRME");
 strcpy(mkkom.name[223],"PK RM"); 	strcpy(mkkom.name1[223],"PK ИП"); mkkom.len[223]=1;
strcpy(mkkom.name2[223],"PKRM");	mkkom.par[223]=3;

 strcpy(mkkom.name[224],"K X=0 0");	strcpy(mkkom.name1[224],"K x=0 0");
strcpy(mkkom.name2[224],"KX=00");
 strcpy(mkkom.name[225],"K X=0 1");	strcpy(mkkom.name1[225],"K x=0 1");
strcpy(mkkom.name2[225],"KX=01");
 strcpy(mkkom.name[226],"K X=0 2");	strcpy(mkkom.name1[226],"K x=0 2");
strcpy(mkkom.name2[226],"KX=02");
 strcpy(mkkom.name[227],"K X=0 3");	strcpy(mkkom.name1[227],"K x=0 3");
strcpy(mkkom.name2[227],"KX=03");
 strcpy(mkkom.name[228],"K X=0 4");	strcpy(mkkom.name1[228],"K x=0 4");
strcpy(mkkom.name2[228],"KX=04");
 strcpy(mkkom.name[229],"K X=0 5");	strcpy(mkkom.name1[229],"K x=0 5");
strcpy(mkkom.name2[229],"KX=05");
 strcpy(mkkom.name[230],"K X=0 6");	strcpy(mkkom.name1[230],"K x=0 6");
strcpy(mkkom.name2[230],"KX=06");
 strcpy(mkkom.name[231],"K X=0 7");	strcpy(mkkom.name1[231],"K x=0 7");
strcpy(mkkom.name2[231],"KX=07");
 strcpy(mkkom.name[232],"K X=0 8");	strcpy(mkkom.name1[232],"K x=0 8");
strcpy(mkkom.name2[232],"KX=08");
 strcpy(mkkom.name[233],"K X=0 9");	strcpy(mkkom.name1[233],"K x=0 9");
strcpy(mkkom.name2[233],"KX=09");
 strcpy(mkkom.name[234],"K X=0 A");	strcpy(mkkom.name1[234],"K x=0 A");
strcpy(mkkom.name2[234],"KX=0A");
 strcpy(mkkom.name[235],"K X=0 B");	strcpy(mkkom.name1[235],"K x=0 B");
strcpy(mkkom.name2[235],"KX=0B");
 strcpy(mkkom.name[236],"K X=0 C");	strcpy(mkkom.name1[236],"K x=0 C");
strcpy(mkkom.name2[236],"KX=0C");
 strcpy(mkkom.name[237],"K X=0 D");	strcpy(mkkom.name1[237],"K x=0 D");
strcpy(mkkom.name2[237],"KX=0D");
 strcpy(mkkom.name[238],"K X=0 E");	strcpy(mkkom.name1[238],"K x=0 E");
strcpy(mkkom.name2[238],"KX=0E");
 strcpy(mkkom.name[239],"PK X=0"); 	strcpy(mkkom.name1[239],"PK x=0"); mkkom.len[239]=1;
strcpy(mkkom.name2[239],"PKX=0");	mkkom.par[239]=3;

 strcpy(mkkom.name[240],".DB 0F0h");	strcpy(mkkom.name1[240],"F0h");
strcpy(mkkom.name2[240],"?");
 strcpy(mkkom.name[241],"P GOTO");	strcpy(mkkom.name1[241],"P БП"); mkkom.len[241]=2; //PБП
strcpy(mkkom.name2[241],"PGOTO");	mkkom.par[241]=2;
 strcpy(mkkom.name[242],"PP RTN");	strcpy(mkkom.name1[242],"PP В/О");
strcpy(mkkom.name2[242],"PPRTN");
 strcpy(mkkom.name[243],"P GSB");	strcpy(mkkom.name1[243],"P ПП"); mkkom.len[243]=2; //PПП
strcpy(mkkom.name2[243],"PGSB");	mkkom.par[243]=2;
 strcpy(mkkom.name[244],"PP M");	strcpy(mkkom.name1[244],"PP П"); mkkom.len[244]=2; //PPП
strcpy(mkkom.name2[244],"PPM");		mkkom.par[244]=4;
 strcpy(mkkom.name[245],".DB 0F5h");	strcpy(mkkom.name1[245],"F5h");
strcpy(mkkom.name2[245],"?");
 strcpy(mkkom.name[246],"PP RM");	strcpy(mkkom.name1[246],"PP ИП"); mkkom.len[246]=2;
strcpy(mkkom.name2[246],"PPRM");	mkkom.par[246]=4;
 strcpy(mkkom.name[247],"P X!=0");	strcpy(mkkom.name1[247],"P x&ne;0"); mkkom.len[247]=2;
strcpy(mkkom.name2[247],"PX!=0");	mkkom.par[247]=2;
 strcpy(mkkom.name[248],"PF L2");	strcpy(mkkom.name1[248],"PF L2"); mkkom.len[248]=2;
strcpy(mkkom.name2[248],"PFL2");	mkkom.par[248]=2;
 strcpy(mkkom.name[249],"P X>=0");	strcpy(mkkom.name1[249],"P x&ge;0"); mkkom.len[249]=2;
strcpy(mkkom.name2[249],"PX>=0");	mkkom.par[249]=2;
 strcpy(mkkom.name[250],"PF L3");	strcpy(mkkom.name1[250],"PF L3"); mkkom.len[250]=2;
strcpy(mkkom.name2[250],"PFL3");	mkkom.par[250]=2;
 strcpy(mkkom.name[251],"PF L1");	strcpy(mkkom.name1[251],"PF L1"); mkkom.len[251]=2;
strcpy(mkkom.name2[251],"PFL1");	mkkom.par[251]=2;
 strcpy(mkkom.name[252],"P X<0");	strcpy(mkkom.name1[252],"P x&lt;0"); mkkom.len[252]=2;
strcpy(mkkom.name2[252],"PX<0");	mkkom.par[252]=2;
 strcpy(mkkom.name[253],"PF L0");	strcpy(mkkom.name1[253],"PF L0"); mkkom.len[253]=2;
strcpy(mkkom.name2[253],"PFL0");	mkkom.par[253]=2;
 strcpy(mkkom.name[254],"P X=0");	strcpy(mkkom.name1[254],"P x=0"); mkkom.len[254]=2;
strcpy(mkkom.name2[254],"PX=0");	mkkom.par[254]=2;
 strcpy(mkkom.name[255],".DB 0FFh");	strcpy(mkkom.name1[255],"FFh");
strcpy(mkkom.name2[255],"?");

end: return(re);
}

/*=====================================================================================*/

int init_mkfunk()
{

// инициализация описания функций
int i;
int re=0;

if (mkfunk.init!=0) goto end;

for (i=0; i<1000; i++)
	{
	if (i<100) mkfunk.rname[i][0]=0;	// нет назв раздела
	mkfunk.name[i][0]=0;	// названия нет
	mkfunk.opis[i][0]=0;	// описания нет
	mkfunk.r[i]=-1;	// нет раздела
	}

strcpy(mkfunk.rname[0],"Графический экран");
mkfunk.r[0]=0;
strcpy(mkfunk.name[0],"Начальные координаты объекта");
strcpy(mkfunk.opis[0],"X=0-127, Y=0-63");
mkfunk.r[1]=0;
strcpy(mkfunk.name[1],"Атрибуты вывода");
strcpy(mkfunk.opis[1],"X=0-замещение, 1-AND, 2-OR, 3-XOR, 4-7 - то же с инверсией");
mkfunk.r[2]=0;
strcpy(mkfunk.name[2],"Расстояние между символами ");
strcpy(mkfunk.opis[2],"X=0-15");
mkfunk.r[3]=0;
strcpy(mkfunk.name[3],"Номер шрифта");
strcpy(mkfunk.opis[3],"X=0-2 - стандартные, 254 - загружаемый"); //v1.18 ПО МК-152
mkfunk.r[4]=0;
strcpy(mkfunk.name[4],"Индексный регистр экрана");
strcpy(mkfunk.opis[4],"X=0-1023");
mkfunk.r[5]=0;
strcpy(mkfunk.name[5],"Работа с графической областью памяти");
strcpy(mkfunk.opis[5],"X - байт графического экрана");
mkfunk.r[6]=0;
strcpy(mkfunk.name[6],"Работа с графической областью памяти, автоинкремент");
strcpy(mkfunk.opis[6],"X - байт графического экрана");
mkfunk.r[7]=0;
strcpy(mkfunk.name[7],"Прокрутка экрана с заполнением 0");
strcpy(mkfunk.opis[7],"X=1-8 число строк");
mkfunk.r[8]=0;
strcpy(mkfunk.name[8],"Прокрутка экрана с заполнением 1");
strcpy(mkfunk.opis[8],"X=1-8 число строк");
mkfunk.r[9]=0;
strcpy(mkfunk.name[9],"Адрес загружаемого шрифта");	// v1.18 ПО МК-152
strcpy(mkfunk.opis[9],"X=0-9999 - адрес начала структуры шрифта");
mkfunk.r[10]=0;
strcpy(mkfunk.name[10],"Инициализация экрана");
strcpy(mkfunk.opis[10],"X=0,2-белый, 1,3-черный, 2,3-с установкой др. регистров");
mkfunk.r[11]=0;
strcpy(mkfunk.name[11],"Вывод точки");
strcpy(mkfunk.opis[11],"X, Y");
mkfunk.r[12]=0;
strcpy(mkfunk.name[12],"Вывод линии");
strcpy(mkfunk.opis[12],"X, Y и начальные координаты");
mkfunk.r[13]=0;
strcpy(mkfunk.name[13],"Вывод прямоугольника");
strcpy(mkfunk.opis[13],"X, Y и начальные координаты");
mkfunk.r[14]=0;
strcpy(mkfunk.name[14],"Вывод рамки");
strcpy(mkfunk.opis[14],"X, Y и начальные координаты");
mkfunk.r[15]=0;
strcpy(mkfunk.name[15],"Вывод образа из памяти программ");
strcpy(mkfunk.opis[15],"X-адрес");
mkfunk.r[16]=0;
strcpy(mkfunk.name[16],"Вывод образа из памяти данных");
strcpy(mkfunk.opis[16],"X-регистр");
mkfunk.r[20]=0;
strcpy(mkfunk.name[20],"Вывод символа");
strcpy(mkfunk.opis[20],"X-код");
mkfunk.r[21]=0;
strcpy(mkfunk.name[21],"Вывод строки из памяти программ");
strcpy(mkfunk.opis[21],"X-адрес");
mkfunk.r[22]=0;
strcpy(mkfunk.name[22],"Вывод строки из памяти данных");
strcpy(mkfunk.opis[22],"X-регистр");
mkfunk.r[23]=0;
strcpy(mkfunk.name[23],"Вывод числа");
strcpy(mkfunk.opis[23],"X-число");
mkfunk.r[24]=0;
strcpy(mkfunk.name[24],"Вывод числа в форматированном виде");
strcpy(mkfunk.opis[24],"X-число, Y=1-8 число знаков целой части, Z=0-8 дробной");


strcpy(mkfunk.rname[1],"Строка комментариев");
mkfunk.r[25]=1;
strcpy(mkfunk.name[25],"Вывод символа");
strcpy(mkfunk.opis[25],"X=0-255 - код");
mkfunk.r[26]=1;
strcpy(mkfunk.name[26],"Вывод строки из памяти программ");
strcpy(mkfunk.opis[26],"X-адрес");
mkfunk.r[27]=1;
strcpy(mkfunk.name[27],"Вывод строки из памяти данных");
strcpy(mkfunk.opis[27],"X-регистр");


strcpy(mkfunk.rname[2],"Буфер клавиатуры");
mkfunk.r[28]=2;
strcpy(mkfunk.name[28],"Чтение состояния клавиатуры");
strcpy(mkfunk.opis[28],"При чтении в X-код нажатой клавиши");
mkfunk.r[29]=2;
strcpy(mkfunk.name[29],"Чтение кода клавиши и очистка буфера");
strcpy(mkfunk.opis[29],"При чтении в X-код, при записи д.б. не 0 для очистки");


strcpy(mkfunk.rname[3],"Универсальный байтовый буфер");
mkfunk.r[30]=3;
strcpy(mkfunk.name[30],"Индексный регистр 0");
strcpy(mkfunk.opis[30],"X=0-255");
mkfunk.r[31]=3;
strcpy(mkfunk.name[31],"Индексный регистр 1");
strcpy(mkfunk.opis[31],"X=0-255");
mkfunk.r[32]=3;
strcpy(mkfunk.name[32],"Запись и чтение по индексному регистру 0");
strcpy(mkfunk.opis[32],"");
mkfunk.r[33]=3;
strcpy(mkfunk.name[33],"Запись и чтение по индексному регистру 1");
strcpy(mkfunk.opis[33],"");
mkfunk.r[34]=3;
strcpy(mkfunk.name[34],"Запись и чтение по индексному регистру 1, автоинкемент");
strcpy(mkfunk.opis[34],"");
mkfunk.r[35]=3;
strcpy(mkfunk.name[35],"Преобразование числа в строку");
strcpy(mkfunk.opis[35],"X-число");
mkfunk.r[36]=3;
strcpy(mkfunk.name[36],"Преобразование числа в строку в форматированном виде");
strcpy(mkfunk.opis[36],"X-число, Y=1-8 число знаков целой части, Z=0-8 дробной");
mkfunk.r[37]=3;
strcpy(mkfunk.name[37],"Преобразование числа во внутренний формат и обратно");
strcpy(mkfunk.opis[37],"X-число");
mkfunk.r[38]=3;
strcpy(mkfunk.name[38],"Преобразование числа в формат float и обратно");
strcpy(mkfunk.opis[38],"X-число");
mkfunk.r[39]=3;
strcpy(mkfunk.name[39],"Преобразование кода команды в мнемонику");
strcpy(mkfunk.opis[39],"X-адрес");


strcpy(mkfunk.rname[4],"Дополнительные функции");
mkfunk.r[40]=4;
strcpy(mkfunk.name[40],"Вычисление четности байта");
strcpy(mkfunk.opis[40],"");
mkfunk.r[41]=4;
strcpy(mkfunk.name[41],"Текущее значение счетчика адреса");
strcpy(mkfunk.opis[41],"");
mkfunk.r[42]=4;
strcpy(mkfunk.name[42],"Индексный регистр памяти программ");
strcpy(mkfunk.opis[42],"");
mkfunk.r[43]=4;
strcpy(mkfunk.name[43],"Чтение памяти программ");
strcpy(mkfunk.opis[43],"");
mkfunk.r[44]=4;
strcpy(mkfunk.name[44],"Чтение памяти программ, автоинкремент");
strcpy(mkfunk.opis[44],"");
mkfunk.r[45]=4;
strcpy(mkfunk.name[45],"Размерность аргумента тригонометрических функций");
strcpy(mkfunk.opis[45],"X=0-градусы, 1-радианы, 2-грады");
mkfunk.r[46]=4;
strcpy(mkfunk.name[46],"Инициализация генератора СЧ");
strcpy(mkfunk.opis[46],"X=0-65535");
mkfunk.r[47]=4;
strcpy(mkfunk.name[47],"Разрешение записи в энергонезависимую память");
strcpy(mkfunk.opis[47],"X=0-запрещено, 1-разрешено, автосброс");
mkfunk.r[48]=4;
strcpy(mkfunk.name[48],"Разрешение автоматического запуска программы");
strcpy(mkfunk.opis[48],"X=0-запрещено, 1-разрешено");
mkfunk.r[49]=4;
strcpy(mkfunk.name[49],"Задание сетевого номера");
strcpy(mkfunk.opis[49],"X=0-255");


strcpy(mkfunk.rname[5],"Функции реального времени");
mkfunk.r[50]=5;
strcpy(mkfunk.name[50],"Формирование интервалов (таймер 0)");
strcpy(mkfunk.opis[50],"X=0-65535 -10 мс");
mkfunk.r[51]=5;
strcpy(mkfunk.name[51],"Формирование интервалов (таймер 1)");
strcpy(mkfunk.opis[51],"X=0-65535 в 10 мс");
mkfunk.r[52]=5;
strcpy(mkfunk.name[52],"Звуковой сигнал");
strcpy(mkfunk.opis[52],"X-длительность в 10 мс, Y-частота, Гц");
mkfunk.r[53]=5;
strcpy(mkfunk.name[53],"Звуковой сигнал клавиатуры");
strcpy(mkfunk.opis[53],"X-длительность в 10 мс, Y-частота, Гц");
mkfunk.r[55]=5;
strcpy(mkfunk.name[55],"Чтение и корректировка времени");
strcpy(mkfunk.opis[55],"X=ЧЧ.ММССсс");
mkfunk.r[56]=5;
strcpy(mkfunk.name[56],"Число");
strcpy(mkfunk.opis[56],"X=1-31");
mkfunk.r[57]=5;
strcpy(mkfunk.name[57],"Месяц");
strcpy(mkfunk.opis[57],"X=1-12");
mkfunk.r[58]=5;
strcpy(mkfunk.name[58],"Год");
strcpy(mkfunk.opis[58],"X=0-9999");	// V1.09 для v1.09 МК-152 и выше, до этого 0-99
mkfunk.r[59]=5;
strcpy(mkfunk.name[59],"День недели");
strcpy(mkfunk.opis[59],"X=1-7");


strcpy(mkfunk.rname[6],"Электронный блокнот");
mkfunk.r[60]=6;
strcpy(mkfunk.name[60],"Группа записей");
strcpy(mkfunk.opis[60],"X=0-16383");
mkfunk.r[61]=6;
strcpy(mkfunk.name[61],"Запись 0");
strcpy(mkfunk.opis[61],"");
mkfunk.r[62]=6;
strcpy(mkfunk.name[62],"Запись 1");
strcpy(mkfunk.opis[62],"");
mkfunk.r[63]=6;
strcpy(mkfunk.name[63],"Запись 2");
strcpy(mkfunk.opis[63],"");
mkfunk.r[64]=6;
strcpy(mkfunk.name[64],"Запись 3");
strcpy(mkfunk.opis[64],"");
mkfunk.r[65]=6;
strcpy(mkfunk.name[65],"Регистр состояния блокнота");
strcpy(mkfunk.opis[65],"X=0-нормальное завершение, 1-ошибка, только чтение");


strcpy(mkfunk.rname[7],"Последовательный порт");
mkfunk.r[70]=7;
strcpy(mkfunk.name[70],"Разрешение порта");
strcpy(mkfunk.opis[70],"X=0-запрещен, 1-разрешен");
mkfunk.r[71]=7;
strcpy(mkfunk.name[71],"Скорость обмена");
strcpy(mkfunk.opis[71],"X=0-13: 5-2400, 7-4800, 9-9600, 11-19200 и др.");
mkfunk.r[72]=7;
strcpy(mkfunk.name[72],"Режим обмена");
strcpy(mkfunk.opis[72],"X=0-8 бит, 1-9 бит");
mkfunk.r[73]=7;
strcpy(mkfunk.name[73],"Время ожидания при приеме");
strcpy(mkfunk.opis[73],"X=0-255 по 1,25 мс");
mkfunk.r[74]=7;
strcpy(mkfunk.name[74],"Режим автоматического приема");
strcpy(mkfunk.opis[74],"X=0-включен, 1-выключен");
mkfunk.r[75]=7;
strcpy(mkfunk.name[75],"Счетчик посылок");
strcpy(mkfunk.opis[75],"X=0-255, только чтение");
mkfunk.r[76]=7;
strcpy(mkfunk.name[76],"Прием данных");
strcpy(mkfunk.opis[76],"X=0-запрещен, 1-разрешен");
mkfunk.r[77]=7;
strcpy(mkfunk.name[77],"Передача данных");
strcpy(mkfunk.opis[77],"X-число посылок 0-255");

mkfunk.r[78]=7;
strcpy(mkfunk.name[78],"Время ожидания ответа");
strcpy(mkfunk.opis[78],"X=0-255 по 1,25 мс");
mkfunk.r[79]=7;
strcpy(mkfunk.name[79],"Текущее значение времени ожидания ответа");
strcpy(mkfunk.opis[79],"X-0-255 по 1,25 мс");

mkfunk.r[80]=7;
strcpy(mkfunk.name[80],"Индексный регистр буфера передачи");
strcpy(mkfunk.opis[80],"");
mkfunk.r[81]=7;
strcpy(mkfunk.name[81],"Байт буфера передачи");
strcpy(mkfunk.opis[81],"");
mkfunk.r[82]=7;
strcpy(mkfunk.name[82],"Байт буфера передачи, автоинкремент");
strcpy(mkfunk.opis[82],"");
mkfunk.r[83]=7;
strcpy(mkfunk.name[83],"Бит 9 буфера передачи");
strcpy(mkfunk.opis[83],"");
mkfunk.r[84]=7;
strcpy(mkfunk.name[84],"Бит 9 буфера передачи, автоинкремент");
strcpy(mkfunk.opis[84],"");
mkfunk.r[85]=7;
strcpy(mkfunk.name[85],"Индексный регистр буфера приема");
strcpy(mkfunk.opis[85],"");
mkfunk.r[86]=7;
strcpy(mkfunk.name[86],"Байт буфера передачи");
strcpy(mkfunk.opis[86],"");
mkfunk.r[87]=7;
strcpy(mkfunk.name[87],"Байт буфера передачи, автоинкремент");
strcpy(mkfunk.opis[87],"");
mkfunk.r[88]=7;
strcpy(mkfunk.name[88],"Бит 9 буфера передачи");
strcpy(mkfunk.opis[88],"");
mkfunk.r[89]=7;
strcpy(mkfunk.name[89],"Бит 9 буфера передачи, автоинкремент");
strcpy(mkfunk.opis[89],"");


strcpy(mkfunk.rname[8],"Параллельный порт");
mkfunk.r[90]=8;
strcpy(mkfunk.name[90],"Регистр режима");
strcpy(mkfunk.opis[90],"X=0-выключен, 1-статический, 2-Centronics, 3-двунаправленный");
mkfunk.r[91]=8;
strcpy(mkfunk.name[91],"Сигналы параллельного порта");
strcpy(mkfunk.opis[91],"Содержимое зависит от режима");
mkfunk.r[92]=8;
strcpy(mkfunk.name[92],"Сигналы параллельного порта");
strcpy(mkfunk.opis[92],"Содержимое зависит от режима");
mkfunk.r[93]=8;
strcpy(mkfunk.name[93],"Тип операции");
strcpy(mkfunk.opis[93],"Статич. реж.: 0-чтение, 1-запись. Двунапр. реж: 0-данные, 1-адрес");
mkfunk.r[94]=8;
strcpy(mkfunk.name[94],"Регистр состояния выполненной команды");
strcpy(mkfunk.opis[94],"0-нормальное завершение, 1-принудительный останов, 2-не готов");
mkfunk.r[95]=8;
strcpy(mkfunk.name[95],"Время ожидания ответа");
strcpy(mkfunk.opis[95],"X=0-255 по 1,25 мс");


strcpy(mkfunk.rname[9],"Работа с двоичными числами");
mkfunk.r[100]=9;
strcpy(mkfunk.name[100],"Разрядность двоичного числа");
strcpy(mkfunk.opis[100],"X от 1 до 4 байт");
mkfunk.r[101]=9;
strcpy(mkfunk.name[101],"Вывод на графический экран");
strcpy(mkfunk.opis[101],"Число в X");
mkfunk.r[102]=9;
strcpy(mkfunk.name[102],"Вывод в строку комментариев");
strcpy(mkfunk.opis[102],"Число в X");
mkfunk.r[103]=9;
strcpy(mkfunk.name[103],"Преобразование в последовательность байтов и обратно");
strcpy(mkfunk.opis[103],"Число X в универсальный байтовый буфер с 0 индекса");

// ПО МК-152 1.07 и выше

strcpy(mkfunk.rname[10],"Работа с файлами из программы пользователя");
mkfunk.r[120]=10;
strcpy(mkfunk.name[120],"Общее разрешение дисковых операций");
strcpy(mkfunk.opis[120],"X=0 - запрещены, 1 - разрешены");
mkfunk.r[121]=10;
strcpy(mkfunk.name[121],"Выбор диска");
strcpy(mkfunk.opis[121],"X=0 - диск А-внутренний, 1 - В-внешний");
mkfunk.r[122]=10;
strcpy(mkfunk.name[122],"Чтение строки каталога");
strcpy(mkfunk.opis[122],"X - номер строки текущего каталога");
mkfunk.r[123]=10;
strcpy(mkfunk.name[123],"Загрузка файла/каталога");
strcpy(mkfunk.opis[123],"X - номер строки каталога, Y - нач. страница для программ");
mkfunk.r[124]=10;
strcpy(mkfunk.name[124],"Удаление файла/каталога");
strcpy(mkfunk.opis[124],"X - номер строки каталога");
mkfunk.r[125]=10;
strcpy(mkfunk.name[125],"Создание файла/каталога");
strcpy(mkfunk.opis[125],"X=2-каталог, X=4,5,6 - файлы, имя в буфере");
mkfunk.r[126]=10;
strcpy(mkfunk.name[126],"Перезапись файла");
strcpy(mkfunk.opis[126],"X - номер строки каталога");
mkfunk.r[128]=10;
strcpy(mkfunk.name[128],"Разрешение операции");
strcpy(mkfunk.opis[128],"X - аналогично разрешаемой операции");
mkfunk.r[129]=10;
strcpy(mkfunk.name[129],"Ошибки дисковых операций");
strcpy(mkfunk.opis[129],"X - код ошибки, 0-нет, только чтение");

strcpy(mkfunk.rname[11],"Прерывания");
mkfunk.r[130]=11;
strcpy(mkfunk.name[130],"Общее разрешение прерываний");
strcpy(mkfunk.opis[130],"X=0 - запрещены, 1 - разрешены");
mkfunk.r[131]=11;
strcpy(mkfunk.name[131],"Регистр настройки прерывания 1");
strcpy(mkfunk.opis[131],"X=0 -запр., 1-255 - приоритет,  Y - вектор");
mkfunk.r[132]=11;
strcpy(mkfunk.name[132],"Регистр настройки прерывания 2");
strcpy(mkfunk.opis[132],"X=0 -запр., 1-255 - приоритет,  Y - вектор");
mkfunk.r[133]=11;
strcpy(mkfunk.name[133],"Регистр настройки прерывания 3");
strcpy(mkfunk.opis[133],"X=0 -запр., 1-255 - приоритет,  Y - вектор");
mkfunk.r[134]=11;
strcpy(mkfunk.name[134],"Регистр настройки прерывания 4");
strcpy(mkfunk.opis[134],"X=0 -запр., 1-255 - приоритет,  Y - вектор");
mkfunk.r[135]=11;
strcpy(mkfunk.name[135],"Регистр настройки прерывания 5");
strcpy(mkfunk.opis[135],"X=0 -запр., 1-255 - приоритет,  Y - вектор");
mkfunk.r[136]=11;
strcpy(mkfunk.name[136],"Регистр настройки прерывания 6");
strcpy(mkfunk.opis[136],"X=0 -запр., 1-255 - приоритет,  Y - вектор");
mkfunk.r[137]=11;
strcpy(mkfunk.name[137],"Регистр настройки прерывания 7");
strcpy(mkfunk.opis[137],"X=0 -запр., 1-255 - приоритет,  Y - вектор");
mkfunk.r[138]=11;
strcpy(mkfunk.name[138],"Регистр настройки прерывания 8");
strcpy(mkfunk.opis[138],"X=0 -запр., 1-255 - приоритет,  Y - вектор");
mkfunk.r[139]=11;
strcpy(mkfunk.name[139],"Регистр настройки прерывания 9");
strcpy(mkfunk.opis[139],"X=0 -запр., 1-255 - приоритет,  Y - вектор");
mkfunk.r[140]=11;
strcpy(mkfunk.name[140],"Регистр настройки прерывания 10");
strcpy(mkfunk.opis[140],"X=0 -запр., 1-255 - приоритет,  Y - вектор");
mkfunk.r[141]=11;
strcpy(mkfunk.name[141],"Регистр настройки прерывания 11");
strcpy(mkfunk.opis[141],"X=0 -запр., 1-255 - приоритет,  Y - вектор");
mkfunk.r[142]=11;
strcpy(mkfunk.name[142],"Регистр настройки прерывания 12");
strcpy(mkfunk.opis[142],"X=0 -запр., 1-255 - приоритет,  Y - вектор");
mkfunk.r[143]=11;
strcpy(mkfunk.name[143],"Регистр настройки прерывания 13");
strcpy(mkfunk.opis[143],"X=0 -запр., 1-255 - приоритет,  Y - вектор");
mkfunk.r[144]=11;
strcpy(mkfunk.name[144],"Регистр настройки прерывания 14");
strcpy(mkfunk.opis[144],"X=0 -запр., 1-255 - приоритет,  Y - вектор");
mkfunk.r[145]=11;
strcpy(mkfunk.name[145],"Регистр настройки прерывания 15");
strcpy(mkfunk.opis[145],"X=0 -запр., 1-255 - приоритет,  Y - вектор");
mkfunk.r[146]=11;
strcpy(mkfunk.name[146],"Регистр настройки прерывания 16");
strcpy(mkfunk.opis[146],"X=0 -запр., 1-255 - приоритет,  Y - вектор");

mkfunk.r[148]=11;
strcpy(mkfunk.name[148],"Сброс всех регистров прерываний");	// Реализовано с версии 1.08 ПО МК-152
strcpy(mkfunk.opis[148],"X=1 - сброс");
mkfunk.r[149]=11;
strcpy(mkfunk.name[149],"Программный вызов прерываний");
strcpy(mkfunk.opis[149],"X=1-16 - номер прерывания");


mkfunk.r[150]=11;
strcpy(mkfunk.name[150],"Прерывание таймера 0");
strcpy(mkfunk.opis[150],"X=0 -запр, 1-16 - номер прерывания");
mkfunk.r[151]=11;
strcpy(mkfunk.name[151],"Прерывание таймера 1");
strcpy(mkfunk.opis[151],"X=0 -запр, 1-16 - номер прерывания");
mkfunk.r[152]=11;
strcpy(mkfunk.name[152],"Прерывание таймера 2 - зв. сигн.");
strcpy(mkfunk.opis[152],"X=0 -запр, 1-16 - номер прерывания");
mkfunk.r[153]=11;
strcpy(mkfunk.name[153],"Прерывание клавиатуры");
strcpy(mkfunk.opis[153],"X=0 -запр, 1-16 - номер прерывания");
mkfunk.r[154]=11;
strcpy(mkfunk.name[154],"Окончание передачи посл. порта");
strcpy(mkfunk.opis[154],"X=0 -запр, 1-16 - номер прерывания");
mkfunk.r[155]=11;
strcpy(mkfunk.name[155],"Окончание приема посл. порта");
strcpy(mkfunk.opis[155],"X=0 -запр, 1-16 - номер прерывания");
mkfunk.r[156]=11;
strcpy(mkfunk.name[156],"BUSY параллельного порта");
strcpy(mkfunk.opis[156],"X=0 -запр, 1-16 - номер прерывания");
mkfunk.r[157]=11;
strcpy(mkfunk.name[157],"Прерывание по отпусканию клавиши");
strcpy(mkfunk.opis[157],"X=0 -запр, 1-16 - номер прерывания");
mkfunk.r[158]=11;
strcpy(mkfunk.name[158],"Окончание ожидания ответа посл. порта");
strcpy(mkfunk.opis[158],"X=0 -запр, 1-16 - номер прерывания");

// Функции ПО МК-152 v1.14, ПО МК-161 v1.01

mkfunk.r[110]=4;
strcpy(mkfunk.name[110],"Установка разрядности RY, RZ, RT при выводе");
strcpy(mkfunk.opis[110],"X=0 -8 разр., 1 - 14 разр.");

mkfunk.r[115]=4;
strcpy(mkfunk.name[115],"Напряжение автономного источника, В");
strcpy(mkfunk.opis[115],"только чтение");

mkfunk.r[116]=4;
strcpy(mkfunk.name[116],"Номер модели");
strcpy(mkfunk.opis[116],"только чтение");

mkfunk.r[117]=4;
strcpy(mkfunk.name[117],"Номер версии ПО");
strcpy(mkfunk.opis[117],"только чтение");


strcpy(mkfunk.rname[12],"АЦП");		// ADC
mkfunk.r[170]=12;
strcpy(mkfunk.name[170],"Режим работы АЦП");
strcpy(mkfunk.opis[170],"X=0 - изм. мгновенных значений, 1 - непр.");

mkfunk.r[171]=12;
strcpy(mkfunk.name[171],"Настройка мультиплексора");
strcpy(mkfunk.opis[171],"X=0 - выкл, 1-4 - вкл., количество каналов.");

mkfunk.r[172]=12;
strcpy(mkfunk.name[172],"Множитель периода дискретизации");
strcpy(mkfunk.opis[172],"X=1-8 в 1,25 мс. для R9179");

mkfunk.r[175]=12;
strcpy(mkfunk.name[175],"Канал 0, измеренное значение, В");
strcpy(mkfunk.opis[175],"только чтение");

mkfunk.r[176]=12;
strcpy(mkfunk.name[176],"Канал 1, измеренное значение, В");
strcpy(mkfunk.opis[176],"только чтение");

mkfunk.r[177]=12;
strcpy(mkfunk.name[177],"Канал 2, измеренное значение, В");
strcpy(mkfunk.opis[177],"только чтение");

mkfunk.r[178]=12;
strcpy(mkfunk.name[178],"Канал 3, измеренное значение, В");
strcpy(mkfunk.opis[178],"только чтение");

mkfunk.r[179]=12;
strcpy(mkfunk.name[179],"Запись массива мгновенных значений в двоичную память");
strcpy(mkfunk.opis[179],"X=1-2048 - число отсчетов");

mkfunk.r[180]=12;
strcpy(mkfunk.name[180],"Канал 0, постоянная времени цифрового фильтра");
strcpy(mkfunk.opis[180],"X=0-10 - от 10 мс до 10,24 с, умн. на кол-во каналов");

mkfunk.r[181]=12;
strcpy(mkfunk.name[181],"Канал 1, постоянная времени цифрового фильтра");
strcpy(mkfunk.opis[181],"X=0-10 - от 10 мс до 10,24 с, умн. на кол-во каналов");

mkfunk.r[182]=12;
strcpy(mkfunk.name[182],"Канал 2, постоянная времени цифрового фильтра");
strcpy(mkfunk.opis[182],"X=0-10 - от 10 мс до 10,24 с, умн. на кол-во каналов");

mkfunk.r[183]=12;
strcpy(mkfunk.name[183],"Канал 3, постоянная времени цифрового фильтра");
strcpy(mkfunk.opis[183],"X=0-10 - от 10 мс до 10,24 с, умн. на кол-во каналов");

mkfunk.r[185]=12;
strcpy(mkfunk.name[185],"Канал 0, нефильтрованное значение, В");
strcpy(mkfunk.opis[185],"только чтение");

mkfunk.r[186]=12;
strcpy(mkfunk.name[186],"Канал 1, нефильтрованное значение, В");
strcpy(mkfunk.opis[186],"только чтение");

mkfunk.r[187]=12;
strcpy(mkfunk.name[187],"Канал 2, нефильтрованное значение, В");
strcpy(mkfunk.opis[187],"только чтение");

mkfunk.r[188]=12;
strcpy(mkfunk.name[188],"Канал 3, нефильтрованное значение, В");
strcpy(mkfunk.opis[188],"только чтение");


strcpy(mkfunk.rname[13],"Последовательный периферийный интерфейс");	// SPI
mkfunk.r[190]=13;
strcpy(mkfunk.name[190],"Режим работы SPI");
strcpy(mkfunk.opis[190],"X=0-3");

mkfunk.r[191]=13;
strcpy(mkfunk.name[191],"Обмен данными по SPI");
strcpy(mkfunk.opis[191],"X=1-255 - число байт");

// ПО МК-152 v1.18 и МК-161 v1.06
strcpy(mkfunk.rname[14],"Графический интерфейс пользователя");	// GUI
mkfunk.r[200]=14;
strcpy(mkfunk.name[200],"Регистр разрешения ГИ");
strcpy(mkfunk.opis[200],"X=1 - разрешён, 0 - нет");

mkfunk.r[201]=14;
strcpy(mkfunk.name[201],"Адрес главной таблицы ГИ");
strcpy(mkfunk.opis[201],"X=0-9999");

mkfunk.r[202]=14;
strcpy(mkfunk.name[202],"Номер экранной формы");
strcpy(mkfunk.opis[202],"X=0-255");

mkfunk.r[203]=14;
strcpy(mkfunk.name[203],"Номер активного элемента");
strcpy(mkfunk.opis[203],"X=0-255");

mkfunk.r[204]=14;
strcpy(mkfunk.name[204],"Регистр действий экранной формы");
strcpy(mkfunk.opis[204],"X=0-255, битовые поля");

mkfunk.r[206]=14;
strcpy(mkfunk.name[206],"Период автоматического обновления экрана");
strcpy(mkfunk.opis[206],"X=0-255, дискретность 10 мс");

mkfunk.r[207]=14;
strcpy(mkfunk.name[207],"Адрес регистра для ввода чисел");
strcpy(mkfunk.opis[207],"X=0-999");

mkfunk.r[208]=14;
strcpy(mkfunk.name[208],"Регистр команд интерфейса");
strcpy(mkfunk.opis[208],"X-код команды");

mkfunk.r[209]=14;
strcpy(mkfunk.name[209],"Регистр результата выполнения операции");
strcpy(mkfunk.opis[209],"X=0 - успешно, 1 - требуется продолжение, 2-6 ошибки");


// совместимые устройства
strcpy(mkfunk.rname[15],"Интерфейс 1-Wire (для устройства \"Электроника MC-1105\")");

mkfunk.r[800]=15;
strcpy(mkfunk.name[800],"Сброс шины (чтение)");
strcpy(mkfunk.opis[800],"X=0 -нет, 1-есть, 2-замыкание");

mkfunk.r[801]=15;
strcpy(mkfunk.name[801],"Передача байта (запись)");
strcpy(mkfunk.opis[801],"X=0-255");

mkfunk.r[802]=15;
strcpy(mkfunk.name[802],"Прием байта (чтение)");
strcpy(mkfunk.opis[802],"Байт в RX");

mkfunk.r[803]=15;
strcpy(mkfunk.name[803],"Запуск преобразования температуры");
strcpy(mkfunk.opis[803],"");

mkfunk.r[804]=15;
strcpy(mkfunk.name[804],"Чтение значения температуры");
strcpy(mkfunk.opis[804],"X - номер датчика");

mkfunk.init=1;

end: return(re);
}


/*==================================================================================*/
int read_mkp()
{
/* считывание файла MKP в буфер для дальнейшего разбора */
int re=0;
int i,j,k;
long l,l2;

for (i=0; i<10008; i++) mkbufp[i]=0x1FF;	/* 0FFh HEX */

i=fgetc(fp1);
if (i==EOF)  {re=EX_IOERR; goto end;}

i%=256;
if (i<0) i=0;
if (i>=100) {re=EX_DATAERR; goto end;}

gcomp_progmin=i;
l=100*i;
gcomp_proglen=0;
l2=0;

m1:	i=fgetc(fp1);
	if (i==EOF) goto end;

	j=fgetc(fp1);
	if (i==EOF) goto end;

	k=256*j+i;
	mkbufp[l]=k;
	l++;
	l2++;
	if (l>=10000) {re=EX_DATAERR; goto end;}
	goto m1;

end:

mkbufp[10000]=mkbufp[0];	/* на всякий случай - для вычисления адресов через границу 10000 */
mkbufp[10001]=mkbufp[1];

if (l2==0) re=EX_DATAERR;
if ((l2%100)==0) gcomp_proglen=l2/100;
	else {gcomp_proglen=l2/100; gcomp_proglen++;} /* если размер файла не был кратен странице */
if (gcomp_progmin + gcomp_proglen >100) {re=EX_DATAERR; gcomp_proglen=100-gcomp_progmin;}

return(re);
}

/*--------------------------------------------- Контрольные суммы --------------*/
int ks_mkp(void)
{
int re=0;
int i,ic,j;

// вычислить контрольную сумму
gcomp_progks1=0;
gcomp_progks2=0;

/* КС всей программы */
for (i=0; i<100*gcomp_proglen; i++)
	{
	ic=(int)(mkbufp[100*gcomp_progmin+i] % 256);	// мл часть
	if (ic<0) ic+=256;
	gcomp_progks1+=ic;
	gcomp_progks2^=ic;
	}

/* КС отдельных страниц */
for (i=0; i<100; i++)
	{
	ks1[i]=0;
	ks2[i]=0;
	for (j=0; j<100; j++)
		{
		ic=(int)(mkbufp[100*i+j] % 256);	// мл часть
		if (ic<0) ic+=256;
		ks1[i]+=ic;
		ks2[i]^=ic;
		}
	}

return(re);
}

/*===========================================================================*/
int mkp2text(void)
{
int re=0;
int i,j,k;
long l,l1,l2;
long lmax;
int ic;
char ics[8];


// простой дизассемблер
l=0; // текущий адрес относительно начала файла
l1=100*gcomp_progmin;
lmax=100*(gcomp_progmin+gcomp_proglen);


m00: // цикл по убиранию FFh с конца файла
i=mkbufp[lmax-1];
if (((i==0xFF)||(i==0x1FF)) && (lmax>l1+1)) {lmax--; goto m00;}

m0:
j=0; // 0-интерпретировать байт как команду, 1 и 2 - как адрес или номер регистра.

// HTML - формат
strcpy(str, "<HTML>\r\n<HEAD><TITLE>");
str2file(4);
strncpy(str,fname[0],256);
str2file(4);
strcpy(str,"</TITLE><META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=windows-1251\"></HEAD>\r\n<BODY>\r\n");
str2file(4);
sprintf(str,"<P>Файл %s (КС %d/%d)</P>\r\n",fname[1],gcomp_progks1,gcomp_progks2);
str2file(4);
strcpy(str,"<TABLE BORDER=1 CELLSPACING=0 CELLPADDING=5 width=100%><TBODY>\r\n");
str2file(4);
strcpy(str,"<TR>\r\n<TD>&nbsp;</TD>\r\n<TD>0</TD>\r\n<TD>1</TD>\r\n<TD>2</TD>\r\n<TD>3</TD>\r\n<TD>4</TD>\r\n<TD>5</TD>\r\n<TD>6</TD>\r\n<TD>7</TD>\r\n<TD>8</TD>\r\n<TD>9</TD>\r\n</TR>\r\n\r\n");
str2file(4);

mph0:
	str[0]=0;
	if ((l%10)==0)
		{strcat(str, "<TR>\r\n<TD>");
		sprintf(ctemp,"%d",l1);
		 if (l1<10) strcat(str,"0");
		 if ((l1<100)&&(lmax>100)) strcat(str,"0");
		 if ((l1<1000)&&(lmax>1000)) strcat(str,"0");
		 strcat(str, ctemp);
		 strcat(str, "</TD>\r\n");
		}

	strcat(str, "<TD>");
	// мнемоника ic
	ic=(int)mkbufp[l1];
		if (ic<0) ic+=256;
		if (ic<0) ic=0;

	if ((ic>=256)&&(ic<512))
		{
		// HEX
		if (lmax-l<10)	// конец программы - удалить все FFh
			{
			k=0;
			for (l2=l; l2<lmax; l2++)
				{if (mkbufp[l2]!=0x1FF) k=1;} // больше нет байт !=FFh ?
			if ((k==0)&&(ic==511))
				{strcat(str,"&nbsp;");
				goto mph01;
				}
			}

		ic-=256;
		sprintf(ctemp,"%X",ic);
		if (ic<16) strcat(str, "0");
		strcat(str, ctemp);
		strcat (str,"h");
		goto mph01;
		}

	if ((ic>=512)&&(ic<768))
		{
		// TEXT
		ic-=512;
		sprintf(ctemp,"%X",ic);
		if (ic<16) strcat(str, "0");
		strcat(str, ctemp);
		strcat (str,"h");
		if (ic>=32)
			{ // TEXT
			ics[0]=ic; ics[1]=0;
			dos2win(ics);
			strcat(str," '");
			strcat(str,ics);
			strcat(str,"'");
			}
		goto mph01;
		}

	if (ic<256)
		{ // обычный байт
		if	(j==0)
			{
			// команда
			strcat(str, mkkom.name1[ic]); // name1 - HTML формат
			j=mkkom.len[ic];
			if (j<0) j=0;
			if (j>2) j=2;
			}
		else
			{
			// адрес или регистр если он есть
			sprintf(ctemp,"%X",ic);
			if (ic<16) strcat(str, "0");
			strcat(str, ctemp);
			if (j>0) j--;
			if (j<0) j=0;
			}
		goto mph01;
		}

	if (ic>=768)
		{ // ошибка v1.18 >=768
		ic%=256;
		sprintf(ctemp,"%X",ic);
		if (ic<16) strcat(str, "0");
		strcat(str, ctemp);
		strcat(str,"h ?");
		}

mph01:	strcat(str, "</TD>\r\n");
	str2file(4);

	l++;
	l1++;

	if ((l%10)==0) {strcpy(str, "</TR>\r\n\r\n");
			str2file(4);
			}

	if (l<lmax) goto mph0;

	k=l%10;
	if (k!=0)
		{
		// если lmax%10!=0
		for (i=k; i<10; i++)
				{strcpy(str, "<TD>&nbsp;</TD>\r\n");
				str2file(4);
				}
		strcpy(str, "</TR>\r\n\r\n");
		str2file(4);
		}

	strcpy(str, "</TBODY></TABLE>\r\n\r\n");
	str2file(4);

	if (gcomp_proglen>1)
		{
		/* больше одной страницы - записать таблицу контрольных сумм */
		strcpy(str, "<hr><P>Контрольные суммы отдельных страниц</P>\r\n");
		str2file(4);
		strcpy(str,"<TABLE BORDER=1 CELLSPACING=0 CELLPADDING=5 width=100%><TBODY>\r\n");
		str2file(4);
		strcpy(str,"<TR><TD>Страница</TD><TD>ADD</TD><TD>XOR</TD>\r\n");
		str2file(4);

		for (i=gcomp_progmin; i<gcomp_progmin+gcomp_proglen; i++)
			{
			sprintf(str,"<TR><TD>%d</TD><TD>%d</TD><TD>%d</TD></TR>\r\n",i,ks1[i],ks2[i]);
			str2file(4);
			}

		sprintf(str,"<TR><TD>Все</TD><TD>%d</TD><TD>%d</TD></TR>\r\n</TBODY></TABLE>\r\n\r\n",gcomp_progks1,gcomp_progks2);
		str2file(4);
		}

	// окончание htm-файла не добавлять - далее записать листинг MKL

// TXT листинг

l=0; // текущий адрес относительно начала файла
l1=100*gcomp_progmin;
//lmax=100*(gcomp_progmin+gcomp_proglen);

j=0; // 0-интерпретировать байт как команду, 1 и 2 - как адрес или номер регистра.

	// вывести адрес
mp0:	str[0]=0;
	sprintf(ctemp,"%d",l1);

	if (l1<10) strcat(str,"0");
	if (l1<100) strcat(str,"0");
	if (l1<1000) strcat(str,"0");
	strcat(str, ctemp);
	strcat(str," \t");

	// шестнадцатиричное значение байта
	ic=(int)mkbufp[l1];
		if (ic<0) ic+=256;
		if (ic<0) ic=0;

	sprintf(ctemp,"%X",ic%256);
	if ((ic%256)<16) strcat(str, "0");
	strcat(str, ctemp);
	strcat(str,"h \t");

	// мнемоника ic

	if ((ic>=256)&&(ic<768))
		{
		// HEX & TEXT
		//ic-=256;
		sprintf(ctemp,"%X",ic%256);
		if ((ic%256)<16) strcat(str, "0");
		strcat(str, ctemp);
		strcat (str,"h");		// HEX-отладка
		k=3;
		goto mp01;
		}

	if (ic<256)
		{
		if (j==0)
			{
			// команда
			strcat(str, mkkom.name[ic]);
			k=strlen(mkkom.name[ic]);
			if (k<0) k=0;
			strcat(str, " ");
			j=mkkom.len[ic];
			if (j<0) j=0;
			if (j>2) j=2;
			// адрес или регистр если он есть

			if (j>0)
				{
				ic=(int)mkbufp[l1+1]%256;
					if (ic<0) ic+=256;
					if (ic<0) ic=0;

				sprintf(ctemp,"%X",ic);
				if (ic<16) strcat(str, "0");
				strcat(str, ctemp);
				k+=2;
				}
			if (j>1)
				{
				ic=(int)mkbufp[l1+2]%256;
					if (ic<0) ic+=256;
					if (ic<0) ic=0;
				sprintf(ctemp,"%X",ic);
				if (ic<16) strcat(str, "0");
				strcat(str, ctemp);
				k+=2;
				}
			}
		else
			{
			// адрес или регистр - нет мнемоники
			if (j>0) j--;
			if (j<0) j=0;
			k=0;
			}
		goto mp01;
	} // end ic<256

	if (ic>=768)
		{
		// ошибка
		ic%=256;
		sprintf(ctemp,"%X",ic);
		if (ic<16) strcat(str, "0");
		strcat(str, ctemp);
		strcat (str,"h ?");
		k=5;
		}
	// выровнять строку комментария текущая длина в k
mp01:
	for (i=k; i<16; i++) strcat(str," ");
	strcat(str, "\t;");

	// символ
	ic=(int)mkbufp[l1]%256;
	if (ic<0) ic+=256;
	if (ic<0) ic=0;
	// DOS 2 Win
	ics[0]=ic%256;
	ics[1]=0;
	dos2win(ics);

	strcat(str,ics);
	strcat(str,"\r\n");
	str2file(3);

	l++;
	l1++;
	if (l<lmax) goto mp0;

	// Добавить комментарии с КС
	sprintf(str,"; Контрольная сумма ADD %d \r\n",gcomp_progks1);
	str2file(3);
	sprintf(str,"; Контрольная сумма XOR %d \r\n",gcomp_progks2%256);
	str2file(3);

return(re);
}

/*=================================================================*/

int compil_p()
{
// Декомпилятор программы в исходный код
// выполнять после заполнения структур команд и функций и записи начала HTM-файла
int re=0;
int i,j,k,k1,k2,kh,kl,m;
int pn,pe,adr;
int flag_adres;	// 3-короткий, 2-дл ст байт, 1 - дл мл байт
int flag_hex;	// это число hex и его номер по порядку, если не первое (.DB)
int flag_ahex;	// при переходе внутрь команды
int flag_funk;	// номер функции не 0 - вставить комментарий о назначении
int flag_metka;
int flag_text;	// текст
int flag_chislo; // число

long l,l1;
char temp[256];
char ics[4];	//для вывода TEXT

// на первом проходе создать список всех меток в структуре DMETKA

pn=100*gcomp_progmin;	// начало
if (pn<0) pn=0;
pe=100*(gcomp_progmin+gcomp_proglen); // конец
if (pe<pn) pe=pn;
if (pe>10000) pe=10000;

m00: // цикл по убиранию FFh с конца файла
i=mkbufp[pe-1];
if (((i==0xFF)||(i==0x1FF)) && (pe>pn+1)) {pe--; goto m00;}


adr=0;	// текущий адрес
flag_adres=0;
m=0;

dmetka.vsego=0;

for (i=gcomp_progmin; i<=gcomp_progmin+gcomp_proglen; i++)
	{ // i=страница
	for (j=0; j<100; j++)
		{ // j - байт на странице
		adr=100*i+j;
		if (adr<0) adr=0;
		k=mkbufp[adr];
		if (adr<9998) {k1=mkbufp[adr+1]; k2=mkbufp[adr+2];}
		if (adr==9998) {k1=mkbufp[9999]; k2=mkbufp[0];}
		if (adr==9999) {k1=mkbufp[0]; k2=mkbufp[1];}
		if (adr>=10000) {k1=0; k2=0;}

		if (adr==(pe-1)) {k1=0xFF; k2=0xFF;}	// за концом файла
		if (adr==(pe-2)) k2=0xFF;

		if (k<0) k+=256;
		if (k<0) k=0;
		if (k1<0) k1+=256;
		if (k1<0) k1=0;
		if (k2<0) k2+=256;
		if (k2<0) k2=0;

		k1%=256;	// убрать признак HEX - это адрес
		k2%=256;	// убрать признак HEX - это адрес


			// оператор - проверить на наличие адреса после него
			m=0;	// тек метка - обнулить

			if ((k==0x51)||(k==0x53)||((k>=0x57)&&(k<=0x5E)) )
				{//flag_adres=3;	// короткий адрес
				// сразу записать адрес текущей метки
				dmetka.adr[dmetka.vsego]=adr;
				dmetka.reg[dmetka.vsego]=0; // это адрес метки

				m=100*i;	// именно так, ибо страница считается по месту нахождения адреса!
				// преобразовать k

				kh=k1/16;
				kl=k1%16;
				m+=10*kh+kl;
				//flag_adres=0;
				goto sm;	// установить метку в dmetka
				}


			if ((k==0xF1)||(k==0xF3)||((k>=0xF7)&&(k<=0xFE)) )
				{//flag_adres=2;	// длинный адрес
				// сразу записать адрес текущей метки
				dmetka.adr[dmetka.vsego]=adr;
				dmetka.reg[dmetka.vsego]=0; // это адрес метки

				kh=k1/16;
				kl=k1%16;
				m=1000*kh+100*kl;
				kh=k2/16;
				kl=k2%16;
				m+=10*kh+kl;
				goto sm;	// установить метку в dmetka
				}

			if ((k==0x4F)||(k==0x6F)||(k==0x7F)||(k==0x8F)||(k==0x9F)||(k==0xAF)||
				(k==0xBF)||(k==0xCF)||(k==0xDF)||(k==0xEF))
				{//flag_adres=3;	// регистр 1 байт
				// записать адрес
				dmetka.adr[dmetka.vsego]=adr;
				dmetka.reg[dmetka.vsego]=1; // это номер регистра 1 байт

				kh=k1/16;
				kl=k1%16;
				m=10*kh+kl;
				//flag_adres=0;
				goto sm;	// установить метку в dmetka
				}

			if ((k==0xF4)||(k==0xF6))
				{//flag_adres=2;	// длинный регистр
				// сразу записать адрес текущей метки
				dmetka.adr[dmetka.vsego]=adr;
				dmetka.reg[dmetka.vsego]=2; // это номер регистра 2 байта

				kh=k1/16;
				kl=k1%16;
				m=1000*kh+100*kl;
				kh=k2/16;
				kl=k2%16;
				m+=10*kh+kl;
				goto sm;
				}
			goto next_j;

sm:		// установить метку в структуру dmetka
		dmetka.val[dmetka.vsego]=m;
		// адрес dmetka.adr был задан раньше
		if (dmetka.vsego<10000) dmetka.vsego++;
next_j:		// конец цикла по j
		k=0;	// лишний оператор
		}
	}

// Дополнить HTM-файл листингом MKL
sprintf(str,"<hr><h4>Исходный текст для кросс-компилятора</h4><P>Файл %s </P>\r\n<pre>\r\n",fname[2]);
str2file(4);

// Разбор операторов и формирование листинга
	flag_adres=0;
	flag_hex=0;
	flag_funk=0;
	flag_text=0;
	flag_chislo=0;

	strcpy(str,".CHARSET 1251\r\n");
	str2file(2);
	sprintf(str,"; Файл %s \r\n",fname[0]);
	str2file(2);
	sprintf(str,".ORG %d \r\n",pn);
	str2file(2);

for (adr=pn; adr<pe; adr++)
	{
	// в adr - адрес
	str[0]=0;	// в str - команда
	str1[0]=0;

	if (flag_adres>0) goto op11;	// пропустить адрес, проверка на переход внутрь отдельно

	k=mkbufp[adr];

	if ((k>=256)&&(k<512))
		{ // HEX
		flag_adres=0;
		flag_funk=0;
		if (flag_chislo>0)
			{
			strcat(str," ");
			str2file(2);
			str[0]=0;
			flag_chislo=0;
			}
		if (flag_text>0)
			{
			strcat(str,"\"\r\n");
			str2file(2);
			str[0]=0;
			flag_text=0;
			}
		flag_hex++;
		k-=256;
		sprintf(ctemp,"%X",k);
		if (((k>0)&&(k<16))||(k>0x9F)) strcat(str,"0");
		strcat(str,ctemp);
		strcat(str,"h");
		goto op;
		}

	if ((k>=512)&&(k<768))
		{
		// TEXT
		flag_hex=0;
		if (flag_chislo>0)
			{
			strcat(str," ");
			str2file(2);
			str[0]=0;
			flag_chislo=0;
			}
		flag_adres=0;
		flag_funk=0;

		k1=k;
		k%=256;

		if (((k>=32)&&(k<176)) || ((k>=224)&&(k<242)))
			{
			ics[0]=k%256;
			ics[1]=0;
			dos2win(ics);
			strcat(str,ics);
			flag_text++;
			}
		else   	{
			if (flag_text>0)
				{
				strcat(str,"\"\r\n");
				str2file(2);
				str[0]=0;
				flag_text=0;
				}
			sprintf(ctemp,"%X",k);
			strcat (str,".DB ");
			if (((k>0)&&(k<16))||(k>0x9F)) strcat(str,"0");
			strcat(str,ctemp);
			strcat(str,"h \t; ");

			sprintf(ctemp,"%02Xh ",k);
			strcat(str,ctemp);
			}
		goto op;
		}

	if (k>=768)
		{ // HEX, но с ошибкой!
		flag_hex=0;
		if (flag_chislo>0)
			{
			strcat(str," ");
			str2file(2);
			str[0]=0;
			flag_chislo=0;
			}
		flag_adres=0;
		flag_funk=0;
		if (flag_text>0)
			{
			strcat(str,"\"\r\n");
			str2file(2);
			str[0]=0;
			flag_text=0;
			}

		k1=k;
		k%=256;
		sprintf(ctemp,"%X",k);

		strcat (str,".DB ");
		if (((k>0)&&(k<16))||(k>0x9F)) strcat(str,"0");
		strcat(str,ctemp);
		strcat(str,"h");
		strcat(str," \t; ");

		sprintf(ctemp,"%X",k);
		strcat(str,ctemp);
		strcat(str,"h ?");

		goto op;
		}


	if ((k>=0)&&(k<256))
		{
		// разобрать как оператор языка
		flag_hex=0;
		flag_adres=0;
		flag_funk=0;
		if (flag_text>0)
			{
			strcat(str,"\"\r\n");
			str2file(2);
			str[0]=0;
			flag_text=0;
			}

		if ((k>12)&&(flag_chislo>0))
			{
			strcat(str,"\r\n");
			str2file(2);
			str[0]=0;
			flag_chislo=0;
			}

		// проверить наличие переходов внутрь команды
		i=mkkom.len[k];
		if (i!=0)
			{
			j=0;
			flag_ahex=0;
			for (l=0; l<dmetka.vsego; l++)
				{
				// dmetka.reg[l]==0 - пройти только по меткам
				if ((dmetka.val[l]==adr+1)&&(dmetka.reg[l]==0)) flag_ahex=i;
				if ((dmetka.val[l]==adr+2)&&(i==2)&&(dmetka.reg[l]==0)) flag_ahex=2;

				if (flag_ahex>0)
					{
					// переход внутрь есть
					// разобрать как hex
					flag_ahex--;	// и сразу уменьшить на 1
					k%=256;
					sprintf(ctemp,"%X",k);
					temp[0]=0;
					strcat(str,".DB ");
					if (((k>0)&&(k<16))||(k>0x9F)) strcat(str,"0");
					strcat(str,ctemp);
					strcat(str,"h\t; Код команды ");
					strcat(str,mkkom.name[k]);
					// эта команда из нескольких байт по определению - найти запись
					for (l1=0; l1<dmetka.vsego; l1++)
						{
						if (dmetka.adr[l1]==adr)
							{
							// записать в комментарий полный код команды
							if (dmetka.reg[l1]==0) strcat(str," A");
								else strcat(str," ");
							sprintf(ctemp,"%d",dmetka.val[l1]);
							strcat(str,ctemp);
							}
						}
					goto op;
					}
				} // end for l
			} // end i!=0

		// если нет переходов внутрь команды

		if (mkkom.par[k]==0)
			{
			// длина команды один байт
			strcpy(str,mkkom.name[k]);	// перенести только мнемонику команды
			if (k<=12) flag_chislo++;	// это число или ",", "/-/", "ВП"
				else flag_chislo=0;
			goto op;
			}

		// иначе - длина больше одного байта
		// параметр команды 0-нет; 1,2 - адрес; 3,4 - номер регистра 1 или 2 байта
		if (mkkom.par[k]>0)
			{
			flag_chislo=0;
			strcpy(str,mkkom.name[k]);	// перенести мнемонику команды
			strcat(str," ");	// и продолжить

			// найти
			for (l=0; l<dmetka.vsego; l++)
				{
				if (dmetka.adr[l]==adr)
					{
					// нашли нужную
					if (dmetka.reg[l]==0) strcat(str,"A");	// это метка

					sprintf(ctemp,"%d",dmetka.val[l]);
					strcat(str,ctemp);

					if ( (dmetka.reg[l]==0)&&((dmetka.val[l]<pn)||(dmetka.val[l]>=pe)) )
						{
						strcat(str,"\t; Ошибка - переход за границы программы");
						}

					if ((dmetka.reg[l]==2)&&(dmetka.val[l]>=9000))
						{
						flag_funk=dmetka.val[l];
						}
					flag_adres=-mkkom.len[k];	// сколько байт дальше пропустить
								// отрицательное значение - признак того, что сам оператор записать
					goto op;	// вставить оператор
					} // найденная метка или регистр
				} // цикл по меткам и регистрам
			} // если есть параметры команды
		} // конец разбора оператора

op: // вставить разобранный оператор из str в очередную строку

	str1[0]=0;	/* записать в str1 */
	// проверить есть ли ссылающаяся на этот адрес метка
	flag_metka=0;
	for (l=0; l<dmetka.vsego; l++)
		{
		if ((dmetka.val[l]==adr)&&(dmetka.reg[l]==0))
			{
			flag_metka++;
			if (flag_hex>1) {strcat(str1,"\r\n");
					flag_hex=1;	// по метке начать .DB с новой строки
					}

			if (flag_text>1) {strcat(str1,"\"\r\n");
					flag_text=1;	// по метке начать .TEXT с новой строки
					}

			// вставить источник ссылки
			if (flag_metka==1)
					{
					l1=strlen(str1);
					if ((l1>0)&&(str1[l1-1]!='\n')) strcat(str1,"\r\n");
					strcat(str1,"\r\nA");
					sprintf(ctemp,"%d",adr);
					strcat(str1,ctemp);
					strcat(str1,":\t ; с адреса ");
					}
				else strcat(str1,", ");
			sprintf(ctemp,"%d ",dmetka.adr[l]);
			strcat(str1,ctemp);
			}
		}

	if (flag_metka!=0)
		{
		// есть хоть одна метка
		strcat(str1,"\r\n");
		}

	if ((flag_chislo<2)&&(flag_hex<2)&&(flag_text<2)) strcat(str1,"\t");
	// если меток нет, и не .DB и не .TEXT - поставить в начале строки табуляцию

	// вставить оператор из str

	if (flag_hex!=0)
		{
		if (flag_hex==1)
			{
			// вставить .DB
			strcat(str1,".DB ");
			}
			else
			{
			// вставить запятую для отделения от пред числа
			strcat(str1,", ");
			}

		strcat(str1,str);
		goto op2;	// вставить конец строки
		}

	if (flag_text!=0)
		{
		if (flag_text==1)
			{
			// вставить .TEXT и открывающую кавычку
			strcat(str1,".TEXT \"");
			}

		strcat(str1,str);
		goto op2;	// вставить конец строки
		}

op11:
	if (flag_adres>0)
		{
		if (flag_adres>=3) flag_adres=1;
		flag_adres--;	// пропустить байты адреса
		goto next_adr1;	// на след адрес
		}

	if (flag_adres<0) flag_adres=-flag_adres;	// изменить знак для пропуска на следующем проходе

	strcat(str1,str);	// это просто оператор

op2: // вставить конец строки если это надо
	if (flag_chislo>0) goto next_adr;
	if (flag_text>0) goto next_adr;
	if ((flag_hex>0)&&(flag_hex<10)) goto next_adr;	// не переводить строку
	if (flag_hex>=10) flag_hex=0;	// здесь можно проверить и обнулить

	if ((flag_funk>=9000)&&(flag_funk<10000)&&(mkfunk.init==1))
		{
		// вставить описание функции
		k=mkfunk.r[flag_funk-9000];
		if (k>=0)
			{
			strcat(str1,"\t; ");
			strcat(str1,mkfunk.name[flag_funk-9000]);
			strcat(str1," (");
			strcat(str1,mkfunk.rname[k]);
			strcat(str1,")");
			}

		flag_funk=0;
		}
	strcat(str1,"\r\n");	// конец очередной строки

next_adr:
	strcpy(str,str1);
	str2file(2);		// записать в MKL
next_adr1: k=0;
	} // end for adr

strcpy(str,"\r\n.END \r\n");
str2file(2);

// Завершить HTM-файл
strcpy(str, "</pre>\r\n</BODY></HTML>\r\n");
str2file(4);

end: return(re);
}


/*================ str2file ==============*/
int str2file(int p)
{
/* запись str в fp3 (MKL p==2), fp3 (TXT p==3) или fp4 (HTM p==4) */
int i,j;
char c;
int re=0;

if ((p<2)||(p>4)) {re=EX_SOFTWARE; goto end;}

switch(p)
	{
	case 2:	for (i=0; i<strlen(str); i++)
		{
		j=fputc(str[i],fp2);
		if (j==EOF) {re=EX_IOERR; goto end;}
		}

		// Продублировать вывод MKL в HTM с заменой символов < и >
		for (i=0; i<strlen(str); i++)
		{
		c=str[i];

		if (c=='<')
			{j=fputc(0x26,fp4); // замена на &lt;
			if (j==EOF) {re=EX_IOERR; goto end;}
			j=fputc(0x6C,fp4);
			if (j==EOF) {re=EX_IOERR; goto end;}
			j=fputc(0x74,fp4);
			if (j==EOF) {re=EX_IOERR; goto end;}
			j=fputc(0x3B,fp4);
			if (j==EOF) {re=EX_IOERR; goto end;}
			}

		if (c=='>')
			{j=fputc(0x26,fp4); // замена на &gt;
			if (j==EOF) {re=EX_IOERR; goto end;}
			j=fputc(0x67,fp4);
			if (j==EOF) {re=EX_IOERR; goto end;}
			j=fputc(0x74,fp4);
			if (j==EOF) {re=EX_IOERR; goto end;}
			j=fputc(0x3B,fp4);
			if (j==EOF) {re=EX_IOERR; goto end;}
			}

		if ((c!='<')&&(c!='>'))
			{
			j=fputc(str[i],fp4);
			if (j==EOF) {re=EX_IOERR; goto end;}
			}
		}

		goto end;

	case 3: for (i=0; i<strlen(str); i++)
		{
		j=fputc(str[i],fp3);
		if (j==EOF) {re=EX_IOERR; goto end;}
		}
		goto end;

	case 4: for (i=0; i<strlen(str); i++)
		{
		j=fputc(str[i],fp4);
		if (j==EOF) {re=EX_IOERR; goto end;}
		}
		goto end;
	}

end: return(re);
}

/*======================================== MAIN ===================*/
int main(int argc, char *argv[])
{
int i,j,k;
char c;
int re=EXIT_SUCCESS;

/* Значения по умолчанию */
kluch_e=0;
fp1=fp2=fp3=fp4=NULL;
fname_vsego=0;

if (dos==0) strcpy(mes0,"\n");
	else strcpy(mes0,"\r\n");

/* разбор строки аргументов */
if (argc<=1) goto nach;
k=0;
for (i=1; i<argc; i++)
	{
	if ((argv[i][0]!='/')&&(argv[i][0]!='-'))
		{
		j=fname_vsego;
		if (j<0) j=0;
		if (j>2) j=2;
		strncpy(fname[j+1],argv[i],248);
		if (j<2) fname_vsego++;
		}

	else	{
		c=argv[i][1];
		if (c=='-') c=argv[i][2];	/* для --help и т.п.*/
		k=1;

		if ((c=='h')||(c=='H')||(c=='?'))
			{ /* help */
			printf("mkp2mkl - converter MKP to MKL, TXT & HTM files %s",mes0);
			printf("mkp2mkl  MKPfile [key] %s",mes0); /* v0.11 */
			printf("mkp2mkl [-h|-v|-w] %s",mes0);
			printf("\t-h,-H,-?\tthis help; %s",mes0);
			printf("\t-v,-V\tversion; %s",mes0);
			printf("\t-w,-W\twarranty and copyright; %s",mes0);
			printf("\t-e,-E\techo on (-e0 - echo off, default); %s%s",mes0,mes0);

			printf(mes0);
			goto end;
			}

		if ((c=='v')||(c=='V'))
			{ /* version */
			printf(version);
			if (dos==0) printf(" (GNU/Linux)");
			if (dos==1) printf(" (DOS)");
			if (dos==2) printf(" (Win32)");
			printf(mes0);
			goto end;
			}

		if ((c=='w')||(c=='W'))
			{ /* copyleft & warranty */
			printf("Copyright (C) 2011 by NPP SEMIKO (Russia, Novosibirsk) %s%s",mes0,mes0);
			printf("This program is free software; you can redistribute it and/or modify %s",mes0);
			printf("it under the terms of the GNU General Public License as published by %s",mes0);
			printf("the Free Software Foundation; either version 2 of the License, or %s",mes0);
			printf("(at your option) any later version. %s%s",mes0,mes0);

			printf("This program is distributed in the hope that it will be useful, %s",mes0);
			printf("but WITHOUT ANY WARRANTY; without even the implied warranty of %s",mes0);
			printf("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the %s",mes0);
			printf("GNU General Public License for more details. %s%s",mes0,mes0);

			printf("You should have received a copy of the GNU General Public License %s",mes0);
			printf("along with this program; if not, write to the Free Software %s",mes0);
			printf("Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA");
			printf(mes0);
			goto end;
			}


		if ((c=='e')||(c=='E'))
			{ /* echo */
			kluch_e=1;
			if (argv[i][2]=='0') kluch_e=0; /* -e0 */
			k=0;
			}

		if (k!=0) {printf("mkp2mkl -h for help %s",mes0); goto end;}
		}
	}

nach:

if (fname_vsego<=0) {re=EX_USAGE; goto end;}	/* нет файла */

fp1=fopen(fname[1],"rb");	/* открыть MKP как двоичный для чтения */
if (fp1==NULL) {re=EX_NOINPUT;
		if (kluch_e!=0) printf("\r\nMKP not found %s",mes0);
		goto end;}
/* файл MKP не открыт */

/* найти имя файла без расширения */
j=0;
k=strlen(fname[1]);
for (i=k; i>0; i--)
	{
	if (fname[1][i]=='.') {j=i; goto m1;}	/* убрать текущее расширение у имени файла */
	}

m1:
strncpy(fname[0],fname[1],248);
if ((j>0)&&(j<strlen(fname[0]))) fname[0][j]=0;
if (dos==1) fname[0][8]=0;	/* в ДОС не более 8 символов в имени */

/* сделать имя для файла MKL */
strncpy(fname[2],fname[0],248);
strcat(fname[2],".mkl");
if (kluch_e!=0) printf("\r\nMKL: %s %s",fname[2],mes0);

fp2=fopen(fname[2],"wb");	/* открыть MKL как двоичный для записи */
if (fp2==NULL) {if (kluch_e!=0) printf("\r\nMKL %s can`t created %s",fname[2],mes0);
		re=EX_CANTCREAT; goto end1;
		}

/* сделать имя для файла TXT */
strncpy(fname[3],fname[0],248);
strcat(fname[3],".txt");
if (kluch_e!=0) printf("\r\nTXT: %s %s",fname[2],mes0);

fp3=fopen(fname[3],"wb");	/* открыть TXT как двоичный для записи */
if (fp3==NULL) {if (kluch_e!=0) printf("\r\nTXT %s can`t created %s",fname[2],mes0);
		re=EX_CANTCREAT; goto end2;
		}

/* сделать имя для файла HTM */
strncpy(fname[4],fname[0],248);
strcat(fname[4],".htm");
if (kluch_e!=0) printf("\r\nHTM: %s %s",fname[3],mes0);

fp4=fopen(fname[4],"wb");	/* открыть HTM как двоичный для записи */
if (fp4==NULL) {if (kluch_e!=0) printf("\r\nHTM %s can`t created %s",fname[3],mes0);
		re=EX_CANTCREAT; goto end3;
		}

init_mkkom();
init_mkfunk();
re=read_mkp();
if (kluch_e!=0) printf ("ERROR in READ_MKP RE=%d \r\n",re);
if (re!=0) goto end4;
re=ks_mkp();
if (kluch_e!=0) printf ("ERROR in KS_MKP RE=%d \r\n",re);
if (re!=0) goto end4;
re=mkp2text();
if (kluch_e!=0) printf ("ERROR in MKP2TXT RE=%d \r\n",re);
if (re!=0) goto end4;
re=compil_p();
if (kluch_e!=0) printf ("ERROR in COMPIL_P RE=%d \r\n",re);
if (re!=0) goto end4;


end4:	fclose(fp4);
end3:	fclose(fp3);
end2:	fclose(fp2);
end1: 	fclose(fp1);
end:
if ((kluch_e!=0)&&(re!=0)) printf("RETURN %d\r\n",re);

return (re);
}
