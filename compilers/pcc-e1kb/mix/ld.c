/*
 *      ОС ДЕМОС СВС-Б.
 *
 *      Редактор связей.
 *
 *      Автор: Вакуленко С.
 *      Версия от 02.02.90.
 *
 *      Исходные тексты взяты из ОС ДЕМОС 2.0 для СМ-4.
 *
 *      Флаги:
 *              -o filename     output file name
 *              -u symbol       'use'
 *              -e symbol       'entry'
 *              -D size         set data size
 *              -Taddress       base address of loading
 *              -llibname       library
 *              -x              discard local symbols
 *              -X              discard locals starting with LOCSYM
 *              -S              discard all except locals and globals
 *              -C              put constants in data segment
 *              -r              preserve rel. bits, don't define common's
 *              -s              discard all symbols
 *              -n              pure procedure
 *              -d              define common even with rflag
 *              -t              tracing
 *              -k              const и text выровнены на границу листа
 */

# include <stdio.h>
# include <signal.h>
# include <sys/types.h>
# include <sys/stat.h>

# ifdef CROSS
#    include "../h/a.out.h"
#    include "../h/ar.h"
#    include "../h/ranlib.h"
# else
#    include <a.out.h>
#    include <ar.h>
#    include <ranlib.h>
# endif

# define W      8               /* длина слова в байтах */
# define LOCSYM 'L'             /* убрать локальные символы, нач. с 'L' */
# define BADDR  (HDRSZ/W)       /* память 0...BADDR-1 свободна */
# define SYMDEF "__.SYMDEF"

# define ishex(c)       ((c)<='9' && (c)>='0' || (c)<='F' && (c)>='A' || (c)<='f' && (c)>='a')
# define hexdig(c)      ((c)<='9' ? (c)-'0' : ((c)&7) + 9)

struct exec filhdr;             /* aout header */
struct ar_hdr archdr;
FILE *text, *reloc;             /* input management */

				/* output management */

FILE *outb, *coutb, *toutb, *doutb, *croutb, *troutb, *droutb, *soutb;

				/* symbol management */
struct local {
	long locindex;          /* index to symbol in file */
	struct nlist *locsymbol; /* ptr to symbol table */
};

# define NSYM           2000
# define NSYMPR         1000
# define NCONST         512
# define LLSIZE         256
# define RANTABSZ       1000

struct constab {
	long h, h2, hr, hr2;
} constab [NCONST];             /* константы */

struct nlist cursym;            /* текущий символ */
struct nlist symtab [NSYM];     /* собственно символы */
struct nlist **symhash [NSYM];  /* указатели на хэш-таблицу */
struct nlist *lastsym;          /* последний введенный символ */
struct nlist *hshtab [NSYM+2];  /* хэш-таблица для символов */
struct local local [NSYMPR];
short symindex;                 /* следующий свободный вход таб. символов */
short newindex [NCONST];        /* таблица переиндексации констант */
short nconst;                   /* след. своб. вход в constab */
short cindex;                   /* тек. индекс в newindex */
short nfile;                    /* номер тек. файла (индекс в coptsize */
short coptsize [LLSIZE];        /* длины сегментов конст. после оптимизации */
long basaddr = BADDR;           /* base address of loading */
struct ranlib rantab [RANTABSZ];
int tnum;                       /* number of elements in rantab */

long liblist [LLSIZE], *libp;   /* library management */

				/* internal symbols */

struct nlist *p_econst, *p_etext, *p_edata, *p_ebss, *p_end, *entrypt;

				/* flags */
int     trace;                  /* internal trace flag */
int     xflag;                  /* discard local symbols */
int     Xflag;                  /* discard locals starting with LOCSYM */
int     Sflag;                  /* discard all except locals and globals*/
int     Cflag;                  /* put constants in data segment */
int     rflag;                  /* preserve relocation bits, don't define commons */
int     arflag;                 /* original copy of rflag */
int     sflag;                  /* discard all symbols */
int     nflag;                  /* pure procedure */
int     dflag;                  /* define common even with rflag */
int     alflag;                 /* const и text выровнены на границу листа */

				/* cumulative sizes set in pass 1 */

long csize, tsize, dsize, bsize, asize, ssize, nsym;

				/* symbol relocation; both passes */

long ctrel, cdrel, cbrel, carel;

int	ofilfnd;
char	*ofilename = "l.out";
char	*filname;
int	errlev;
int	delarg	= 4;
char    tfname [] = "/tmp/ldaXXXXX";
char    libname [] = "/usr/SVSB/lib/libxxxxxxxxxxxxxxx";

# define LNAMLEN 17             /* originally 12 */

struct nlist **lookup (), **slookup (), *lookloc ();
long fgeth (), fputh (), add (), addlong (), atol ();
extern char * malloc ();

# define ALIGN(x,y)     ((x)+(y)-1-((x)+(y)-1)%(y))

# define MSG(l,r)       (msg ? (r) : (l))

char msg;

initmsg ()
{
	register char *p;
	extern char *getenv ();

	msg = (p = getenv ("MSG")) && *p == 'r';
}

delexit ()
{
	unlink ("l.out");
	if (!delarg && !arflag) chmod (ofilename, 0777 & ~umask(0));
	exit (delarg);
}

main (argc, argv)
char **argv;
{
	initmsg ();
	if (argc == 1) {
		printf (MSG (
"Usage: %s [-xXsSrndt] [-lname] [-D num] [-u name] [-e name] [-o file] file...\n",
"Вызов: %s [-xXsSrndt] [-lимя] [-D число] [-u имя] [-e имя] [-o файл] файл...\n"),
			argv [0]);
		exit (4);
	}
	if (signal (SIGINT, SIG_IGN) != SIG_IGN) signal (SIGINT, delexit);
	if (signal (SIGTERM, SIG_IGN) != SIG_IGN) signal (SIGTERM, delexit);

	/*
	 * Первый проход: вычисление длин сегментов и таблицы имен,
	 * а также адреса входа.
	 */

	pass1 (argc, argv);
	filname = 0;

	/*
	 * Обработка таблицы имен.
	 */

	middle ();

	/*
	 * Создание буферных файлов и запись заголовка
	 */

	setupout ();

	/*
	 * Второй проход: настройка связей.
	 */

	pass2 (argc, argv);

	/*
	 * Сброс буферов.
	 */

	finishout ();

	if (!ofilfnd) {
		unlink ("a.out");
		link ("l.out", "a.out");
		ofilename = "a.out";
	}
	delarg = errlev;
	delexit ();
	return (0);
}

pass1 (argc, argv)
char **argv;
{
	register c, i;
	long num;
	register char *ap, **p;
	char save;

	/* scan files once to find symdefs */

	p = argv + 1;
	libp = liblist;
	for (c=1; c<argc; ++c) {
		filname = 0;
		ap = *p++;

		if (*ap != '-') {
			load1arg (ap);
			continue;
		}
		for (i=1; ap[i]; i++) {
			switch (ap [i]) {

				/* output file name */
			case 'o':
				if (++c >= argc)
					error (2, MSG ("-o: argument missed",
						"-o: пропущен аргумент"));
				ofilename = *p++;
				ofilfnd++;
				continue;

				/* 'use' */
			case 'u':
				if (++c >= argc)
					error (2, MSG ("-u: argument missed",
						"-u: пропущен аргумент"));
				enter (slookup (*p++));
				continue;

				/* 'entry' */
			case 'e':
				if (++c >= argc)
					error (2, MSG ("-e: argument missed",
						"-e: пропущен аргумент"));
				enter (slookup (*p++));
				entrypt = lastsym;
				continue;

				/* set data size */
			case 'D':
				if (++c >= argc)
					error (2, MSG ("-D: argument missed",
						"-D: пропущен аргумент"));
				num = W * atoi (*p++);
				if (dsize > num)
					error (2, MSG ("-D: too small",
						"-D: слишком мало"));
				dsize = num;
				continue;

				/* base address of loading */
			case 'T':
				basaddr = atol (ap+i+1);
				break;

				/* library */
			case 'l':
				save = ap [--i];
				ap [i] = '-';
				load1arg (&ap[i]);
				ap [i] = save;
				break;

				/* discard local symbols */
			case 'x':
				xflag++;
				continue;

				/* discard locals starting with LOCSYM */
			case 'X':
				Xflag++;
				continue;

				/* discard all except locals and globals*/
			case 'S':
				Sflag++;
				continue;

				/* put constants in data segment */
			case 'C':
				Cflag++;
				continue;

				/* preserve rel. bits, don't define common */
			case 'r':
				rflag++;
				arflag++;
				continue;

				/* discard all symbols */
			case 's':
				sflag++;
				xflag++;
				continue;

				/* pure procedure */
			case 'n':
				nflag++;
				continue;

				/* define common even with rflag */
			case 'd':
				dflag++;
				continue;

				/* tracing */
			case 't':
				trace++;
				continue;

			case 'k':
				alflag++;
				continue;

			default:
				error (2, MSG ("unknown flag",
					"неизвестный флаг"));
			}
			break;
		}
	}
}

/* scan file to find defined symbols */
load1arg (cp)
register char *cp;
{
	register long nloc;

	switch (getfile (cp)) {
	case 0:                 /* regular file */
		load1 (0L, 0, mkfsym (cp, 0));
		break;
	case 1:                 /* regular archive */
		nloc = W;
archive:
		while (step (nloc))
			nloc += archdr.ar_size + ARHDRSZ;
		break;
	case 2:                 /* table of contents */
		getrantab ();
		while (ldrand ());
		freerantab ();
		*libp++ = -1;
		checklibp ();
		break;
	case 3:                 /* out of date archive */
		error (0, MSG ("out of date (warning)",
			"архив устарел (предупреждение)"));
		nloc = W + archdr.ar_size + ARHDRSZ;
		goto archive;
	}
	fclose (text);
	fclose (reloc);
}

ldrand ()
{
	register struct ranlib *p;
	struct nlist **pp;
	long *oldp = libp;

	for (p=rantab; p<rantab+tnum; ++p) {
		pp = slookup (p->ran_name);
		if (! *pp)
			continue;
		if ((*pp)->n_type == N_EXT+N_UNDF)
			step (p->ran_off);
	}
	return (oldp != libp);
}

step (nloc)
register long nloc;
{
	register char *cp;

	fseek (text, nloc, 0);
	if (!fgetarhdr (text, &archdr)) {
		*libp++ = -1;
		checklibp ();
		return (0);
	}
	cp = malloc (15);
	strncpy (cp, archdr.ar_name, 14);
	cp [14] = '\0';
	if (load1 (nloc + ARHDRSZ, 1, mkfsym (cp, 0)))
		*libp++ = nloc;
	free (cp);
	checklibp ();
	return (1);
}

checklibp ()
{
	if (libp >= &liblist[LLSIZE])
		error (2, MSG ("library table overflow",
			"переполнена таблица библиотек"));
}

freerantab ()
{
	register struct ranlib *p;

	for (p=rantab; p<rantab+tnum; ++p)
		free (p->ran_name);
}

getrantab ()
{
	register struct ranlib *p;
	register n;

	for (p=rantab; p<rantab+RANTABSZ; ++p) {
		n = fgetran (text, p);
		if (n < 0)
			error (2, MSG ("out of memory", "мало памяти"));
		if (n == 0) {
			tnum = p-rantab;
			return;
		}
	}
	error (2, MSG ("ranlib buffer overflow",
		"переполнена таблица ranlib"));
}

/* single file */
load1 (loc, libflg, nloc)
long loc;
{
	register struct nlist *sp;
	int savindex, savcindex;
	int ndef, type, symlen, nsymbol;

	readhdr (loc);
	if (filhdr.a_flag & RELFLG) {
		error (1, MSG ("file stripped", "нет информации о настройке"));
		return (0);
	}
	savcindex = cindex;
	fseek (reloc, loc + N_SYMOFF (filhdr), 0);
	coptsize[nfile] = passconst ();
	ctrel += tsize/W;
	cdrel += dsize/W;
	cbrel += bsize/W;
	carel += asize/W;
	loc += HDRSZ + (filhdr.a_const + filhdr.a_text + filhdr.a_data) * 2;
	fseek (text, loc, 0);
	ndef = 0;
	savindex = symindex;
	if (nloc) nsymbol = 1; else nsymbol = 0;
	for (;;) {
		symlen = fgetsym (text, &cursym);
		if (symlen == 0)
			error (2, MSG ("out of memory", "мало памяти"));
		if (symlen == 1)
			break;
		type = cursym.n_type;
		if (Sflag && ((type & N_TYPE) == N_ABS ||
			(type & N_TYPE) > N_ACOMM))
		{
			free (cursym.n_name);
			continue;
		}
		if (! (type & N_EXT)) {
			if (!sflag && !xflag &&
			    (!Xflag || cursym.n_name[0] != LOCSYM)) {
				nsymbol++;
				nloc += symlen;
			}
			free (cursym.n_name);
			continue;
		}
		symreloc ();
		if (enter (lookup ())) continue;
		free (cursym.n_name);
		if (cursym.n_type == N_EXT+N_UNDF) continue;
		sp = lastsym;
		if (sp->n_type == N_EXT+N_UNDF ||
			sp->n_type == N_EXT+N_COMM ||
			sp->n_type == N_EXT+N_ACOMM)
		{
			if (cursym.n_type == N_EXT+N_COMM ||
				cursym.n_type == N_EXT+N_ACOMM)
			{
				sp->n_type = cursym.n_type;
				if (cursym.n_value > sp->n_value)
					sp->n_value = cursym.n_value;
			}
			else if (sp->n_type==N_EXT+N_UNDF ||
				cursym.n_type==N_EXT+N_DATA ||
				cursym.n_type==N_EXT+N_BSS)
			{
				ndef++;
				sp->n_type = cursym.n_type;
				sp->n_value = cursym.n_value;
			}
		}
	}
	if (! libflg || ndef) {
		csize = add (csize, (long) W * coptsize[nfile++],
			MSG ("const segment overflow",
				"переполнен сегмент const"));
		tsize = add (tsize, filhdr.a_text,
			MSG ("text segment overflow",
				"переполнен сегмент text"));
		dsize = add (dsize, filhdr.a_data,
			MSG ("data segment overflow",
				"переполнен сегмент data"));
		bsize = add (bsize, filhdr.a_bss,
			MSG ("bss segment overflow",
				"переполнен сегмент bss"));
		asize = addlong (asize, filhdr.a_abss,
			MSG ("abss segment overflow",
				"abss overflow"));
		ssize = add (ssize, (long) nloc,
			MSG ("symbol table overflow",
				"переполнена таблица символов"));
		nsym += nsymbol;
		return (1);
	}

	/*
	 * No symbols defined by this library member.
	 * Rip out the hash table entries and reset the symbol table.
	 */

	cindex = savcindex;
	nconst -= coptsize[nfile];
	while (symindex > savindex) {
		register struct nlist **p;

		p = symhash[--symindex];
		free ((*p)->n_name);
		*p = 0;
	}
	return (0);
}

passconst ()
{
	register short count;
	short save;
	register struct constab *p, *c;

	save = nconst;
	count = filhdr.a_const / W;
	c = &constab[nconst];
	while (count--) {
		c->h = fgeth (text);
		c->h2 = fgeth (text);
		c->hr = fgeth (reloc);
		c->hr2 = fgeth (reloc);
		p = c;
		if (!c->hr && !c->hr2) for (p=constab; p<c; p++)
			if (!p->hr2 && c->h==p->h && c->h2==p->h2 && !p->hr)
				break;
		if (p==c && ++c >= &constab[NCONST])
			error (2, MSG ("constant table overflow",
				"переполнена таблица литералов"));
		newindex[cindex++] = p - constab;
	}
	nconst = c - constab;
	return (nconst - save);
}

/* used after pass 1 */
long    corigin;
long    cbasaddr;
long    torigin;
long    dorigin;
long    borigin;
long    aorigin;

middle()
{
	register struct nlist *sp, *symp;
	register long t;
	register long cmsize, acmsize;
	int nund;
	long cmorigin, acmorigin;

	p_econst = *slookup ("_econst");
	p_etext = *slookup ("_etext");
	p_edata = *slookup ("_edata");
	p_ebss = *slookup ("_ebss");
	p_end = *slookup ("_end");

	/*
	 * If there are any undefined symbols, save the relocation bits.
	 */

	symp = &symtab[symindex];
	if (!rflag) {
		for (sp=symtab; sp<symp; sp++)
			if (sp->n_type == N_EXT+N_UNDF &&
				sp != p_end && sp != p_ebss && sp != p_edata &&
				sp != p_etext && sp != p_econst)
			{
				rflag++;
				dflag = 0;
				break;
			}
	}
	if (rflag) Cflag = alflag = nflag = sflag = 0;

	/*
	 * Assign common locations.
	 */

	cmsize = 0;
	acmsize = 0;
	if (dflag || !rflag) {
		ldrsym (p_econst, csize/W, N_EXT+N_CONST);
		ldrsym (p_etext, tsize/W, N_EXT+N_TEXT);
		ldrsym (p_edata, dsize/W, N_EXT+N_DATA);
		ldrsym (p_ebss, bsize/W, N_EXT+N_BSS);
		ldrsym (p_end, asize/W, N_EXT+N_ABSS);
		for (sp=symtab; sp<symp; sp++)
			if ((sp->n_type & N_TYPE) == N_COMM) {
				t = sp->n_value;
				sp->n_value = cmsize/W;
				cmsize = add (cmsize, (long) t*W,
					"переполнен сегмент bss");
			} else if ((sp->n_type & N_TYPE) == N_ACOMM) {
				t = sp->n_value;
				sp->n_value = acmsize/W;
				acmsize = addlong (acmsize, (long) t*W,
					 "переполнен сегмент abss");
			}
	}

	/*
	 * Now set symbols to their final value
	 */

	if (Cflag)
		torigin = basaddr;
	else {
		corigin = basaddr;
		torigin = corigin + csize/W;
	}
	if (alflag) torigin = ALIGN (torigin, 1024);
	if (Cflag) {
		corigin = torigin + tsize/W;
		dorigin = corigin + csize/W;
	} else
		dorigin = torigin + tsize/W;
	if (nflag || alflag) dorigin = ALIGN (dorigin, 1024);
	cmorigin = dorigin + dsize/W;
	borigin = cmorigin + cmsize/W;
	acmorigin = borigin + bsize/W;
	aorigin = acmorigin + acmsize/W;
	cbasaddr = corigin;
	nund = 0;
	for (sp=symtab; sp<symp; sp++) {
		switch (sp->n_type) {
		case N_EXT+N_UNDF:
			if (!arflag) errlev |= 01;
			if (!arflag)   {
				if (!nund)
					printf (MSG ("Undefined:\n",
						"Не определены:\n"));
				nund++;
				printf ("\t%s\n", sp->n_name);
			}
			break;
		default:
		case N_EXT+N_ABS:
			break;
		case N_EXT+N_CONST:
			sp->n_value += corigin;
			break;
		case N_EXT+N_TEXT:
			sp->n_value += torigin;
			break;
		case N_EXT+N_DATA:
			sp->n_value += dorigin;
			break;
		case N_EXT+N_BSS:
			sp->n_value += borigin;
			break;
		case N_EXT+N_ABSS:
			sp->n_value += aorigin;
			break;
		case N_COMM:
		case N_EXT+N_COMM:
			sp->n_type = N_EXT+N_BSS;
			sp->n_value += cmorigin;
			break;
		case N_ACOMM:
		case N_EXT+N_ACOMM:
			sp->n_type = N_EXT+N_ABSS;
			sp->n_value += acmorigin;
			break;
		}
		if (sp->n_value & ~0777777777)
			error (1, MSG ("long address: %s=0%lo",
				"длинный адрес: %s=0%lo"),
				sp->n_name, sp->n_value);
	}
	if (sflag || xflag) ssize = 0;
	bsize = add (bsize, cmsize, "переполнен сегмент bss");
	asize = addlong (asize, acmsize, "переполнен сегмент abss");

	/*
	 * Compute ssize; add length of local symbols, if need,
	 * and one more zero byte. Alignment will be taken at setupout.
	 */

	if (sflag) ssize = 0;
	else {
		if (xflag) ssize = 0;
		for (sp = symtab; sp < &symtab[symindex]; sp++)
			ssize += sp->n_len + 6;
		ssize++;
	}
}

ldrsym (sp, val, type)
register struct nlist *sp;
long val;
{
	if (sp == 0) return;
	if (sp->n_type != N_EXT+N_UNDF) {
		printf ("%s: ", sp->n_name);
		error (1, MSG ("name redefined", "имя уже определено"));
		return;
	}
	sp->n_type = type;
	sp->n_value = val;
}

setupout ()
{
	tcreat (&outb, 0);
	mktemp (tfname);
	tcreat (&coutb, 1);
	tcreat (&toutb, 1);
	tcreat (&doutb, 1);
	if (!sflag || !xflag) tcreat (&soutb, 1);
	if (rflag) {
		tcreat (&croutb, 1);
		tcreat (&troutb, 1);
		tcreat (&droutb, 1);
	}
	filhdr.a_magic = nflag ? NMAGIC : alflag ? AMAGIC : FMAGIC;
	filhdr.a_const = csize;
	filhdr.a_text = tsize;
	filhdr.a_data = dsize;
	filhdr.a_bss = bsize;
	filhdr.a_abss = asize;
	filhdr.a_syms = ALIGN (ssize, W);
	if (entrypt) {
		if (entrypt->n_type != N_EXT+N_TEXT &&
		    entrypt->n_type != N_EXT+N_UNDF)
			error (1, MSG ("entry out of text",
				"точка входа не в сегменте text"));
		else filhdr.a_entry = entrypt->n_value;
	} else
		filhdr.a_entry = torigin;
	if (rflag)
		filhdr.a_flag &= ~RELFLG;
	else
		filhdr.a_flag |= RELFLG;
	if (Cflag)
		filhdr.a_flag |= TCDFLG;
	else
		filhdr.a_flag &= ~TCDFLG;
	fputhdr (&filhdr, outb);
}

tcreat (buf, tempflg)
register FILE **buf;
register tempflg;
{
	*buf = fopen (tempflg ? tfname : ofilename, "w+");
	if (! *buf)
		error (2, tempflg ?
			MSG ("cannot create temporary file",
				"не могу создать временный файл") :
			MSG ("cannot create output file",
				"не могу создать выходной файл"));
	if (tempflg) unlink (tfname);
}

pass2 (argc, argv)
int argc;
char **argv;
{
	register int c, i;
	long dnum;
	register char *ap, **p;

	p = argv+1;
	libp = liblist;
	cindex = 0;
	nconst = 0;
	nfile = 0;
	for (c=1; c<argc; c++) {
		ap = *p++;
		if (*ap != '-') {
			load2arg (ap);
			continue;
		}
		for (i=1; ap[i]; i++) {
			switch (ap[i]) {

			case 'D':
/*
/* по-моему, все-таки, должно быть так.
				for (dnum=atoi(*p); dorigin<dnum; dorigin++) {
*/
				for (dnum=atoi(*p); dnum>0; --dnum) {
					fputh (0L, doutb);
					fputh (0L, doutb);
					if (rflag) {
						fputh (0L, droutb);
						fputh (0L, droutb);
					}
				}
			case 'u':
			case 'e':
			case 'o':
			case 'v':
				++c;
				++p;

			default:
				continue;

			case 'l':
				ap [--i] = '-';
				load2arg (&ap[i]);
				break;

			}
			break;
		}
	}
}

load2arg (acp)
register char *acp;
{
	register long *lp;

	if (getfile (acp) == 0) {
		if (trace)
			printf ("%s:\n", acp);
		mkfsym (acp, 1);
		load2 (0L);
	} else {
		/* scan archive members referenced */
		char *arname = acp;

		for (lp = libp; *lp != -1; lp++) {
			fseek (text, *lp, 0);
			fgetarhdr (text, &archdr);
			acp = malloc (15);
			strncpy (acp, archdr.ar_name, 14);
			acp [14] = '\0';
			if (trace)
				printf ("%s(%s):\n", arname, acp);
			mkfsym (acp, 1);
			free (acp);
			load2 (*lp + ARHDRSZ);
		}
		libp = ++lp;
	}
	fclose (text);
	fclose (reloc);
}

load2 (loc)
long loc;
{
	register struct nlist *sp;
	register struct local *lp;
	register int symno;
	int type;
	long count;

	readhdr (loc);
	ctrel += torigin;
	cdrel += dorigin;
	cbrel += borigin;
	carel += aorigin;

	if (trace > 1)
		printf ("ctrel=%lxh, cdrel=%lxh, cbrel=%lxh, carel=%lxh\n",
			ctrel, cdrel, cbrel, carel);
	/*
	 * Reread the symbol table, recording the numbering
	 * of symbols for fixing external references.
	 */

	lp = local;
	symno = -1;
	loc += HDRSZ;
	fseek (text, loc + (filhdr.a_const + filhdr.a_text +
		filhdr.a_data) * 2, 0);
	for (;;) {
		symno++;
		count = fgetsym (text, &cursym);
		if (count == 0)
			error (2, MSG ("out of memory", "мало памяти"));
		if (count == 1)
			break;
		symreloc ();
		type = cursym.n_type;
		if (Sflag && ((type & N_TYPE) == N_ABS ||
			(type & N_TYPE) > N_ACOMM))
		{
			free (cursym.n_name);
			continue;
		}
		if (! (type & N_EXT)) {
			if (!sflag && !xflag &&
			    (!Xflag || cursym.n_name [0] != LOCSYM))
				fputsym (&cursym, soutb);
			free (cursym.n_name);
			continue;
		}
		if (! (sp = *lookup()))
			error (2, MSG ("internal error: symbol not found",
				"внутренняя ошибка: символ не найден"));
		free (cursym.n_name);
		if (cursym.n_type == N_EXT+N_UNDF ||
			cursym.n_type == N_EXT+N_COMM ||
			cursym.n_type == N_EXT+N_ACOMM)
		{
			if (lp >= &local [NSYMPR])
				error (2, MSG ("local symbol table overflow",
					"переполнена таблица локальных имен"));
			lp->locindex = symno;
			lp++->locsymbol = sp;
			continue;
		}
		if (cursym.n_type != sp->n_type ||
			cursym.n_value != sp->n_value)
		{
			printf ("%s: ", cursym.n_name);
			error (1, MSG ("name redefined", "имя уже определено"));
		}
	}

	count = loc + filhdr.a_const + filhdr.a_text + filhdr.a_data;

	if (trace > 1)
		printf ("** CONST **\n");
	relocconst (lp);

	if (trace > 1)
		printf ("** TEXT **\n");
	fseek (text, loc + filhdr.a_const, 0);
	fseek (reloc, count + filhdr.a_const, 0);
	relocate (lp, toutb, troutb, filhdr.a_text);

	if (trace > 1)
		printf ("** DATA **\n");
	fseek (text, loc + filhdr.a_const + filhdr.a_text, 0);
	fseek (reloc, count + filhdr.a_const + filhdr.a_text, 0);
	relocate (lp, doutb, droutb, filhdr.a_data);

	nconst += coptsize[nfile];
	cindex += filhdr.a_const/W;
	corigin += coptsize[nfile];
	torigin += filhdr.a_text/W;
	dorigin += filhdr.a_data/W;
	borigin += filhdr.a_bss/W;
	aorigin += filhdr.a_abss/W;
	nfile++;
}

relocconst (lp)
struct local *lp;
{
	long r, t;
	register struct constab *p, *c;

	p = &constab[nconst];
	c = p + coptsize[nfile];
	for (; p<c; p++) {
		relhalf (lp, p->h, p->hr, &t, &r);
		fputh (t, coutb);
		if (rflag) fputh (r, croutb);
		relhalf (lp, p->h2, p->hr2, &t, &r);
		fputh (t, coutb);
		if (rflag) fputh (r, croutb);
	}
}

relocate (lp, b1, b2, len)
struct local *lp;
FILE *b1, *b2;
long len;
{
	long r, t;

	len /= W/2;
	while (len--) {
		t = fgeth (text);
		r = fgeth (reloc);
		relhalf (lp, t, r, &t, &r);
		fputh (t, b1);
		if (rflag) fputh (r, b2);
	}
}

relhalf (lp, t, r, pt, pr)
struct local *lp;
register long t, r;
long *pt, *pr;
{
	register long a, ad;
	register short i;
	register struct nlist *sp;

	if (trace > 2)
		printf ("%08x %08x", t, r);

	/* extract address from command */

	switch ((int) r & RSHORT) {
	case 0:
		a = t & 0777777777;
		break;
	case RLONG:
	case RTRUNC:
		a = t & 03777777;
		break;
	case RSHORT:
		a = t & 07777;
		break;
	case RSHIFT:
		a = t & 077777;
		a <<= 12;
		break;
	default:
		a = 0;
		break;
	 }

	/* compute address shift `ad' */
	/* update relocation word */

	ad = 0;
	switch ((int) r & REXT) {
	case RCONST:
		i = newindex [a - HDRSZ/W + cindex];
		ad = cbasaddr + i - a;
		break;
	case RTEXT:
		ad = ctrel;
		break;
	case RDATA:
		ad = cdrel;
		break;
	case RBSS:
		ad = cbrel;
		break;
	case RABSS:
		ad = carel;
		break;
	case REXT:
		sp = lookloc (lp, (int) RGETIX (r));
		r &= RSHORT;
		if (sp->n_type == N_EXT+N_UNDF ||
		    sp->n_type == N_EXT+N_COMM ||
		    sp->n_type == N_EXT+N_ACOMM)
		{
			r |= REXT | RPUTIX (nsym+(sp-symtab));
			break;
		}
		r |= reltype (sp->n_type);
		ad = sp->n_value;
		break;
	}

	/* add updated address to command */

	switch ((int) r & RSHORT) {
	case 0:
		t &= ~0777777777;
		t |= (a + ad) & 0777777777;
		break;
	case RSHORT:
		t &= ~07777;
		t |= (a + ad) & 07777;
		break;
	case RLONG:
		t &= ~03777777;
		t |= (a + ad) & 03777777;
		break;
	case RSHIFT:
		t &= ~03777777;
		t |= (a + ad) >> 12 & 03777777;
		break;
	case RTRUNC:
		t &= ~03777777;
		t |= (a + (ad & 07777)) & 03777777;
		break;
	}

	if (trace > 2)
		printf (" -> %08x %08x\n", t, r);

	*pt = t;
	*pr = r;
}

finishout ()
{
	register long n;
	register struct nlist *p;

	if (nflag || alflag) {
		if (alflag) {
			n = corigin;
			while (n & 01777) {
				n ++;
				fputh (0L, coutb);
				fputh (0L, coutb);
			}
		}
		/* now torigin points to the end of text */
		n = torigin;
		while (n & 01777) {
			n ++;
			fputh (0L, toutb);
			fputh (0L, toutb);
			if (rflag) {
				fputh (0L, troutb);
				fputh (0L, troutb);
			}
		}
	}
	if (! Cflag)
		copy (coutb);
	copy (toutb);
	if (Cflag)
		copy (coutb);
	copy (doutb);
	if (rflag) {
		if (! Cflag)
			copy (croutb);
		copy (troutb);
		if (Cflag)
			copy (croutb);
		copy (droutb);
	}
	if (! sflag) {
		if (! xflag) copy (soutb);
		for (p=symtab; p<&symtab[symindex]; ++p)
			fputsym (p, outb);
		putc (0, outb);
		while (ssize++ % W)
			putc (0, outb);
	}
	fclose (outb);
}

copy (buf)
register FILE *buf;
{
	register c;

	rewind (buf);
	while ((c = getc (buf)) != EOF) putc (c, outb);
	fclose (buf);
}

long add (a,b,s)
register long a, b;
char *s;
{
	a += b;
	if (a >= 04000000L*W) error (1, s);
	return (a);
}

long addlong (a, b, s)
register long a, b;
char *s;
{
	a += b;
	if (a >= 01000000000L*W) error (1, s);
	return (a);
}

mkfsym (s, wflag)
register char *s;
{
	register char *p;

	if (sflag || xflag) return (0);
	for (p=s; *p;) if (*p++ == '/') s = p;
	if (!wflag) return (p - s + 6);
	cursym.n_len = p - s;
	cursym.n_name = malloc (cursym.n_len + 1);
	if (! cursym.n_name)
		error (2, MSG ("out of memory", "мало памяти"));
	for (p=cursym.n_name; *s; p++, s++) *p = *s;
	cursym.n_type = N_FN;
	cursym.n_value = torigin;
	fputsym (&cursym, soutb);
	free (cursym.n_name);
	return (cursym.n_len + 6);
}

getfile (cp)
register char *cp;
{
	int c;
	struct stat x;

	text = 0;
	filname = cp;
	if (cp[0] == '-' && cp[1] == 'l') {
		if (cp[2] == '\0') cp = "-la";
		filname = libname;
		for (c = 0; cp [c+2]; c++) filname [c + LNAMLEN] = cp [c+2];
		filname [c + LNAMLEN] = '.';
		filname [c + LNAMLEN + 1] = 'a';
		filname [c + LNAMLEN + 2] = '\0';
		text = fopen (filname, "r");
		if (! text) filname += 4;
	}
	if (! text && ! (text = fopen (filname, "r")))
		error (2, MSG ("cannot open", "не могу открыть"));
	reloc = fopen (filname, "r");
	if (! reloc)
		error (2, MSG ("cannot open", "не могу открыть"));
	if (! fgetint (text, &c))
		error (1, MSG ("unexpected EOF", "преждевременный конец файла"));
	if (c != ARMAG)
		return (0);     /* regular file */
	if (! fgetarhdr (text, &archdr))
		return (1);     /* regular archive */
	if (strncmp (archdr.ar_name, SYMDEF, sizeof (archdr.ar_name)))
		return (1);     /* regular archive */
	fstat (fileno (text), &x);
	if (x.st_mtime > archdr.ar_date+2)
		return (3);     /* out of date archive */
	return (2);             /* randomized archive */
}

struct nlist ** lookup()
{
	register i;
	int clash;
	register char *cp, *cp1;
	register struct nlist **hp;

	i = 0;
	for (cp = cursym.n_name; *cp; i = (i << 1) + *cp++);
	for (hp = &hshtab[(i & 077777) % NSYM + 2]; *hp != 0;) {
		cp1 = (*hp)->n_name;
		clash = 0;
		for (cp = cursym.n_name; *cp;)
			if (*cp++ != *cp1++) {
				clash = 1;
				break;
			}
		if (clash) {
			if (++hp >= &hshtab[NSYM+2])
				hp = hshtab;
		} else
			break;
	}
	return (hp);
}

struct nlist **slookup (s)
char *s;
{
	cursym.n_len = strlen (s) + 1;
	cursym.n_name = s;
	cursym.n_type = N_EXT+N_UNDF;
	cursym.n_value = 0;
	return (lookup ());
}

enter (hp)
register struct nlist **hp;
{
	register struct nlist *sp;

	if (! *hp) {
		if (symindex >= NSYM)
			error (2, MSG ("symbol table overflow",
				"переполнена таблица символов"));
		symhash [symindex] = hp;
		*hp = lastsym = sp = &symtab[symindex++];
		sp->n_len = cursym.n_len;
		sp->n_name = cursym.n_name;
		sp->n_type = cursym.n_type;
		sp->n_value = cursym.n_value;
		return (1);
	} else {
		lastsym = *hp;
		return (0);
	}
}

symreloc()
{
	register short i;

	switch (cursym.n_type) {

	case N_CONST:
	case N_EXT+N_CONST:
		i = cindex + cursym.n_value - HDRSZ/W;
		cursym.n_value = newindex[i];
		return;

	case N_TEXT:
	case N_EXT+N_TEXT:
		cursym.n_value += ctrel;
		return;

	case N_DATA:
	case N_EXT+N_DATA:
		cursym.n_value += cdrel;
		return;

	case N_BSS:
	case N_EXT+N_BSS:
		cursym.n_value += cbrel;
		return;

	case N_ABSS:
	case N_EXT+N_ABSS:
		cursym.n_value += carel;
		return;

	case N_EXT+N_UNDF:
	case N_EXT+N_COMM:
	case N_EXT+N_ACOMM:
		return;
	}
	if (cursym.n_type & N_EXT) cursym.n_type = N_EXT+N_ABS;
}

/* VARARGS2 */

error (n, s, a, b, c)
char *s;
{
	if (!errlev) printf ("ld: ");
	if (filname) printf ("%s: ", filname);
	printf (s, a, b, c);
	printf ("\n");
	if (n > 1) delexit ();
	errlev = n;
}

struct nlist *lookloc (lp, sn)
register struct local *lp;
register sn;
{
	register struct local *clp;

	for (clp=local; clp<lp; clp++)
		if (clp->locindex == sn)
			return (clp->locsymbol);
	if (trace) {
		fprintf (stderr, "*** %d ***\n", sn);
		for (clp=local; clp<lp; clp++)
			fprintf (stderr, "%d, ", clp->locindex);
		fprintf (stderr, "\n");
	}
	error (2, MSG ("bad symbol reference", "неверная ссылка на символ"));
	/* NOTREACHED */
}

readhdr (loc)
long loc;
{
	fseek (text, loc, 0);
	if (!fgethdr (text, &filhdr))
		error (2, MSG ("bad format", "неверный формат"));
	if (filhdr.a_magic != FMAGIC)
		error (2, MSG ("bad magic", "неверный magic"));
	if (filhdr.a_const % W)
		error (2, MSG ("bad length of const", "неверная длина const"));
	if (filhdr.a_text % W)
		error (2, MSG ("bad length of text", "неверная длина text"));
	if (filhdr.a_data % W)
		error (2, MSG ("bad length of data", "неверная длина data"));
	if (filhdr.a_bss % W)
		error (2, MSG ("bad length of bss", "неверная длина bss"));
	if (filhdr.a_abss % W)
		error (2, MSG ("bad length of abss", "неверная длина abss"));
	ctrel = - BADDR - filhdr.a_const / W;
	cdrel = - BADDR - (filhdr.a_const + filhdr.a_text) / W;
	cbrel = - BADDR - (filhdr.a_const + filhdr.a_text +
		filhdr.a_data) / W;
	carel = - BADDR - (filhdr.a_const + filhdr.a_text +
		filhdr.a_data + filhdr.a_bss) / W;
}

long atol (p)
register char *p;
{
	register long l;
	register b;

	if (*p == '0')
		if (*++p == 'x' || *p == 'X') {
			b = 16;
			++p;
		} else
			b = 8;
	else
		b = 10;
	for (l=0; ishex (*p); ++p)
		l = l*b + hexdig (*p);
	return (l);
}

reltype (stype)
{
	switch (stype & N_TYPE) {
	case N_UNDF:    return (0);
	case N_ABS:     return (RABS);
	case N_CONST:   return (RCONST);
	case N_TEXT:    return (RTEXT);
	case N_DATA:    return (RDATA);
	case N_BSS:     return (RBSS);
	case N_ABSS:    return (RABSS);
	case N_STRNG:   return (RDATA);
	case N_COMM:    return (RBSS);
	case N_ACOMM:   return (RABSS);
	case N_FN:      return (0);
	default:        return (0);
	}
}
