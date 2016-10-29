#include <rpc/rpc.h>
#include "rgroups.h"
#include "groups.h"

extern int errno;

static void *voidarg;
static char host [256] = "localhost";
static CLIENT *server;
static struct timeval timo = { 10*60, 0 };
static struct timeval zerotimo = { 0, 0 };

# define MAXTRY 10
static bad_count;
static void badcall (prog)
char *prog;
{
	register char *s;
	clnt_perror (server, prog);
	s = clnt_spcreateerror(prog);
	clnt_destroy (server);
	server = 0;
	error ("cannot call %s on %s cnt=%d:%s", prog, host,bad_count+1, s);
	if ( bad_count++ < MAXTRY && loadgroups(1) )
	       return;
	quit ();
}
# define BADCALL(s) {badcall(s); goto r;}


int loadgroups (waitflag)
{
	int *result;
	int iMAX = MAXTRY;
/*
	if (gethostname (host, sizeof (host) - 1) < 0) {
		error ("cannot get host name");
		return (0);
	}
 */
	server = 0;
	while ( iMAX-- > 0 && !server )
	{
	       server = clnt_create (host, RGROUPSPROG, RGROUPSVERS, "tcp");
	       if (! server)
	       {
		       error ("Connect to database failed CNT=%d", MAXTRY - iMAX);
		       if ( iMAX % 8 ) pinggroups();
		       sleep(10);
	       }
	}
	if (! server) {
		error ("cannot connect to database server on %s", host);
		return (0);
	}
	/* set bigger timeout for all subsequent remote calls */
	clnt_control (server, CLSET_TIMEOUT, &timo);

	result = loadgroups_1 (&waitflag, server);
	if (! result)
		{
		       bad_count += 4;
		       badcall ("loadgroups");
		}
	if (*result)
		groupslimit (200);
	return (*result);
}

int savegroups ()
{
	int *result;

	if (! server)
		return (-1);
r:      result = savegroups_1 (voidarg, server);
	if (! result)
		BADCALL ("savegroups");
	clnt_destroy (server);
	server = 0;
	return (*result);
}

long *userlist (cnt)
int *cnt;
{
	longvect *result;

r:      result = userlist_1 (voidarg, server);
	if (! result)
		BADCALL ("userlist");
	*cnt = result->longvect_len;
	return (result->longvect_val);
}

long *grouplist (cnt)
int *cnt;
{
	longvect *result;

r:      result = grouplist_1 (voidarg, server);
	if (! result)
		BADCALL ("grouplist");
	*cnt = result->longvect_len;
	return (result->longvect_val);
}

long grouptag (name)
char *name;
{
	long *result;

r:      result = grouptag_1 (&name, server);
	if (! result)
		BADCALL ("grouptag");
	return (*result);
}

long usertag (name)
char *name;
{
	long *result;

r:      result = usertag_1 (&name, server);
	if (! result)
		BADCALL ("usertag");
	return (*result);
}

void groupstat (g, ns, nf, nr)
long g;
int *ns, *nf, *nr;
{
	statrec *result;

r:      result = groupstat_1 (&g, server);
	if (! result)
		BADCALL ("groupstat");
	*ns = result->subs;
	*nf = result->feed;
	*nr = result->rfeed;
}

struct stattab *userstat (tab, len)
long *tab;
{
	static statvect *result;
	longvect parms;

	parms.longvect_val = tab;
	parms.longvect_len = len;
r:      result = userstat_1 (&parms, server);
	if (! result)
		BADCALL ("userstat");
	return ((struct stattab *) result->statvect_val);
}

struct subscrtab *groupsubscr (g, cnt)
long g;
int *cnt;
{
	static subscrvect *result;

	if (result && result->subscrvect_val) {
		free ((char *) result->subscrvect_val);
		result = 0;
	}
r:      result = groupsubscr_1 (&g, server);
	if (! result)
		BADCALL ("groupsubscr");
	*cnt = result->subscrvect_len;
	return ((struct subscrtab *) result->subscrvect_val);
}

struct subscrtab *usersubscr (u, gtab, cnt)
long u;
char **gtab;
int *cnt;
{
	static subscrvect *result;
	usersubscrparms parms;

	if (result && result->subscrvect_val) {
		free ((char *) result->subscrvect_val);
		result = 0;
	}
	parms.user = u;
	parms.gtab.gtab_val = gtab;
	for (parms.gtab.gtab_len=0; gtab && *gtab; ++gtab)
		++parms.gtab.gtab_len;
r:      result = usersubscr_1 (&parms, server);
	if (! result)
		BADCALL ("usersubscr");
	*cnt = result->subscrvect_len;
	return ((struct subscrtab *) result->subscrvect_val);
}

long addgroup (name, last)
char *name;
long last;
{
	long *result;
	addgroupparms parms;

	parms.name = name;
	parms.last = last;
r:      result = addgroup_1 (&parms, server);
	if (! result)
		BADCALL ("addgroup");
	return (*result);
}

int delgroup (name)
char *name;
{
	int *result;

r:      result = delgroup_1 (&name, server);
	if (! result)
		BADCALL ("delgroup");
	return (*result);
}

long adduser (name)
char *name;
{
	long *result;

r:      result = adduser_1 (&name, server);
	if (! result)
		BADCALL ("adduser");
	return (*result);
}

int deluser (name)
char *name;
{
	int *result;

r:      result = deluser_1 (&name, server);
	if (! result)
		BADCALL ("deluser");
	return (*result);
}

int *delusertab (tab, len)
long *tab;
{
	static intvect *result;
	longvect parms;

	if (result && result->intvect_val) {
		free ((char *) result->intvect_val);
		result = 0;
	}
	parms.longvect_val = tab;
	parms.longvect_len = len;
r:      result = delusertab_1 (&parms, server);
	if (! result)
		BADCALL ("delusertab");
	return (result->intvect_val);
}

int subscribe (u, g, mode)
long u, g;
{
	int *result;
	subscrparms parms;

	parms.user = u;
	parms.group = g;
	parms.mode = mode;
r:      result = subscribe_1 (&parms, server);
	if (! result)
		BADCALL ("subscribe");
	return (*result);
}

int unsubscribe (u, g)
long u, g;
{
	int *result;
	unsubscrparms parms;

	parms.user = u;
	parms.group = g;
r:      result = unsubscribe_1 (&parms, server);
	if (! result)
		BADCALL ("unsubscribe");
	return (*result);
}

int *unsubscrtab (tab, len)
long *tab;
{
	static intvect *result;
	longvect parms;

	if (result && result->intvect_val) {
		free ((char *) result->intvect_val);
		result = 0;
	}
	parms.longvect_val = tab;
	parms.longvect_len = len;
r:      result = unsubscrtab_1 (&parms, server);
	if (! result)
		BADCALL ("unsubscrtab");
	return (result->intvect_val);
}

int unsubscrall (u)
long u;
{
	int *result;

r:      result = unsubscrall_1 (&u, server);
	if (! result)
		BADCALL ("unsubscrall");
	return (*result);
}

long grouplast (tag)
long tag;
{
	long *result;

r:      result = grouplast_1 (&tag, server);
	if (! result)
		BADCALL ("grouplast");
	return (*result);
}

long userflags (tag)
long tag;
{
	long *result;

r:      result = userflags_1 (&tag, server);
	if (! result)
		BADCALL ("userflags");
	return (*result);
}

char *groupname (tag)
long tag;
{
	static char **result;

	if (result && *result) {
		free (*result);
		result = 0;
	}
r:      result = groupname_1 (&tag, server);
	if (! result)
		BADCALL ("groupname");
	/* When groupname fails to find the tag into the database,
	 * it returns NULL.  RPC does not allow to pass NULL
	 * as an element of <string> type.  We will avoid it
	 * by passing an empty string and then converting it
	 * to NULL at the receiving side. */
	return (**result ? *result : 0);
}

char *username (tag)
long tag;
{
	static char **result;

	if (result && *result) {
		free (*result);
		result = 0;
	}
r:      result = username_1 (&tag, server);
	if (! result)
		BADCALL ("username");
	/* When username fails to find the tag into the database,
	 * it returns NULL.  RPC does not allow to pass NULL
	 * as an element of <string> type.  We will avoid it
	 * by passing an empty string and then converting it
	 * to NULL at the receiving side. */
	return (**result ? *result : 0);
}

void groupslimit (lim)
{
	void *result;

r:      result = groupslimit_1 (&lim, server);
	if (! result)
		BADCALL ("groupslimit");
}

void groupssync ()
{
	void *result;

r:      result = groupssync_1 (voidarg, server);
	if (! result)
		BADCALL ("groupssync");
}

void setuserflags (tag, flags)
long tag, flags;
{
	void *result;
	setuflagparms parms;

	parms.tag = tag;
	parms.flags = flags;
r:      result = setuserflags_1 (&parms, server);
	if (! result)
		BADCALL ("setuserflags");
}

void setsubscr (g, tab, n)
long g;
struct subscrtab *tab;
{
	void *result;
	setsubscrparms parms;

	parms.group = g;
	parms.tab.subscrvect_val = (subscrrec *) tab;
	parms.tab.subscrvect_len = n;
r:      result = setsubscr_1 (&parms, server);
	if (! result)
		BADCALL ("setsubscr");
}

void setgrouplast (tag, last)
long tag, last;
{
	void *result;
	setglastparms parms;

	parms.tag = tag;
	parms.last = last;
r:      result = setgrouplast_1 (&parms, server);
	if (! result)
		BADCALL ("setgrouplast");
}

void restart ()
{
	clnt_control (server, CLSET_TIMEOUT, &zerotimo);
	clnt_call (server, RESTART, xdr_void, voidarg, NULL, NULL, timo);
	clnt_control (server, CLSET_TIMEOUT, &timo);
}

void die ()
{
	clnt_control (server, CLSET_TIMEOUT, &zerotimo);
	clnt_call (server, DIE, xdr_void, voidarg, NULL, NULL, timo);
	clnt_control (server, CLSET_TIMEOUT, &timo);
}
