# include <rpc/rpc.h>
# include <sys/time.h>
# include <stdio.h>
# include "rgroups.h"

# include "config.h"

static char host [256] = "localhost";
static void *voidarg;

extern FILE *f2open ();

static CLIENT *rundb ()
{
	CLIENT *rgroupd;
	char cmd [256];
	int pid;

	sprintf (cmd, "%s/bin/rgroupd", SERVDIR);

	/* let's fork */
	pid = fork ();
	if (pid < 0) {
		error ("cannot fork");
		quit ();
	}
	if (! pid) {
		/* we are a child */
		execl (cmd, "rgroupd", (char *) 0);
		_exit (-1);
	}
	sleep (60);
	messg ("database server run");
	rgroupd = clnt_create (host, RGROUPSPROG, RGROUPSVERS, "tcp");
	if (! rgroupd) {
		error ("cannot connect to database on %s, halted",
			host);
		quit ();
	}
	return (rgroupd);
}

/*
 * Проверка "жив курилка, не помер".
 * Тонкое место: таймаут должен быть солидный,
 * а вдруг идет обновление (перепись) базы,
 * это дело долгое.
 */

pinggroups ()
{
	static struct timeval timo = { 600,  0 }; /* 10 минут */
	CLIENT *rgroupd;
	FILE *fd;
	int pid, cnt;

/*
	if (gethostname (host, sizeof (host) - 1) < 0) {
		error ("cannot get host name");
		return (0);
	}
 */
	rgroupd = clnt_create (host, RGROUPSPROG, RGROUPSVERS, "tcp");
	if (! rgroupd)
		rgroupd = rundb ();
	for (cnt=0; cnt<10; ++cnt)
		if (clnt_call (rgroupd, 0L, xdr_void, voidarg,
		    xdr_void, voidarg, timo) == RPC_SUCCESS)
			break;
	if (cnt >= 10) {
		error ("cannot call database on %s", host);
		clnt_destroy (rgroupd);

		fd = f2open (SERVDIR, "rgrouppid", "r");
		fscanf (fd, "%d", &pid);
		fclose (fd);

		kill (pid, 15);
		sleep (2);
		kill (pid, 15);
		sleep (2);

		rgroupd = rundb ();
	}
	clnt_destroy (rgroupd);
}
