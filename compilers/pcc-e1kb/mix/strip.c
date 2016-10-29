
# include <signal.h>
# include <stdio.h>

# ifdef CROSS
#    include "../h/a.out.h"
# else
#    include <a.out.h>
# endif

# ifndef BUFSIZ
# define BUFSIZ 512
# endif

char * tname;
char * mktemp();
struct exec head;
int status;
FILE * tf;

# define MSG(l,r) (msg ? (r) : (l))

char msg;

initmsg ()
{
	register char *p;
	extern char *getenv ();

	msg = (p = getenv ("MSG")) && *p == 'r';
}

main (argc, argv)
char *argv[];
{
	register i;

	initmsg ();
	signal (SIGHUP, SIG_IGN);
	signal (SIGINT, SIG_IGN);
	signal (SIGQUIT, SIG_IGN);
	tname = mktemp ("/tmp/sXXXXX");
	close (creat(tname, 0600));
	tf = fopen (tname, "w+");
	if (! tf) {
		printf (MSG ("cannot create temp file\n",
			"не могу создать временный файл\n"));
		exit (2);
	}
	for (i=1; i<argc; i++) {
		strip (argv[i]);
		if (status > 1)
			break;
	}
	fclose (tf);
	unlink (tname);
	exit (status);
}

strip (name)
char * name;
{
	register FILE * f;
	long size;

	f = fopen (name, "r");
	if (! f) {
		printf (MSG ("cannot open %s\n", "не могу открыть %s\n"),
			name);
		status = 1;
		goto out;
	}
	fgethdr (f, &head);
	if (N_BADMAG (head)) {
		printf (MSG ("%s not in a.out format\n",
				"%s не в формате a.out\n"),
			name);
		status = 1;
		goto out;
	}
	if (! head.a_syms && (head.a_flag & RELFLG)) {
		printf (MSG ("%s already stripped\n",
				"%s уже обрезано\n"),
			name);
		goto out;
	}
	size = head.a_const + head.a_text + head.a_data;
	head.a_syms = 0;
	head.a_flag |= RELFLG;
	fseek (tf, 0l, 0);
	fputhdr (&head, tf);
	if (copy (name, f, tf, size)) {
		status = 1;
		goto out;
	}
	size += HDRSZ;
	fclose (f);
	f = fopen (name, "w");
	if (! f) {
		printf (MSG ("%s cannot recreate\n",
				"не могу переоткрыть %s\n"),
			name);
		status = 1;
		goto out;
	}
	fseek (tf, 0l, 0);
	if (copy (name, tf, f, size))
		status = 2;
out:
	fclose (f);
}

copy (name, fr, to, size)
char *name;
FILE * fr, * to;
long size;
{
	register s, n;
	char buf [BUFSIZ];

	while (size != 0) {
		s = BUFSIZ;
		if (size < s)
			s = size;
		n = fread (buf, 1, s, fr);
		if (n != s) {
			printf (MSG ("%s unexpected eof\n",
					"%s преждевременный конец файла\n"),
				name);
			return (1);
		}
		n = fwrite (buf, 1, s, to);
		if (n != s) {
			printf (MSG ("%s unexpected write eof\n",
					"%s ошибка при записи\n"),
				name);
			return (1);
		}
		size -= s;
	}
	return (0);
}
