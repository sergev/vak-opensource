# include "param.h"
# include "dir.h"
# include "a.out.h"
# include "svsb.h"

/* # define DEBUG */

# define hexdig(c)      (((c)<='9' ? 0 : 9) + (c)&0xf)

# define RUNSUFF        ".sa"

struct bootcmd {
	int (*fun) ();
	char *name, *rname;
	char *args, *rargs;
	char *help, *rhelp;
};

int dohelp (), dodump (), dorun (), docat (), dodir (), dotouch ();
int doload (), dogo (), dosize (), dolang (), dols (), dood (), dodev ();

struct bootcmd cmdtab [] = {
	dohelp,         "help",         "инф",          "[cmd]",        "[ком]",
	"Print help on commands",       "Печать информации по командам",
	dodump,         "dump",         "дамп",         "[addr]",       "[адр]",
	"Memory dump",                  "Дамп памяти",
	doload,         "load",         "заг",          "name",         "имя",
	"Load file into memory",        "Загрузка файла в память",
	dogo,           "go",           "иди",          "",             "",
	"Run loaded file",              "Запуск загруженного файла",
	dorun,          "run",          "вып",          "name",         "имя",
	"Load and go",                  "Загрузка и запуск",
	docat,          "cat",          "пок",          "name",         "имя",
	"Type file on screen",          "Выдача файла на экран",
	dosize,         "size",         "разм",         "[name]",       "[имя]",
	"Show header info",             "Выдача заголовка",
	dodev,          "dev",          "уст",          "[name]",       "[имя]",
	"Set default device",           "Установка текущего устройства",
	dolang,         "lang",         "язык",         "[l|r]",        "[л|р]",
	"Set language",                 "Установка языка диагностики",
	dodir,          "dir",          "кат",          "[name]",       "[имя]",
	"Show current directory",       "Выдача текущего каталога",
	dols,           "ls",           "сп",           "[name]",       "[имя]",
	"List files in directory",      "Выдача списка файлов каталога",
	dood,           "od",           "вд",           "[name]",       "[имя]",
	"Dump file in hexadecimal",     "Выдача файла в шестнадцатеричном виде",
	dotouch,        "touch",        "проб",         "[name]",       "[имя]",
	"Open and close file",          "Открытие-закрытие файла",
	0,              0,              0,              0,              0,
};

static struct exec x;
extern openinit;

main ()
{
	char line [100];

	openinit = 0;
	meminit ();
	printf (MSG ("\nBOOT %s    %s\n", "\nГРУЗИЛО %s    %s\n"),
		BOOTVERSION, MSG (LCOPYRIGHT, RCOPYRIGHT));

	scinit ();      /* инициализация селекторных каналов */
	bcinit ();      /* инициализация байтовых каналов */

	for (;;) {
		do {
			printf (": ");
			gets (line);
		} while (! line [0]);
		if (! builtin (line))
			printf (MSG ("Unknown command '%s'\n",
				"Нет такой команды '%s'\n"), line);
	}
}

dolang (lang)
char *lang;
{
	if (lang && *lang) {
		_latmsg = (*lang=='r' || *lang=='р') ? 0 : 1;
		return;
	}
	printf (MSG ("lang is l\n", "язык р\n"));
}

doload (line)
char *line;
{
	int io;
	register i;
	register *addr;

	if (! line || ! *line) {
		printf (MSG ("Nothing to load\n", "Нечего грузить\n"));
		return (-1);
	}
	strcat (line, RUNSUFF);
	io = open (line, 0);
	if (io < 0)
		return;

	i = read(io, (char *)&x, sizeof x);
	if (i != sizeof x || N_BADMAG (x)) {
		printf (MSG ("Bad format\n", "Плохой формат\n"));
		close (io);
		return (-1);
	}
	addr = (int *) SA_BASADDR;

	/* read const segment */
	printf("%ld", x.a_const);
	if (read(io, (char *) addr, x.a_const) != x.a_const)
		goto shread;
	addr += x.a_const/8;

	/* read text segment */
	printf(" + %ld", x.a_text);
	if (read(io, (char *) addr, x.a_text) != x.a_text)
		goto shread;
	addr += x.a_text/8;

	/* read data segment */
	printf(" + %ld", x.a_data);
	if (read(io, (char *) addr, x.a_data) != x.a_data)
		goto shread;
	close (io);
	addr += x.a_data/8;

	/* clean bss segment */
	printf(" + %ld", x.a_bss);
	i = x.a_bss/8 + 1024;           /* slop */
	while (--i >= 0)
		*addr++ = 0;
	printf (MSG (" start 0x%lx\n", " старт 0x%lx\n"), x.a_entry);
	return (0);
shread:
	printf (MSG ("Short read\n", "Мало\n"));
	close (io);
	x.a_magic = 0;
	return (-1);
}

dosize (line)
char *line;
{
	int io;
	register i;
	struct exec xx;

	if (line && *line) {
		strcat (line, RUNSUFF);
		io = open (line, 0);
		if (io < 0)
			return;
		i = read (io, (char *) &xx, sizeof xx);
		close (io);
		if (i != sizeof xx) {
bad:                    printf (MSG ("Bad format\n", "Плохой формат\n"));
			return;
		}
	} else
		xx = x;
	if (N_BADMAG (xx))
		goto bad;
	printf ("%ld", xx.a_const);
	printf (" + %ld", xx.a_text);
	printf (" + %ld", xx.a_data);
	printf (" + %ld", xx.a_bss);
	printf (MSG (" start 0x%lx\n", " старт 0x%lx\n"), x.a_entry);
}

dogo ()
{
	if (N_BADMAG (x)) {
		printf (MSG ("Bad format\n", "Плохой формат\n"));
		return;
	}
	(*((int (*)()) x.a_entry))();
	_rtt ();
}

dorun (line)
char *line;
{
	if (doload (line) < 0)
		return;
	dogo ();
}

/*
 * Execute built-in command
 * Return 1 if ok or 0 if bad comand
 */
builtin (cmd)
char *cmd;
{
	register char *p;
	register struct bootcmd *c;

	/* replace tabs by spaces */
	for (p=cmd; *p; ++p)
		if (*p == '\t')
			*p = ' ';

	/* skip leading blanks */
	while (*cmd && *cmd==' ')
		++cmd;
	if (! *cmd) {
# ifdef DEBUG
		printf ("empty line on leading blanks\n");
# endif /* DEBUG */
		return (1);             /* empty line */
	}
	/* delete trailing blanks */
	while (--p>=cmd && *p==' ')
		*p = 0;
	if (p < cmd) {
# ifdef DEBUG
		printf ("empty line on trailing blanks\n");
# endif /* DEBUG */
		return (1);             /* empty line */
	}
	/* skip command name */
	for (p=cmd; *p && *p!=' '; ++p);
	if (*p == ' ')
		*p++ = 0;
	/* p now points to first parameter */

	for (c=cmdtab; c->name; ++c)
		if (! strcmp (c->name, cmd) || ! strcmp (c->rname, cmd)) {
			(*c->fun) (p, cmd);
			return (1);
		}

	/* command not found */
	return (0);
}

dohelp (p, cmd)
char *p;
{
	register struct bootcmd *c;
	int lat;

	if (p && *p) {
		for (c=cmdtab; c->name; ++c)
			if (! strcmp (c->name, p)) {
				printf ("%s\t%s\t%s\n", c->name,
					MSG (c->args, c->rargs),
					MSG (c->help, c->rhelp));
				return;
			} else if (! strcmp (c->rname, p)) {
				printf ("%s\t%s\t%s\n", c->rname,
					MSG (c->args, c->rargs),
					MSG (c->help, c->rhelp));
				return;
			}
		printf (MSG ("Unknown command '%s'\n",
			"Нет такой команды '%s'\n"), p);
		return;
	}
	lat = ! strcmp (cmd, "help");
	for (c=cmdtab; c->name; ++c)
		printf ("%s\t%s\t%s\n",
			lat ? c->name : c->rname,
			MSG (c->args, c->rargs),
			MSG (c->help, c->rhelp));
}

dodump (p)
char *p;
{
	static *addr;
	register n, i, c;

	if (p && *p) {
		for (n=0; *p; ++p)
			n = n<<4 | hexdig (*p);
		addr = (int *) n;
	}
	for (n=0; n<16; ++n, ++addr) {
		printf ("%x:\t%w ", addr, *addr);
		for (i=0; i<64; i+=8) {
			c = *addr >> i & 0xff;
			if (c<' ' || c==0177) {
				putchar ('^');
				c = c & 037 | 0100;
			} else if (c>=0200 && c<0300 || c == 0377) {
				putchar ('~');
				c = c & 037 | 0100;
			} else
				putchar (' ');
			putchar (c);
		}
		putchar ('\n');
	}
}

docat (name)
char *name;
{
	int c, i;
	char buf [1024], *p;

	i = open (name, 0);
	if (i <= 0)
		return;
	for (;;) {
		c = read (i, buf, sizeof (buf));
		if (c <= 0)
			break;
		for (p=buf; --c>=0; ++p)
			putchar (*p);
	}
	close (i);
}

dotouch (name)
char *name;
{
	int i;

	i = open (name, 0);
	if (i >= 0)
		close (i);
}

dood (name)
char *name;
{
	int c, i, addr;
	int buf [DEV_BSIZE/sizeof(int)], *p;

	i = open (name, 0);
	if (i <= 0)
		return;
	addr = 0;
	for (;;) {
		c = read (i, buf, sizeof (buf));
		if (c <= 0)
			break;
		c /= (int) sizeof (int);
		for (p=buf; c>0; c-=2, addr+=2, p+=2)
			printf ("%x:\t%w  %w\n", addr, p[0], p[1]);
	}
	close (i);
}

dodir (name)
char *name;
{
	register DIR *d;
	register struct direct *p;
	int type, length, len, n;

	if (! name || ! *name)
		name = ".";
	d = opendir (name);
	if (! d)
		return;
	for (n=0; p=readdir(d); ++n) {
		printf ("%s\t", p->d_name);
		len = strlen (p->d_name);
		if (len < 8)
			putchar ('\t');
		if (stat (d->dd_fd, p->d_name, &type, &length) < 0)
			printf ("<???>");
		else if (! type)
			printf ("%d", length);
		else
			printf (type>1 ? "<SPEC>" : "<DIR>");
		if (n & 1)
			putchar ('\n');
		else {
			putchar ('\t');
			putchar ('\t');
		}
	}
	if (n & 1)
		putchar ('\n');
	closedir (d);
}

dols (name)
char *name;
{
	register DIR *d;
	register struct direct *p;
	int n, len;

	if (! name || ! *name)
		name = ".";
	d = opendir (name);
	if (! d)
		return;
	for (n=0; p=readdir(d); ++n) {
		printf (p->d_name);
		len = strlen (p->d_name);
		if (len < 8)
			putchar ('\t');
		if ((n & 3) == 3)
			putchar ('\n');
		else
			putchar ('\t');
	}
	if (n & 3)
		putchar ('\n');
	closedir (d);
}
