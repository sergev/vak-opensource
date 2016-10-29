# include "param.h"
# include "dir.h"
# include "a.out.h"
# include "svsb.h"

# define RUNSUFF        ".sa"

char bootcmd [] = "xx(0,0)boot";

static struct exec x;

main ()
{
	meminit ();
	printf (MSG ("\nBOOT %s    %s\n", "\nГРУЗИЛО %s    %s\n"),
		BOOTVERSION, MSG (LCOPYRIGHT, RCOPYRIGHT));

	scinit ();      /* инициализация селекторных каналов */

	printf (": run %s\n", bootcmd);
	run (bootcmd);
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
