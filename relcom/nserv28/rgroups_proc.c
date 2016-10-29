# include <stdio.h>
# include <rpc/rpc.h>
# include "rgroups.h"
# include "groups.h"
# include "config.h"

typedef char *string;

static void *OK = (void *) &OK;         /* abstract non-NULL pointer */
static refcnt;                          /* reference count */

void quit ()
{
	svc_unregister (RGROUPSPROG, RGROUPSVERS);
	if (refcnt)
		savegroups ();
	exit (-1);
}

# ifndef LOG_TO_STDERR
/*
 * Output error message to log file.
 * VARARGS1
 */

messg (s, a, b, c, d)
char *s, *a, *b, *c, *d;
{
	log ("g", s, a, b, c, d);
}

/*
 * Output error message to log file.
 * VARARGS1
 */

error (s, a, b, c, d)
char *s, *a, *b, *c, *d;
{
	log ("g +ERR+", s, a, b, c, d);
}

# else

/* VARARGS1 */
void messg (s, a, b, c, d)
char *s, *a, *b, *c, *d;
{
	fprintf (stderr, s, a, b, c, d);
	fprintf (stderr, "\n");
}
/* VARARGS1 */
void error (s, a, b, c, d)
char *s, *a, *b, *c, *d;
{
	fprintf (stderr, s, a, b, c, d);
	fprintf (stderr, "\n");
}
# endif

void           *die_1           ()
{
	/*
	 * Unregister the server and die gracefully.
	 */
	svc_unregister (RGROUPSPROG, RGROUPSVERS);
	if (refcnt)
		savegroups ();
	exit (0);
	return (OK);
}

void           *restart_1       ()
{
	/*
	 * Unregister the server and start up again.
	 */
	char myname [512];

	if (refcnt)
		savegroups ();
	refcnt = 0;
	strcpy (myname, SERVDIR);
	strcat (myname, "/bin/rgroupd");
	execl (myname, (char *) 0);
	return (OK);
}

int             *loadgroups_1   (waitflag)
int *waitflag;
{
	static int rez;

	if (! refcnt) {
		rez = loadgroups (*waitflag);
		if (rez == 1)
			++refcnt;
		return (&rez);
	}
	++refcnt;
	rez = 1;
	return (&rez);
}

int             *savegroups_1   ()
{
	static int rez;

	/*
	 * Reference count is never reduced.
	 */
	rez = refcnt ? 0 : -1;
	return (&rez);
}

long            *grouptag_1     (name)
string *name;
{
	static long rez;

	if (! refcnt) {
		rez = 0;
		return (&rez);
	}
	rez = grouptag (*name);
	return (&rez);
}

long            *usertag_1      (name)
string *name;
{
	static long rez;

	if (! refcnt) {
		rez = 0;
		return (&rez);
	}
	rez = usertag (*name);
	return (&rez);
}

long            *adduser_1      (name)
string *name;
{
	static long rez;

	if (! refcnt) {
		rez = 0;
		return (&rez);
	}
	rez = adduser (*name);
	return (&rez);
}

int             *deluser_1      (name)
string *name;
{
	static int rez;

	if (! refcnt) {
		rez = 0;
		return (&rez);
	}
	rez = deluser (*name);
	return (&rez);
}

intvect         *delusertab_1  (parms)
longvect *parms;
{
	static intvect rez;
	static int *tab;

	if (! refcnt) {
		rez.intvect_val = 0;
		rez.intvect_len = 0;
		return (&rez);
	}
	if (tab)
		free ((char *) tab);
	tab = delusertab (parms->longvect_val,
		parms->longvect_len);
	rez.intvect_val = tab;
	rez.intvect_len = tab ? parms->longvect_len : 0;
	return (&rez);
}

long            *addgroup_1     (parms)
addgroupparms *parms;
{
	static long rez;

	if (! refcnt) {
		rez = 0;
		return (&rez);
	}
	rez = addgroup (parms->name, parms->last);
	return (&rez);
}

int             *delgroup_1     (name)
string *name;
{
	static int rez;

	if (! refcnt) {
		rez = 0;
		return (&rez);
	}
	rez = delgroup (*name);
	return (&rez);
}

int             *subscribe_1    (parms)
subscrparms *parms;
{
	static int rez;

	if (! refcnt) {
		rez = 0;
		return (&rez);
	}
	rez = subscribe (parms->user, parms->group, parms->mode);
	return (&rez);
}

int             *unsubscribe_1  (parms)
unsubscrparms *parms;
{
	static int rez;

	if (! refcnt) {
		rez = 0;
		return (&rez);
	}
	rez = unsubscribe (parms->user, parms->group);
	return (&rez);
}

intvect		*unsubscrtab_1  (parms)
longvect *parms;
{
	static intvect rez;
	static int *tab;

	if (! refcnt) {
		rez.intvect_val = 0;
		rez.intvect_len = 0;
		return (&rez);
	}
	if (tab)
		free ((char *) tab);
	tab = unsubscrtab (parms->longvect_val,
		parms->longvect_len);
	rez.intvect_val = tab;
	rez.intvect_len = tab ? parms->longvect_len : 0;
	return (&rez);
}

int             *unsubscrall_1  (group)
long *group;
{
	static int rez;

	if (! refcnt) {
		rez = 0;
		return (&rez);
	}
	rez = unsubscrall (*group);
	return (&rez);
}

long            *userflags_1    (user)
long *user;
{
	static long rez;

	if (! refcnt) {
		rez = 0;
		return (&rez);
	}
	rez = userflags (*user);
	return (&rez);
}

long            *grouplast_1    (group)
long *group;
{
	static long rez;

	if (! refcnt) {
		rez = 0;
		return (&rez);
	}
	rez = grouplast (*group);
	return (&rez);
}

string          *groupname_1    (group)
long *group;
{
	static string rez;

	if (! refcnt) {
		rez = "";
		return (&rez);
	}
	rez = groupname (*group);
	if (! rez)
		rez = "";
	return (&rez);
}

string          *username_1     (user)
long *user;
{
	static string rez;

	if (! refcnt) {
		rez = "";
		return (&rez);
	}
	rez = username (*user);
	if (! rez)
		rez = "";
	return (&rez);
}

void            *groupslimit_1  (limit)
int *limit;
{
	if (refcnt)
		groupslimit (*limit);
	return (OK);
}

void            *groupssync_1   ()
{
	if (refcnt)
		groupssync ();
	return (OK);
}

longvect         *userlist_1     ()
{
	static longvect rez;
	int len;

	if (! refcnt) {
		rez.longvect_val = 0;
		rez.longvect_len = 0;
		return (&rez);
	}
	if (rez.longvect_val)
		free ((char *) rez.longvect_val);
	rez.longvect_val = userlist (&len);
	rez.longvect_len = len;
	return (&rez);
}

longvect        *grouplist_1    ()
{
	static longvect rez;
	int len;

	if (! refcnt) {
		rez.longvect_val = 0;
		rez.longvect_len = 0;
		return (&rez);
	}
	if (rez.longvect_val)
		free ((char *) rez.longvect_val);
	rez.longvect_val = grouplist (&len);
	rez.longvect_len = len;
	return (&rez);
}

subscrvect      *groupsubscr_1  (group)
long *group;
{
	static subscrvect rez;
	int len;

	if (! refcnt) {
		rez.subscrvect_val = 0;
		rez.subscrvect_len = 0;
		return (&rez);
	}
	rez.subscrvect_val = (subscrrec *) groupsubscr (*group, &len);
	rez.subscrvect_len = len;
	return (&rez);
}

statrec         *groupstat_1  (group)
long *group;
{
	static statrec rez;
	int len;

	if (! refcnt) {
		rez.subs = rez.feed = rez.rfeed = 0;
		return (&rez);
	}
	groupstat (*group, &rez.subs, &rez.feed, &rez.rfeed);
	return (&rez);
}

statvect         *userstat_1  (parms)
longvect *parms;
{
	static statvect rez;
	static struct stattab *tab;
	int len;

	if (! refcnt) {
		rez.statvect_val = 0;
		rez.statvect_len = 0;
		return (&rez);
	}
	if (tab)
		free ((char *) tab);
	tab = userstat (parms->longvect_val, parms->longvect_len);
	rez.statvect_val = (statrec *) tab;
	rez.statvect_len = tab ? parms->longvect_len : 0;
	return (&rez);
}

subscrvect      *usersubscr_1   (parms)
usersubscrparms *parms;
{
	static subscrvect rez;
	int len;

	if (! refcnt) {
		rez.subscrvect_val = 0;
		rez.subscrvect_len = 0;
		return (&rez);
	}
	rez.subscrvect_val = (subscrrec *) usersubscr (parms->user,
		parms->gtab.gtab_val, &len);
	rez.subscrvect_len = len;
	return (&rez);
}

void            *setuserflags_1 (parms)
setuflagparms *parms;
{
	if (refcnt)
		setuserflags (parms->tag, parms->flags);
	return (OK);
}

void            *setsubscr_1    (parms)
setsubscrparms *parms;
{
	if (refcnt)
		setsubscr (parms->group, (struct subscrtab *) parms->tab.subscrvect_val,
			parms->tab.subscrvect_len);
	return (OK);
}

void            *setgrouplast_1 (parms)
setglastparms *parms;
{
	if (refcnt)
		setgrouplast (parms->tag, parms->last);
	return (OK);
}
