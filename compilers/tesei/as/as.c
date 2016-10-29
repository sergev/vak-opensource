/*
 * Assembler for Angstrem KP1878BE1 processor.
 *
 * Отличия от фирменного ассемблера:
 * 1) В директиве .byte требуется четное количество аргументов.
 * 2) При ссылке на локальную метку необходимо задавать направление
 *    ссылки буквами `f' и `b' (вперед и назад) после номера метки,
 *    например "jmp $1f".
 * 3) Нет встроенного препроцессора. Рекомендуется использовать cpp.
 *
 * Copyright (C) 2002 Serge Vakulenko <vak@cronyx.ru>
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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <argp.h>

const char *argp_program_version =
	"Tesei Assembler 1.0\n"
	"Copyright (C) 2002 Serge Vakulenko\n"
	"This program is free software; it comes with ABSOLUTELY NO WARRANTY;\n"
	"see the GNU General Public License for more details.";

const char *argp_program_bug_address = "<vak@cronyx.ru>";

#define STSIZE		2000	/* symbol table size */
#define TXTSIZE		1024	/* command segment size */
#define DATSIZE		128	/* data segment size */
#define DATSTART	64	/* data segment start */
#define OUTSIZE		20	/* hex output line size */
#define MAXCONS		256	/* max constants */
#define MAXREL		10240	/* max relocated references */
#define MAXLIBS		10	/* max libraries */
#define MAXLABELS	1000	/* max relative (digit) labels */
#define MAXLINESZ	1024	/* max source line length */

/*
 * Lexical items.
 */
#define LEOF    0	/* end of file */
#define LEOL    1	/* end of line */
#define LNUM    2	/* number */
#define LNAME   3	/* name */
#define LCMD    4	/* machine command */
#define LREG    5	/* register (for example %a1 or b2) */
#define LSREG	6	/* spec. register (for example #1 or #b) */
#define LLSHIFT 7	/* << */
#define LRSHIFT 8	/* >> */
#define LMOD	9	/* // */
#define LBYTE   10	/* .byte */
#define LWORD   11	/* .word */
#define LEND	12	/* .end */
#define LEVEN	13	/* .even */
#define LLIST	14	/* .list - ignored */
#define LDATA   15	/* .data */
#define LCONST  16	/* .const */
#define LLAB	17	/* local label */

/*
 * Symbol/expression types.
 */
#define TUNDF   0       /* unknown symbol */
#define TABS    1       /* constant */
#define TDATA   2       /* variable or array */
#define TTEXT   3       /* label or function */
#define TCONST  4       /* readonly data in text segment */

/*
 * Command types.
 */
#define F1OP    1       /* 1 register operand */
#define F2OP    2       /* 2 register operands */
#define FJUMP   3       /* 12-bit address argument */
#define FRC8    4       /* register + 8-bit constant */
#define FRC5    5       /* register + 5-bit constant */
#define FRC4    6       /* register + 4-bit constant */
#define FTPR    7       /* spec register + register */
#define FFPR    8       /* register + spec register */
#define FCON1   9       /* 1-bit constant */
#define FCON4   10      /* 4-bit constant */
#define FSR	11	/* spec register */
#define FSC8	12	/* spec register + 8-bit constant */
#define FLDAH	13	/* ldah */
#define FLDAL	14	/* ldal */
#define FLIAH	15	/* liah */
#define FLIAL	16	/* lial */
#define FMDAH	17	/* mdah */
#define FMDAL	18	/* mdal */
#define FMIAH	19	/* miah */
#define FMIAL	20	/* mial */
#define FNOTR   0x80    /* next command not reached */

/*
 * Relocation flags.
 */
#define RLAB    0x001	/* relative label */
#define RWL3	0x002	/* low 8 bits of word address, shifted left by 3 */
#define RWH3	0x004	/* high 8 bits of word address, shifted left by 3 */
#define RBL3	0x008	/* low 8 bits of byte address, shifted left by 3 */
#define RBH3	0x010	/* high 8 bits of byte address, shifted left by 3 */
#define RWL5	0x020	/* low 8 bits of word address, shifted left by 5 */
#define RWH5	0x040	/* high 8 bits of word address, shifted left by 5 */
#define RBL5	0x080	/* low 8 bits of byte address, shifted left by 5 */
#define RBH5	0x100	/* high 8 bits of byte address, shifted left by 5 */

struct stab {
	char *name;
	int len;
	int type;
	int value;
} stab [STSIZE];

struct labeltab {
	int num;
	int value;
} labeltab [MAXLABELS];

struct constab {
	char *val;
	int len;
	int sym;
} constab [MAXCONS];

struct reltab {
	int addr;
	int sym;
	int flags;
} reltab [MAXREL];

struct libtab {
	char *name;
} libtab [MAXLIBS];

struct source_line {
	char *text;
	int address;
	int end_address;
	int label_address;
	int line_number;
};

char *input_file_name;
char *outfile;
int debug;
int filenum;
int count;
int dcount = DATSTART;
int lastcmd;
int blexflag, backlex, blextype;
char name [256];
int intval;
int extref;
int extflag;
int stabfree;
int unprotect = 0;
int wdog = 0;
int mclre = 0;
int nconst;
int nrel;
int nlib;
int nlabels;
int outaddr;
unsigned char outbuf [OUTSIZE], *outptr = outbuf;

unsigned char data [2*DATSIZE];
unsigned short text [2*TXTSIZE];
unsigned char tbusy [2*TXTSIZE];

int input_line_number;
int input_file_source_base;
int last_char = -1;
unsigned char current_line [MAXLINESZ];
unsigned char *current_ptr;
struct source_line *sources;
int sources_size;
int source_line_count;

/*
 * Every input symbol has some meaning: letter, decimal digit,
 * octal digit or hex digit. This table contains type tags for all
 * input characters.
 */
unsigned char ctype [256] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,7,7,7,7,7,7,7,7,5,5,0,0,0,0,0,0,
	0,9,9,9,9,9,9,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,0,0,0,8,
	0,9,9,9,9,9,9,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,0,0,0,0,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
};
#define ISHEX(c)        (ctype[(c)&0377] & 1)
#define ISOCTAL(c)      (ctype[(c)&0377] & 2)
#define ISDIGIT(c)      (ctype[(c)&0377] & 4)
#define ISLETTER(c)     (ctype[(c)&0377] & 8)

void parse (void);
void relocate (void);
void libraries (void);
void listing (void);
void output (void);
void makecmd (int code, int type);
void makeconst (int sym);
int getexpr (int *s);

struct table {
	unsigned val;
	char *name;
	short type;
} table [] = {
/*
 * Instruction set.
 *
Opcode - Mnemonic - Flags --- Description --- */
/* Двухоперандные команды */
{0x0400,"mov",  F2OP },	/* 0000 01ss sssd dddd Пересылка */
{0x0800,"cmp",  F2OP },	/* 0000 10ss sssd dddd Сравнение */
{0x1000,"add",  F2OP },	/* 0001 00ss sssd dddd Сложение */
{0x0c00,"sub",  F2OP },	/* 0000 11ss sssd dddd Вычитание */
{0x1400,"and",  F2OP },	/* 0001 01ss sssd dddd Логическое И */
{0x1800,"or",   F2OP },	/* 0001 10ss sssd dddd Логическое ИЛИ */
{0x1c00,"xor",  F2OP },	/* 0001 11ss sssd dddd Исключающее ИЛИ */
/* Литерные команды */
{0x4000,"movl", FRC8 },	/* 010c cccc cccd dddd Пересылка литеры */
{0x4000,"mdah", FMDAH },/* Загрузка старшего байта байтного адреса команд */
{0x4000,"mdal", FMDAL },/* Загрузка младшего байта байтного адреса команд */
{0x4000,"miah", FMIAH },/* Загрузка старшего байта словного адреса команд */
{0x4000,"mial", FMIAL },/* Загрузка младшего байта словного адреса команд */
{0x4000,"clr",	F1OP },	/* 0100 0000 000d dddd */
{0x6000,"cmpl", FRC8 },	/* 011c cccc cccd dddd Сравнение с литерой */
{0x3000,"addl", FRC5 },	/* 0011 00cc cccd dddd Сложение с литерой */
{0x3020,"inc",  F1OP },	/* 0011 0000 001d dddd */
{0x2c00,"subl", FRC5 },	/* 0010 11cc cccd dddd Вычитание литеры */
{0x2c20,"dec",	F1OP },	/* 0010 1100 001d dddd */
{0x2800,"bicl", FRC4 },	/* 0010 100c cccd dddd Сброс разрядов */
{0x2a00,"bich", FRC4 },	/* 0010 101c cccd dddd Сброс разрядов */
{0x3800,"bisl", FRC4 },	/* 0011 100c cccd dddd Установка разрядов */
{0x3a00,"bish", FRC4 },	/* 0011 101c cccd dddd Установка разрядов */
{0x3c00,"btgl", FRC4 },	/* 0011 110c cccd dddd Инверсия разрядов */
{0x3e00,"btgh", FRC4 },	/* 0011 111c cccd dddd Инверсия разрядов */
{0x3400,"bttl", FRC4 },	/* 0011 010c cccd dddd Проверка разрядов */
{0x3600,"btth", FRC4 },	/* 0011 011c cccd dddd Проверка разрядов */
/* Однооперандные команды */
{0x0020,"swap", F1OP },	/* 0000 0000 001d dddd Обмен тетрад */
{0x0040,"neg",  F1OP },	/* 0000 0000 010d dddd Смена знака */
{0x0060,"not",  F1OP },	/* 0000 0000 011d dddd Инверсия всех разрядов */
{0x0080,"shl",  F1OP },	/* 0000 0000 100d dddd Логический сдвиг влево */
{0x00a0,"shr",  F1OP },	/* 0000 0000 101d dddd Логический сдвиг вправо */
{0x00c0,"shra", F1OP },	/* 0000 0000 110d dddd Арифметический сдвиг вправо */
{0x00e0,"rlc",  F1OP },	/* 0000 0000 111d dddd Циклический сдвиг влево */
{0x0100,"rrc",  F1OP },	/* 0000 0001 000d dddd Циклический сдвиг вправо */
{0x0120,"adc",  F1OP },	/* 0000 0001 001d dddd Сложение с переносом */
{0x0140,"sbc",	F1OP },	/* 0000 0001 010d dddd Вычитание переноса */
/* Команды работы со служебными регистрами и регистром состояния */
{0x2000,"ldr",  FSC8 },	/* 0010 0ccc cccc cnnn Загрузка служебных регистров */
{0x2000,"ldah", FLDAH },/* Загрузка старшего байта байтного адреса команд */
{0x2000,"ldal", FLDAL },/* Загрузка младшего байта байтного адреса команд */
{0x2000,"liah", FLIAH },/* Загрузка старшего байта словного адреса команд */
{0x2000,"lial", FLIAL },/* Загрузка младшего байта словного адреса команд */
{0x0200,"mtpr", FTPR },	/* 0000 0010 nnns ssss Запись в служебные регистры */
{0x0300,"mfpr", FFPR },	/* 0000 0011 nnnd dddd Чтение служебных регистров */
{0x0010,"push", FSR},	/* 0000 0000 0001 0nnn Запись в стек данных */
{0x0018,"pop",  FSR},	/* 0000 0000 0001 1nnn Чтение из стека данных */
{0x0180,"sst",  FCON4},	/* 0000 0001 1000 bbbb Установка разрядов RS */
{0x0181,"stc",  0},	/* 0000 0001 1000 0001 */
{0x0188,"stie", 0},	/* 0000 0001 1000 1000 */
{0x0184,"stn",  0},	/* 0000 0001 1000 0100 */
{0x0182,"stz",  0},	/* 0000 0001 1000 0010 */
{0x01c0,"cst",  FCON4},	/* 0000 0001 1100 bbbb Сброс разрядов RS */
{0x01c1,"clc",  0},	/* 0000 0001 1100 0001 */
{0x01c8,"clie",	0},	/* 0000 0001 1100 1000 */
{0x01c4,"cln",  0},	/* 0000 0001 1100 0100 */
{0x01c2,"clz",  0},	/* 0000 0001 1100 0010 */
{0x0004,"tof",  0 },	/* 0000 0000 0000 0100 Проверка переполнения */
{0x0005,"tdc",  0 },	/* 0000 0000 0000 0101 Проверка тетрадного переноса */
/* Команды передачи управления */
{0x8000,"jmp",  FJUMP|FNOTR}, /* 1000 00aa aaaa aaaa Безусловный переход */
{0x9000,"jsr",  FJUMP},	/* 1001 00aa aaaa aaaa Переход к подпрограмме */
{0xb000,"jnz",  FJUMP},	/* 1011 00aa aaaa aaaa Переход по Z = 0 (не равно) */
{0xb000,"jne",  FJUMP},
{0xa000,"jz",   FJUMP},	/* 1010 00aa aaaa aaaa Переход по Z = 1 (равно) */
{0xa000,"jeq",  FJUMP},
{0xc000,"jns",  FJUMP},	/* 1100 00aa aaaa aaaa Переход по S = 0 (плюс) */
{0xd000,"js",   FJUMP},	/* 1101 00aa aaaa aaaa Переход по S = 1 (минус) */
{0xe000,"jnc",  FJUMP},	/* 1110 00aa aaaa aaaa Переход по C = 0 */
{0xe000,"jae",  FJUMP},
{0xf000,"jc",   FJUMP},	/* 1111 00aa aaaa aaaa Переход по C = 1 */
{0xf000,"jb",   FJUMP},
{0x0003,"ijmp", FNOTR},/* 0000 0000 0000 0011 Косвенный переход */
{0x0007,"ijsr", 0 },	/* 0000 0000 0000 0111 Косвенный переход к подпрограмме */
{0x000c,"rts",  FNOTR},	/* 0000 0000 0000 1100 Возврат из подпрограммы */
{0x000e,"rtsc", FCON1|FNOTR}, /* 0000 0000 0000 111c Возврат из подпрограммы с битом С */
{0x000d,"rti",  FNOTR},	/* 0000 0000 0000 1101 Возврат из прерывания */
/* Специальные команды */
{0x0000,"nop",  0 },	/* 0000 0000 0000 0000 Нет операции */
{0x0001,"wait", 0 },	/* 0000 0000 0000 0001 Ожидание */
{0x0008,"slp",	0 },	/* 0000 0000 0000 1000 Останов */
{0x0002,"rst",	FNOTR},	/* 0000 0000 0000 0010 Сброс */
{0x0006,"sksp", 0 },	/* 0000 0000 0000 0110 Прогон стека команд */

{0}};

struct argp_option argp_options[] = {
	{"debug",	'D', 0,		0,	"Print debugging information" },
	{"output",	'o', "FILE",	0,	"Output to FILE (default infile.hex)" },
	{"library",	'l', "DIR",	0,	"Add DIR to library search path" },
	{ 0 }
};

/*
 * Parse a single option.
 */
int parse_option (int key, char *arg, struct argp_state *state)
{
	switch (key) {
	case 'D':
		debug = 1;
		break;
	case 'o':
		outfile = arg;
		break;
	case 'l':
		if (nlib >= MAXLIBS)
			argp_failure (state, 1, 0, "too many libraries");
		libtab[nlib++].name = arg;
		break;
	case ARGP_KEY_END:
		if (state->arg_num < 1)		/* Not enough arguments. */
			argp_usage (state);
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

/*
 * Our argp parser.
 */
struct argp argp_parser = {
	/* The options we understand. */
	argp_options,

	/* Function to parse a single option. */
	parse_option,

	/* A description of the arguments we accept. */
	"infile.mic...",

	/* Program documentation. */
	"\nAssembler for Angstrem Tesei (KP1878BE1) processor"
};

/*
 * Display an error message and stop.
 */
void uerror (char *s, ...)
{
	va_list ap;

	va_start (ap, s);
	fprintf (stderr, "as: ");
	if (input_file_name)
		fprintf (stderr, "%s, ", input_file_name);
	fprintf (stderr, "%d: ", input_line_number);
	vfprintf (stderr, s, ap);
	va_end (ap);
	fprintf (stderr, "\n");
	if (outfile)
		unlink (outfile);
	exit (1);
}

int open_input (char *filename)
{
	input_file_name = filename;
	if (! freopen (filename, "r", stdin))
		return 0;

	input_line_number = 0;
	input_file_source_base = source_line_count;
	last_char = -1;
	current_ptr = 0;
	return 1;
}

int main (int argc, char **argv)
{
	int i;
	char *cp;

	argp_parse (&argp_parser, argc, argv, 0, &i, 0);
	if (! outfile) {
		outfile = malloc (4 + strlen (argv[i]));
		if (! outfile)
			uerror ("out of memory");
		strcpy (outfile, argv[i]);
		cp = strrchr (outfile, '.');
		if (! cp)
			cp = outfile + strlen (outfile);
		strcpy (cp, ".hex");
	}
	for (; i<argc; i++) {
		if (! open_input (argv[i]))
			uerror ("cannot open");
		parse ();
		++filenum;
	}
	input_file_name = 0;

	if (! nlib)
		libtab[nlib++].name = "/usr/local/lib/tesei";
	libraries ();
	relocate ();
	listing ();
	output ();
	return 0;
}

void set_line_address (int lnum, int address)
{
/*if (lnum < 30) fprintf (stderr, "line %d - address %d\n", lnum, address);*/
	sources[input_file_source_base+lnum-1].address = address;
	sources[input_file_source_base+lnum-1].end_address = address;
}

void set_line_end_address (int lnum, int address)
{
	sources[input_file_source_base+lnum-1].end_address = address;
}

void set_line_label_address (int lnum, int address)
{
	sources[input_file_source_base+lnum-1].label_address = address;
}

int get_char ()
{
	if (last_char >= 0) {
		int c = last_char;
		last_char = -1;
		return c;
	}
	if (! current_ptr || ! *current_ptr) {
		/* Read line from input file to `current_line' array.
		 * Set `current_ptr' to the first byte. */
again:		current_ptr = fgets (current_line, sizeof(current_line), stdin);
		if (! current_ptr)
			return -1;
		++input_line_number;
		if (*current_ptr == '#')
			goto again;
		if (source_line_count >= sources_size) {
			int bytes;

			sources_size += 512;
			bytes = sources_size * sizeof (struct source_line);
			if (sources_size == 512)
				sources = (struct source_line*) malloc (bytes);
			else
				sources = (struct source_line*)
					realloc (sources, bytes);
			if (! sources)
				uerror ("out of memory");
		}
		sources[source_line_count].line_number = input_line_number;
		sources[source_line_count].address = -1;
		sources[source_line_count].label_address = -1;
		sources[source_line_count].text = strdup (current_line);
		if (! sources[source_line_count].text)
			uerror ("out of memory");
		++source_line_count;
	}
	return *current_ptr++;
}

void unget_char (int c)
{
	if (c < 0)
		return;
	last_char = c;
}

/*
 * Search for command in op table.
 * Return an integer index or -1.
 */
int lookcmd ()
{
	int i;

	for (i=0; table[i].name; ++i)
		if (! strcasecmp (table[i].name, name))
			return i;
	return -1;
}

/*
 * Global array `name' contains some name.
 * Try to recognize it as a name of a harddware register.
 * Valid register names are:
 *	a0..a7
 *	b0..b7
 *	c0..c7
 *	d0..d7
 * Return an integer value of register or -1.
 */
int lookreg ()
{
	int reg;

	if (name[2] != 0)
		return -1;

	switch (name[0]) {
	case 'a': case 'b': case 'c': case 'd':
		reg = name[0] - 'a';
		break;
	case 'A': case 'B': case 'C': case 'D':
		reg = name[0] - 'A';
		break;
	default:
		return -1;
	}
	reg <<= 3;

	switch (name[1]) {
	case '0': case '1': case '2': case '3':
	case '4': case '5': case '6': case '7':
		reg |= name[1] - '0';
		break;
	default:
		return -1;
	}
	return reg;
}

/*
 * Convert a symbol to hex digit value.
 */
int hexdig (int c)
{
	if (c <= '9')      return c - '0';
	else if (c <= 'F') return c - 'A' + 10;
	else               return c - 'a' + 10;
}

/*
 * Read the integer value.
 * 1234   - decimal
 * 1234o  - octal
 * 1234h - hexadecimal
 * 1101b - binary
 */
void getnum (int c)
{
	unsigned char buf [80], *p, *end;
	unsigned char base;

	p = buf;
	while (ISHEX (c) && p < buf + sizeof(buf)) {
		*p++ = hexdig (c);
		c = get_char ();
	}
	switch (c) {
	case 'o': case 'O':
		base = 8;
		break;
	case 'h': case 'H':
		base = 16;
		break;
	default:
		if (c >= 0)
			unget_char (c);
		if (p[-1] == 0xb) {
			base = 2;
			--p;
		} else
			base = 10;
		break;
	}
	intval = 0;
	end = p;
	for (p=buf; p<end; ++p) {
		if (*p >= base) {
			uerror ("invalid numeric constant");
			break;
		}
		intval = intval * base + *p;
	}
}

/*
 * Read an identifier and put it to `name' array.
 * Parameter `c' contains the first symbol of the identifier.
 */
void getname (int c)
{
	char *cp;

	for (cp=name; c>' ' && c!=':'; c=get_char()) {
		if (! ISLETTER (c) && ! ISDIGIT (c))
			break;
		*cp++ = c;
	}
	*cp = 0;
	unget_char (c);
}

/*
 * Look up the symbol.
 */
int lookname ()
{
	int i, len;
	struct stab *s;

	len = strlen (name);
	s = 0;
	if (name[0] == 'L')
		name[0] = '.';
	for (i=0; i<stabfree; ++i) {
		if (! stab[i].len && ! s) {
			s = stab+i;
			continue;
		}
		if (name[0] == '.' && stab[i].len == len+1 &&
		    stab[i].name[0] == 'A'+filenum &&
		    ! strcmp (stab[i].name+1, name))
			return i;
		if (stab[i].len == len && ! strcmp (stab[i].name, name))
			return i;
	}
	if (! s)
		s = stab + stabfree++;

	/* Add the new symbol. */
	if (s >= stab + STSIZE)
		uerror ("symbol table overflow");
	s->name = malloc (2 + len);
	if (! s->name)
		uerror ("out of memory");
	s->len = len;
	if (name[0] == '.') {
		s->name[0] = 'A' + filenum;
		strcpy (s->name+1, name);
		++s->len;
	} else
		strcpy (s->name, name);
	s->value = 0;
	s->type = 0;
	return s - stab;
}

/*
 * Read the next lexical item from the input stream.
 */
int getlex (int *pval)
{
	int c;

	if (blexflag) {
		blexflag = 0;
		*pval = blextype;
		if (backlex == LEOL)
			++input_line_number;
		return backlex;
	}
	for (;;) switch (c = get_char()) {
	case ';':
skiptoeol:      while ((c = get_char()) != '\n')
			if (c == EOF)
				return LEOF;
	case '\n':
		c = get_char ();
		if (c == ';')
			goto skiptoeol;
		unget_char (c);
/*if (input_line_number < 30) fprintf (stderr, "EOL at line %d\n", input_line_number);*/
		return LEOL;
	case ' ':
	case '\t':
	case '\r':
	case '\f':
		continue;
	case EOF:
		return LEOF;
	case '<':
		if ((c = get_char()) == '<')
			return LLSHIFT;
		unget_char (c);
		return '<';
	case '>':
		if ((c = get_char()) == '>')
			return LRSHIFT;
		unget_char (c);
		return '>';
	case '/':
		if ((c = get_char()) == '/')
			return LMOD;
		unget_char (c);
		return '/';
	case '$':
		intval = 0;
		while (ISDIGIT (c = get_char())) {
			intval = intval * 10 + c - '0';
		}
		unget_char (c);
		return LLAB;
	case '#':
		c = get_char ();
		switch (c) {
		case '0': case '1': case '2': case '3':
		case '4': case '5': case '6': case '7':
			*pval = c - '0';
			break;
		case 'a': case 'b': case 'c': case 'd':
		case 'e': case 'f': case 'g': case 'h':
			*pval = c - 'a';
			break;
		case 'A': case 'B': case 'C': case 'D':
		case 'E': case 'F': case 'G': case 'H':
			*pval = c - 'A';
			break;
		default:
			unget_char (c);
			return '#';
		}
		return LSREG;
	case '%':
		c = get_char ();
		switch (c) {
		case 'a': case 'b': case 'c': case 'd':
			*pval = c - 'a';
			break;
		case 'A': case 'B': case 'C': case 'D':
			*pval = c - 'A';
			break;
		default:
			unget_char (c);
			return '%';
		}
		*pval <<= 3;
		c = get_char ();
		switch (c) {
		case '0': case '1': case '2': case '3':
		case '4': case '5': case '6': case '7':
			*pval |= c - '0';
			break;
		default:
			uerror ("bad register name");
		}
		return LREG;
	case '\'':
		c = get_char ();
		if (c == '\'')
			uerror ("bad char constant");
		if (c == '\\')
			switch (c = get_char()) {
			case 'a':  c = 0x07; break;
			case 'b':  c = '\b'; break;
			case 'f':  c = '\f'; break;
			case 'n':  c = '\n'; break;
			case 'r':  c = '\r'; break;
			case 't':  c = '\t'; break;
			case 'v':  c = '\v'; break;
			case '\'': break;
			case '\\': break;
			default: uerror ("bad char constant");
			}
		if (get_char() != '\'')
			uerror ("bad char constant");
		intval = c;
		return LNUM;
	case '*':       case '\\':	case '@':
	case '^':       case '&':       case '!':
	case '"':       case ',':       case '[':       case ']':
	case '(':       case ')':       case '{':       case '}':
	case '=':       case ':':       case '+':       case '-':
		return c;
	case '0':       case '1':       case '2':       case '3':
	case '4':       case '5':       case '6':       case '7':
	case '8':       case '9':
		getnum (c);
		return LNUM;
	default:
		if (! ISLETTER (c))
			uerror ("bad character: \\%o", c & 0377);
		getname (c);
		if (name[0] == '.') {
			if (! name[1]) return '.';
			if (! strcmp (name, ".data"))  return LDATA;
			if (! strcmp (name, ".const")) return LCONST;
			if (! strcmp (name, ".byte"))  return LBYTE;
			if (! strcmp (name, ".word"))  return LWORD;
			if (! strcmp (name, ".end"))   return LEND;
			if (! strcmp (name, ".endc"))  return LEND; /* same as .end */
			if (! strcmp (name, ".even"))  return LEVEN;
			if (! strcmp (name, ".list"))  return LLIST;
			if (! strcmp (name, ".macro")) uerror ("command .macro not implemented");
			if (! strcmp (name, ".endm"))  uerror ("command .endm not implemented");
			if (! strcmp (name, ".if"))    uerror ("command .if not implemented");
			if (! strcmp (name, ".else"))  uerror ("command .else not implemented");
			if (! strcmp (name, ".endif")) uerror ("command .endif not implemented");
			if (! strcmp (name, ".bpt"))   uerror ("command .bpt not implemented");
		}
		if ((*pval = lookcmd()) != -1)
			return LCMD;
		if ((*pval = lookreg()) != -1)
			return LREG;
		*pval = lookname ();
		return LNAME;
	}
}

void ungetlex (int val, int type)
{
	blexflag = 1;
	backlex = val;
	blextype = type;
	if (backlex == LEOL) {
		--input_line_number;
/*if (input_line_number < 30) fprintf (stderr, "backlex EOL at line %d\n", input_line_number);*/
	}
}

/*
 * Get the expression term.
 */
int getterm ()
{
	int cval, s;

	switch (getlex (&cval)) {
	default:
		uerror ("operand missing");
	case LNUM:
		return TABS;
	case LNAME:
		if (stab[cval].type == TUNDF || stab[cval].type == TCONST) {
			intval = 0;
			extref = cval;
		} else
			intval = stab[cval].value;
		return stab[cval].type;
	case LLAB:
		cval = get_char ();
		if (cval == 'b' || cval == 'B')
			extref = -intval;
		else if (cval == 'f' || cval == 'F')
			extref = intval;
		else
			uerror ("invalid $ label reference");
		extflag |= RLAB;
		intval = 0;
		return TUNDF;
	case '.':
		intval = count;
		return TTEXT;
	case '(':
		getexpr (&s);
		if (getlex (&cval) != ')')
			uerror ("bad () syntax");
		return s;
	}
}

/*
 * Read the expression.
 * Put the type of expression into *s.
 *
 * expr = [term] {op term}...
 * term = LNAME | LNUM | "." | "(" expr ")"
 * op   = "+" | "-" | "&" | "!" | "\\" | "^" | "<<" | ">>" | "/" | "*" | "//"
 */
int getexpr (int *s)
{
	int clex, cval, s2, rez;

	/* Get the first item. */
	switch (clex = getlex (&cval)) {
	default:
		ungetlex (clex, cval);
		uerror ("undefined operand");

	case LNUM:
	case LNAME:
	case '.':
	case '(':
	case '@':
	case LLAB:
		ungetlex (clex, cval);
		*s = getterm ();
		rez = intval;
		break;
	}
	for (;;) {
		switch (clex = getlex (&cval)) {
		case '+':
			s2 = getterm ();
			if (*s == TABS) *s = s2;
			else if (s2 != TABS)
				uerror ("too complex expression");
			rez += intval;
			break;
		case '-':
			s2 = getterm ();
			if (s2 != TABS)
				uerror ("too complex expression");
			rez -= intval;
			break;
		case '&':
			s2 = getterm ();
			if (*s != TABS || s2 != TABS)
				uerror ("too complex expression");
			rez &= intval;
			break;
		case '!':
			s2 = getterm ();
			if (*s != TABS || s2 != TABS)
				uerror ("too complex expression");
			rez |= intval;
			break;
		case '\\':
			s2 = getterm ();
			if (*s != TABS || s2 != TABS)
				uerror ("too complex expression");
			rez ^= intval;
			break;
		case '^':
			s2 = getterm ();
			if (*s != TABS || s2 != TABS)
				uerror ("too complex expression");
			rez ^= ~intval;
			break;
		case LLSHIFT:
			s2 = getterm ();
			if (*s != TABS || s2 != TABS)
				uerror ("too complex expression");
			rez <<= intval;
			break;
		case LRSHIFT:
			s2 = getterm ();
			if (*s != TABS || s2 != TABS)
				uerror ("too complex expression");
			rez >>= intval;
			break;
		case '*':
			s2 = getterm ();
			if (*s != TABS || s2 != TABS)
				uerror ("too complex expression");
			rez *= intval;
			break;
		case '/':
			s2 = getterm ();
			if (*s != TABS || s2 != TABS)
				uerror ("too complex expression");
			if (! intval)
				uerror ("division by zero");
			rez /= intval;
			break;
		case LMOD:
			s2 = getterm ();
			if (*s != TABS || s2 != TABS)
				uerror ("too complex expression");
			if (! intval)
				uerror ("division (%%) by zero");
			rez %= intval;
			break;
		default:
			ungetlex (clex, cval);
			intval = rez;
			return rez;
		}
	}
}

void parse ()
{
	int clex, cval, tval, lnum;

	for (;;) {
		clex = getlex (&cval);
		switch (clex) {
		case LEND:
			set_line_label_address (input_line_number, count);
			return;

		case LEOF:
			return;

		case LEOL:
			continue;

		case LCMD:
			makecmd (table[cval].val, table[cval].type);
			break;

		case '.':
			if (getlex (&cval) != '=')
				uerror ("invalid . = assignment");
			getexpr (&tval);
			if (tval != TABS)
				uerror ("bad value .org");
			count = intval;
			break;

		case LNAME:
			lnum = input_line_number;
			clex = getlex (&tval);
			switch (clex) {
			case '=':              /* name '=' val */
				getexpr (&tval);
				if (tval == TUNDF)
					uerror ("bad value for equivalence");
				if (stab[cval].type != TUNDF) {
					if (stab[cval].type != tval ||
					    stab[cval].value != intval)
						uerror ("name redefined");
					break;
				}
				stab[cval].type = tval;
				stab[cval].value = intval;
				set_line_label_address (lnum, intval);
				break;
			case ':':               /* name: */
				if (stab[cval].type != TUNDF) {
					uerror ("name redefined");
					break;
				}
				clex = getlex (&tval);
				switch (clex) {
				case LDATA:             /* name: .data size */
					getexpr (&tval);
					if (tval != TABS || intval < 0)
						uerror ("bad length .data");
					if (stab[cval].type != TUNDF) {
						if (stab[cval].type != TDATA)
							uerror ("name already defined");
						break;
					}
					stab[cval].type = TDATA;
					stab[cval].value = dcount;
					set_line_label_address (lnum, dcount);
					dcount += intval;
					break;
				case LCONST:            /* name: .const val,val... */
					if (stab[cval].type != TUNDF)
						uerror ("name already defined");
					stab[cval].type = TCONST;
					stab[cval].value = 0;
					makeconst (cval);
					break;
				default:
					ungetlex (clex, tval);
					stab[cval].value = count;
					stab[cval].type = TTEXT;
					lastcmd = 0;
					set_line_label_address (lnum, count);
					continue;
				}
				break;
			default:
				uerror ("expect `:' or `=' after label");
			}
			break;

		case LLAB:
			if (nlabels >= MAXLABELS)
				uerror ("too many digital labels");
			labeltab[nlabels].num = intval;
			labeltab[nlabels].value = count;
			++nlabels;
			set_line_label_address (input_line_number, count);
			clex = getlex (&tval);
			if (clex != ':')
				uerror ("bad digital label");
			lastcmd = 0;
			continue;

		case LLIST:		/* .list - ignored */
			getexpr (&tval);
			break;

		case LEVEN:		/* .even - ignored */
			break;

		case LBYTE:		/* .byte */
			lnum = input_line_number;
			set_line_address (lnum, count);
			for (;;) {
				getexpr (&tval);
				if (tval != TABS)
					uerror ("bad value .byte");
				clex = getlex (&cval);
				if (clex != ',')
					uerror ("even number of bytes required");
				cval = intval & 0xff;

				getexpr (&tval);
				if (tval != TABS)
					uerror ("bad value .byte");

				text [count] = intval << 8 | cval;
				tbusy [count] = 1;
				set_line_end_address (lnum, count);
				++count;

				clex = getlex (&cval);
				if (clex != ',') {
					ungetlex (clex, cval);
					break;
				}
			}
			break;

		case LWORD:		/* .word */
			lnum = input_line_number;
			set_line_address (lnum, count);
			for (;;) {
				getexpr (&tval);
				if (tval != TABS)
					uerror ("bad value .word");

				text [count] = intval;
				tbusy [count] = 1;
				set_line_end_address (lnum, count);
				++count;

				clex = getlex (&cval);
				if (clex != ',') {
					ungetlex (clex, cval);
					break;
				}
			}
			break;

		default:
			uerror ("syntax error");
		}
		clex = getlex (&cval);
		if (clex == LEOF)
			return;
		if (clex != LEOL)
			uerror ("bad command argument");
	}
}

/*
 * Flush the output buffer.
 */
void outflush (void)
{
	unsigned char *p, sum = 0;

	/* set byte count */
	outbuf[0] = outptr - outbuf - 4;

	putchar (':');
	for (p=outbuf; p<outptr; ++p) {
		printf ("%02X", *p);
		sum += *p;
	}
	printf ("%02X\n", (unsigned char) -sum);
	outptr = outbuf;
}

/*
 * Put the word to the output buffer.
 */
void outhex (long addr, int val)
{
	if (outptr >= outbuf + OUTSIZE ||
	    (outptr > outbuf && addr != outaddr+1))
		outflush ();
	if (outptr == outbuf) {
		*outptr++ = 0;
		*outptr++ = addr >> 7;
		*outptr++ = addr << 1;
		*outptr++ = 0;
	}
	*outptr++ = val;
	*outptr++ = val >> 8;
	outaddr = addr;
}

/*
 * Write the resulting hex image.
 */
void output (void)
{
	int i;

	if (! freopen (outfile, "w", stdout))
		uerror ("cannot open %s", outfile);

	for (i=0; i<TXTSIZE; ++i)
		if (tbusy [i])
			outhex (i, text[i]);
	if (outptr > outbuf)
		outflush ();

	printf (":00000001FF\n");
}

/*
 * Compile the constant.
 */
void makeconst (int sym)
{
	char buf [MAXLINESZ], *p = buf;
	int len, tval, clex, cval;

	if (nconst >= MAXCONS)
		uerror ("too many constants");
	for (;;) {
		getexpr (&tval);
		if (tval != TABS)
			uerror ("bad value .const");

		if (p >= buf + sizeof (buf))
			uerror ("too long .const");
		*p++ = intval;

		clex = getlex (&cval);
		if (clex != ',') {
			ungetlex (clex, cval);
			break;
		}
	}
	len = p - buf;

	constab[nconst].val = malloc (len + 1);
	if (! constab[nconst].val)
		uerror ("out of memory");
	memcpy (constab[nconst].val, buf, len);

	constab[nconst].len = len;
	constab[nconst].sym = sym;
	++nconst;
}

/*
 * Set text segment value.
 */
void settext (int addr, int val)
{
	text [addr] = val;
	tbusy [addr] = 1;
	lastcmd = val;
}

/*
 * Resolve pending references, adding
 * modules from libraries.
 */
void libraries ()
{
	struct stab *s;
	int n, undefined;
	char name [256];

	/* For every undefined reference,
	 * add the module from the library. */
	undefined = 0;
	for (s=stab; s<stab+stabfree; ++s) {
		if (s->type != TUNDF)
			continue;

		for (n=0; n<nlib; ++n) {
			sprintf (name, "%s/%s.mic", libtab[n].name, s->name);
			if (open_input (name)) {
				parse ();
				++filenum;
				break;
			}
		}
		if (n >= nlib) {
			fprintf (stderr, "as: undefined: %s\n", s->name);
			++undefined;
		}
	}
	if (undefined > 0)
		exit (1);
	input_file_name = 0;
}

/*
 * Find the relative label address,
 * by the reference address and the label number.
 * Backward references have negative label numbers.
 */
int findlabel (int addr, int sym)
{
	struct labeltab *p;

	if (sym < 0) {
		/* Backward reference. */
		for (p=labeltab+nlabels-1; p>=labeltab; --p)
			if (p->value <= addr && p->num == -sym)
				return p->value;
		uerror ("undefined label %db at address %d", -sym, addr);
	} else {
		/* Forward reference. */
		for (p=labeltab; p<labeltab+nlabels; ++p)
			if (p->value > addr && p->num == sym)
				return p->value;
		uerror ("undefined label %df at address %d", sym, addr);
	}
	return 0;
}

int compare_constab_len (const void *pa, const void *pb)
{
	const struct constab *a = pa, *b = pb;

	if (a->len > b->len)
		return -1;
	return (a->len < b->len);
}

/*
 * Allocate constants and relocate references.
 */
void relocate ()
{
	int n, v, mask;
	struct constab *c, *p;
	struct reltab *r;
	int tsize, csize, dsize;

	tsize = csize = 0;
	dsize = dcount - DATSTART;

	for (n=0; n<TXTSIZE; ++n)
		if (tbusy [n])
			++tsize;

	/* Place the constants at the end of text segment. */
	qsort (constab, nconst, sizeof (constab[0]), compare_constab_len);
	for (c=constab; c<constab+nconst; ++c) {
		/* Try to find and reuse the constant. */
		for (p=constab; p<c; ++p)
			if (p->len >= c->len &&
			    memcmp (p->val + p->len - c->len, c->val, c->len) == 0) {
				stab[c->sym].value = stab[p->sym].value + p->len - c->len;
				stab[c->sym].type = TTEXT;
				break;
			}
		if (p < c)
			continue;

		stab[c->sym].value = count;
		stab[c->sym].type = TTEXT;
		for (n=0; n<c->len; ++n) {
			settext (count++, c->val[n]);
			++csize;
		}
	}

	/* Relocate pending references. */
	for (r=reltab; r<reltab+nrel; ++r) {
		if (r->flags & RLAB)
			v = findlabel (r->addr, r->sym);
		else
			v = stab[r->sym].value;

		if (r->flags & (RWL3 | RWH3 | RBL3 | RBH3)) {
			if      (r->flags & RWL3) v <<= 3;
			else if (r->flags & RWH3) v >>= 5;
			else if (r->flags & RBL3) v <<= 4;
			else if (r->flags & RBH3) v >>= 4;

			mask = 0x7f8;
		} else if (r->flags & (RWL5 | RWH5 | RBL5 | RBH5)) {
			if      (r->flags & RWL5) v <<= 5;
			else if (r->flags & RWH5) v >>= 3;
			else if (r->flags & RBL5) v <<= 6;
			else if (r->flags & RBH5) v >>= 2;

			mask = 0x1fe0;
		} else {
			mask = 0xfff;
		}
		v += text [r->addr] & mask;
		text [r->addr] &= ~mask;
		text [r->addr] |= v & mask;
	}
	fprintf (stderr, "Total text %d words, const %d words, data %d bytes\n",
		tsize, csize, dsize);
	if (count > TXTSIZE)
		uerror ("text segment overflow: %d words", count);
	if (dcount > DATSIZE)
		uerror ("data segment overflow: %d bytes", dsize);
	fprintf (stderr, "Free text %d words, data %d bytes\n",
		TXTSIZE - count, DATSIZE - dcount);
}

int compare_stab (const void *pa, const void *pb)
{
	const struct stab *a = pa, *b = pb;

#define ISDATA(a) (((a)->type == TDATA || (a)->type == TABS))
	if (ISDATA(a) && !ISDATA(b))
		return -1;
	if (!ISDATA(a) && ISDATA(b))
		return 1;
	if (a->value < b->value)
		return -1;
	return (a->value > b->value);
#undef ISDATA
}

int compare_constab (const void *pa, const void *pb)
{
	const struct constab *a = pa, *b = pb;

	if (a->sym < b->sym)
		return -1;
	return (a->sym > b->sym);
}

/*
 * Print the table of symbols and text constants.
 */
void listing ()
{
	struct stab *s;
	struct constab *c;
	struct source_line *l;
	char *p, *lstname;
	FILE *lstfile;
	int t, n;

	lstname = malloc (4 + strlen (outfile));
	if (! lstname)
		uerror ("out of memory");
	strcpy (lstname, outfile);
	p = strrchr (lstname, '.');
	if (! p)
		p = lstname + strlen (lstname);
	strcpy (p, ".lst");

	lstfile = fopen (lstname, "w");
	if (! lstfile)
		uerror ("cannot write to %s", lstname);

	fprintf (lstfile, "Code:\n");
	for (l=sources; l<sources+source_line_count; ++l) {
		fprintf (lstfile, " %4d", l->line_number);
		if (l->address >= 0)
			fprintf (lstfile, " %04x %04x", l->address, text[l->address]);
		else if (l->label_address >= 0)
			fprintf (lstfile, " %04x     ", l->label_address);
		else
			fprintf (lstfile, "          ");

		fprintf (lstfile, "\t%s", l->text);

		if (l->address >= 0)
			for (n=l->address+1; n<=l->end_address; ++n)
				fprintf (lstfile, "      %04x %04x\n", n, text[n]);
	}
	fprintf (lstfile, "\n");

	/* Remember the addresses of constants. */
	for (c=constab; c<constab+nconst; ++c)
		c->sym = stab[c->sym].value;

	/* Sort the symbol table. */
	qsort (stab, stabfree, sizeof (stab[0]), compare_stab);

	fprintf (lstfile, "Data symbols:\n");
	for (s=stab; s<stab+stabfree; ++s) {
		if (s->name[1] == '.')
			continue;
		switch (s->type) {
		default:     continue;
		case TDATA:  t = 'D'; break;
		case TABS:   t = 'A'; break;
		}
		fprintf (lstfile, "      %04x  %c  %.*s\n",
			s->value, t, s->len, s->name);
	}
	fprintf (lstfile, "\nCode symbols:\n");
	for (s=stab; s<stab+stabfree; ++s) {
		if (s->name[1] == '.')
			continue;
		switch (s->type) {
		default:     continue;
		case TUNDF:  t = 'U'; break;
		case TTEXT:  t = 'T'; break;
		case TCONST: t = 'C'; break;
		}
		fprintf (lstfile, "      %04x  %c  %.*s\n",
			s->value, t, s->len, s->name);
	}
	t = 0;
	for (n=TXTSIZE-1; n>=0; --n)
		if (tbusy [n]) {
			t = n;
			break;
		}
	fprintf (lstfile, "      %04x  T  <end>\n", t+1);

	/* Sort the table of constants. */
	qsort (constab, nconst, sizeof (constab[0]), compare_constab);
	fprintf (lstfile, "\nText constants:\n");
	for (c=constab; c<constab+nconst; ++c) {
		/* Skip repeated constants. */
		if (c > constab && c->sym < c[-1].sym + c[-1].len)
			continue;

		fprintf (lstfile, "      %04x  C  \"", c->sym);
		for (p=c->val; p<c->val+c->len; ++p) {
			switch (*p) {
			case '\n': fprintf (lstfile, "\\n");  break;
			case '\r': fprintf (lstfile, "\\r");  break;
			case '"':  fprintf (lstfile, "\\\""); break;
			case '\\': fprintf (lstfile, "\\\\"); break;
			case '\0':
				if (p < c->val+c->len-1)
					fprintf (lstfile, "\\000");
				break;
			default:
				if (*p < ' ' || *p > '~')
					fprintf (lstfile, "\\%03o", *p);
				else
					fprintf (lstfile, "%c", *p);
			}
		}
		fprintf (lstfile, "\"\n");
	}
	fprintf (lstfile, "\nDone.\n");
}

void addreloc (int addr, int sym, int flags)
{
	if (nrel >= MAXREL)
		uerror ("too many relocations");
	reltab[nrel].addr = addr;
	reltab[nrel].sym = sym;
	reltab[nrel].flags = flags;
	++nrel;
	if (debug) {
		fprintf (stderr, "reloc %d", addr);
		if (sym)
			fprintf (stderr, " %d", sym);
		if (flags & RLAB) fprintf (stderr, " RLAB");
		if (flags & RWL3) fprintf (stderr, " RWL3");
		if (flags & RWH3) fprintf (stderr, " RWH3");
		if (flags & RBL3) fprintf (stderr, " RBL3");
		if (flags & RBH3) fprintf (stderr, " RBH3");
		fprintf (stderr, "\n");
	}
}

/*
 * Compile the command.
 */
void makecmd (int code, int type)
{
	int arg, arg2, expr_type, cmdline;

	extflag = 0;
	cmdline = input_line_number;
	switch (type & ~FNOTR) {
	default:
		uerror ("internal error: invalid command type");

	case 0:			/* no argument */
		break;

	case FJUMP:             /* 12-bit argument */
		getexpr (&expr_type);
		if (expr_type == TUNDF || expr_type == TCONST)
			addreloc (count, extref, extflag);
		code |= intval & 0xfff;
		break;

	case FCON1:		/* 1-bit constant */
		getexpr (&expr_type);
		if (expr_type != TABS || intval > 1)
			uerror ("bad constant");
		code |= intval;
		break;

	case FCON4:		/* 4-bit constant */
		getexpr (&expr_type);
		if (expr_type != TABS || intval > 0x0f)
			uerror ("bad constant");
		code |= intval;
		break;

	case FSR:		/* spec.register */
		if (getlex (&arg) != LSREG)
			uerror ("invalid register number");
		code |= arg;
		break;

	case FRC8:		/* register, 8-bit constant */
		if (getlex (&arg) != LREG)
			uerror ("invalid register number");
		code |= arg;
		if (getlex (&arg) != ',')
			uerror ("comma expected");
		getexpr (&expr_type);
		if (expr_type == TUNDF || expr_type == TCONST)
			addreloc (count, extref, extflag | RWL5);
		code |= (intval & 0xff) << 5;
		break;

	case FSC8:		/* spec.register, 8-bit constant */
		if (getlex (&arg) != LSREG)
			uerror ("invalid register number");
		code |= arg;
		if (getlex (&arg2) != ',')
			uerror ("comma expected");
		getexpr (&expr_type);
		if (expr_type == TUNDF || expr_type == TCONST)
			addreloc (count, extref, extflag | RWL3);
		intval &= 0xff;
		switch (arg) {
		case 0: case 1: case 2: case 3: code |= intval & 0xf8;	break;
		case 4: case 5: case 6: case 7: code |= intval << 3;	break;
		}
		break;

	case FLDAL:		/* ldah #x, addr */
		if (getlex (&arg) != LSREG)
			uerror ("invalid register number");
		if (arg != 6 && arg != 7)
			uerror ("command ldah is used only with registers #6, #7");
		code |= arg;
		if (getlex (&arg2) != ',')
			uerror ("comma expected");
		getexpr (&expr_type);
		if (expr_type != TABS && expr_type != TDATA) {
			intval <<= 1;
			if (expr_type == TUNDF || expr_type == TCONST)
				addreloc (count, extref, extflag | RBL3);
			else if (expr_type != TTEXT)
				uerror ("code address expected");
		}
		code |= (intval & 0xff) << 3;
		break;

	case FLDAH:		/* ldah #x, addr */
		if (getlex (&arg) != LSREG)
			uerror ("invalid register number");
		if (arg != 6 && arg != 7)
			uerror ("command ldah is used only with registers #6, #7");
		code |= arg;
		if (getlex (&arg2) != ',')
			uerror ("comma expected");
		getexpr (&expr_type);
		if (expr_type != TABS && expr_type != TDATA) {
			intval <<= 1;
			if (expr_type == TUNDF || expr_type == TCONST)
				addreloc (count, extref, extflag | RBH3);
			else if (expr_type != TTEXT)
				uerror ("code address expected");
		}
		code |= (intval >> 8 & 0xff) << 3;
		break;

	case FLIAL:		/* lial is the same as ldr */
		if (getlex (&arg) != LSREG)
			uerror ("invalid register number");
		if (arg != 6 && arg != 7)
			uerror ("command ldah is used only with registers #6, #7");
		code |= arg;
		if (getlex (&arg2) != ',')
			uerror ("comma expected");
		getexpr (&expr_type);
		if (expr_type == TUNDF || expr_type == TCONST)
			addreloc (count, extref, extflag | RWL3);
		else if (expr_type != TTEXT && expr_type != TABS)
			uerror ("code address expected");
		code |= (intval & 0xff) << 3;
		break;

	case FLIAH:		/* liah #x, addr */
		if (getlex (&arg) != LSREG)
			uerror ("invalid register number");
		if (arg != 6 && arg != 7)
			uerror ("command ldah is used only with registers #6, #7");
		code |= arg;
		if (getlex (&arg2) != ',')
			uerror ("comma expected");
		getexpr (&expr_type);
		if (expr_type == TUNDF || expr_type == TCONST)
			addreloc (count, extref, extflag | RWH3);
		else if (expr_type != TTEXT && expr_type != TABS)
			uerror ("code address expected");
		code |= (intval >> 8 & 0xff) << 3;
		break;

	case FMDAL:		/* mdah #x, addr */
		if (getlex (&arg) != LREG)
			uerror ("invalid register number");
		code |= arg;
		if (getlex (&arg2) != ',')
			uerror ("comma expected");
		getexpr (&expr_type);
		if (expr_type != TABS && expr_type != TDATA) {
			intval <<= 1;
			if (expr_type == TUNDF || expr_type == TCONST)
				addreloc (count, extref, extflag | RBL5);
			else if (expr_type != TTEXT)
				uerror ("code address expected");
		}
		code |= (intval & 0xff) << 5;
		break;

	case FMDAH:		/* mdah #x, addr */
		if (getlex (&arg) != LREG)
			uerror ("invalid register name");
		code |= arg;
		if (getlex (&arg2) != ',')
			uerror ("comma expected");
		getexpr (&expr_type);
		if (expr_type != TABS && expr_type != TDATA) {
			intval <<= 1;
			if (expr_type == TUNDF || expr_type == TCONST)
				addreloc (count, extref, extflag | RBH5);
			else if (expr_type != TTEXT)
				uerror ("code address expected");
		}
		code |= (intval >> 8 & 0xff) << 5;
		break;

	case FMIAL:		/* mial is the same as ldr */
		if (getlex (&arg) != LREG)
			uerror ("invalid register name");
		code |= arg;
		if (getlex (&arg2) != ',')
			uerror ("comma expected");
		getexpr (&expr_type);
		if (expr_type == TUNDF || expr_type == TCONST)
			addreloc (count, extref, extflag | RWL5);
		else if (expr_type != TTEXT && expr_type != TABS)
			uerror ("code address expected");
		code |= (intval & 0xff) << 5;
		break;

	case FMIAH:		/* miah #x, addr */
		if (getlex (&arg) != LREG)
			uerror ("invalid register name");
		code |= arg;
		if (getlex (&arg2) != ',')
			uerror ("comma expected");
		getexpr (&expr_type);
		if (expr_type == TUNDF || expr_type == TCONST)
			addreloc (count, extref, extflag | RWH5);
		else if (expr_type != TTEXT && expr_type != TABS)
			uerror ("code address expected");
		code |= (intval >> 8 & 0xff) << 5;
		break;

	case F1OP:		/* register */
		if (getlex (&arg) != LREG)
			uerror ("invalid register name");
		code |= arg;
		break;

	case FTPR:		/* spec register, register */
		if (getlex (&arg) != LSREG)
			uerror ("invalid register number");
		code |= arg << 5;
		if (getlex (&arg) != ',')
			uerror ("comma expected");
		if (getlex (&arg) != LREG)
			uerror ("invalid register name");
		code |= arg;
		break;

	case FFPR:		/* register, spec register */
		if (getlex (&arg) != LREG)
			uerror ("invalid register name");
		code |= arg;
		if (getlex (&arg) != ',')
			uerror ("comma expected");
		if (getlex (&arg) != LSREG)
			uerror ("invalid register number");
		code |= arg << 5;
		break;

	case F2OP:		/* register, register */
		if (getlex (&arg) != LREG)
			uerror ("invalid destination name");
		code |= arg;
		if (getlex (&arg) != ',')
			uerror ("comma expected");
		if (getlex (&arg) != LREG)
			uerror ("invalid source name");
		code |= arg << 5;
		break;

	case FRC5:		/* register, 5-bit constant */
		if (getlex (&arg) != LREG)
			uerror ("invalid register number");
		code |= arg;
		if (getlex (&arg) != ',')
			uerror ("comma expected");
		getexpr (&expr_type);
		if (expr_type != TDATA && expr_type != TABS)
			uerror ("invalid value");
		code |= (intval & 0x1f) << 5;
		break;

	case FRC4:		/* register, 4-bit constant */
		if (getlex (&arg) != LREG)
			uerror ("invalid register number");
		code |= arg;
		if (getlex (&arg) != ',')
			uerror ("comma expected");
		getexpr (&expr_type);
		if (expr_type != TDATA && expr_type != TABS)
			uerror ("invalid value");
		code |= (intval & 0x0f) << 5;
		break;
	}
	set_line_address (cmdline, count);
	settext (count, code);
	++count;
}
