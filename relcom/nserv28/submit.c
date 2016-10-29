/*
 * $Header: submit.c,v 2.0 92/08/05 08:43:12 vak Exp $
 * $Log:	submit.c,v $
 * Revision 2.0  92/08/05  08:43:12  vak
 * 
 * 
 * Revision 1.3  92/06/19  22:20:35  usenet
 * Fix errors. Rudnev
 * 
 * Revision 1.2  92/06/19  22:10:59  usenet
 * 
 * Revision 1.2  92/06/19  22:08:16  usenet
 * News Server
 * 
 */
# include <stdio.h>
# include "config.h"
# include "header.h"
# include "groups.h"
# include "lang.h"

# define TMPFNAME "/tmp/NSiXXXXXX"

extern char *sender;
extern needreply;

extern long time (), filesize ();
extern char *mktemp (), *strcopy (), *canondate (), *groupclass ();
extern FILE *popen ();

char *messageid ()
{
	static char id [80];
	register char *p;
	static idn = 0, pid = -1;
	static long tim = 0;

	p = MYDOMAIN;
	if (! *p)
		p = MYADDRESS;
	else
		++p;
	if (pid < 0)
		pid = getpid ();
	if (! tim)
		tim = time ((long *) 0) & 0xffffff;
	sprintf (id, "<%u.%u%06lu@%s>", idn++, pid, tim, p);
	return (id);
}

/*
 * Submit an article.
 */

# ifdef USE_INEWS
submit ()
{
	char line [512];
	register c, lines;
	FILE *fd, *fd2;
	static char tmpf [] = TMPFNAME;

	mktemp (tmpf);
	fd2 = fopen (tmpf, "w+");
	if (! fd2) {
		error ("cannot create %s", tmpf);
		quit ();
	}

	/* rewrite text of article to temporary file */
	/* count number of lines in article */
	line [sizeof (line) - 1] = 0;
	for (lines=0; fgets (line, sizeof (line)-1, stdin); ++lines)
		fputs (line, fd2);
	fflush (fd2);
	rewind (fd2);

	/* check list of groups */
	if (! checkgroups (tmpf)) {
		error ("bad Newsgroups: %s", h_newsgroups);
		printf (unsent_message_follows);
		printunsent (stdout, tmpf);
		fclose (fd2);
		unlink (tmpf);
		return (0);
	}
	unlink (tmpf);
	fd = fopen (tmpf, "w");
	if (! fd) {
		error ("cannot create %s", tmpf);
		quit ();
	}

	writehead (fd, lines);

	/* copy article body */
	while ((c = getc (fd2)) >= 0)
		putc (c, fd);
	fclose (fd2);
	fclose (fd);

	/* run inews */
	if (run ("%s -h < %s", INEWSNAME, tmpf) != 0) {
		error ("inews failed");
		message (cannot_post_article);
		printf (unsent_message_follows);
		printunsent (stdout, tmpf);
		unlink (tmpf);
		return (0);
	}
	parsestat (filesize (tmpf));
	return (1);
}
#else
submit ()
{
	register c, lines;
	FILE *fd, *fd2;
	long t;
	char incoming [512], line [512];
	static char tmpf [] = TMPFNAME;

	mktemp (tmpf);
	fd2 = fopen (tmpf, "w+");
	if (! fd2) {
		error ("cannot create %s", tmpf);
		quit ();
	}

	/* rewrite text of article to temporary file */
	/* count number of lines in article */
	line [sizeof (line) - 1] = 0;
	for (lines=0; fgets (line, sizeof (line)-1, stdin); ++lines)
		fputs (line, fd2);
	fflush (fd2);
	rewind (fd2);

	/* check list of groups */
	if (! checkgroups (tmpf)) {
		error ("bad Newsgroups: %s", h_newsgroups);
		printf (unsent_message_follows);
		printunsent (stdout, tmpf);
		fclose (fd2);
		unlink (tmpf);
		return (0);
	}
	unlink (tmpf);

	time (&t);
	for (;;) {
		sprintf (incoming, "%s/in.coming/%ld.t", NEWSSPOOLDIR, t);
		if (access (incoming, 0) < 0)
			break;
		++t;
	}
	fd = fopen (incoming, "w");
	if (! fd) {
		error ("cannot create %s", incoming);
		quit ();
	}

	writehead (fd, lines);

	/* copy article body */
	while ((c = getc (fd2)) >= 0)
		putc (c, fd);
	fclose (fd2);
	fclose (fd);

	parsestat (filesize (incoming));
	return (1);
}
#endif

char *nextword (p)
register char **p;
{
	static char groupname [128];
	register char *g;

	for (g=groupname; **p; ++g, ++(*p)) {
		if (g > groupname + sizeof (groupname)-2)
			break;
		if (**p==' ' || **p==',' || **p=='\t')
			break;
		*g = **p;
	}
	*g = 0;
	while ( **p== ',' || **p == ' ' || **p == '\t' )
	       ++(*p);
	return (groupname);
}


writehead (fd, lines)                   /* write article header */
FILE *fd;
{
	char *p, *gname , cc;
	fprintf (fd, "Path: newsserv\n");
	if (h_from)
		fprintf (fd, "From: %s\n", h_from);
	if (h_resent_from)
		fprintf (fd, "Resent-From: %s\n", h_resent_from);
	/* Поскольку идет крайне много ошибок в разделителе, будем понимать и
	 * запятую, и пробел, и табуляцию
	 */
	p = h_newsgroups;
	fprintf (fd, "Newsgroups:");
	cc = ' ';
	while (*p) {
		gname = nextword (&p);
		if (! gname)
			break;
		if (! *gname)
			continue;
	       fprintf(fd,"%c%s",cc,gname);
	       cc = ',';
	}
	fprintf(fd,"\n");
	if (h_subject) {
		if (!strncmp (h_subject, "[NEWS] ", 7) ||
		    !strncmp (h_subject, "[News] ", 7))
			fprintf (fd, "Subject: %s\n", h_subject + 7);
		else if (!strncmp (h_subject, "Re: [NEWS] ", 11) ||
		    !strncmp (h_subject, "re: [NEWS] ", 11) ||
		    !strncmp (h_subject, "Re: [News] ", 11) ||
		    !strncmp (h_subject, "re: [News] ", 11))
			fprintf (fd, "Subject: Re: %s\n", h_subject + 11);
		else if (!strncmp (h_subject, "Re:[NEWS] ", 10) ||
		    !strncmp (h_subject, "re:[NEWS] ", 10) ||
		    !strncmp (h_subject, "Re:[News] ", 10) ||
		    !strncmp (h_subject, "re:[News] ", 10))
			fprintf (fd, "Subject: Re: %s\n", h_subject + 10);
		else
			fprintf (fd, "Subject: %s\n", h_subject);
	} else
		fprintf (fd, "Subject: <none>\n");
	fprintf (fd, "Date: %s\n", canondate (h_date));
	fprintf (fd, "Distribution: %s\n", h_distribution ?
		h_distribution : "world");
	fprintf (fd, "Organization: %s\n", h_organization ?
		h_organization : "unknown");
	fprintf (fd, "Message-ID: %s\n", h_message_id ?
		h_message_id : messageid ());
	fprintf (fd, "Sender: %s%s\n", MYADDRESS, MYDOMAIN);
	fprintf (fd, "Reply-To: %s\n", h_reply_to ? h_reply_to : sender);
	if (h_approved)
		fprintf (fd, "Approved: %s\n", h_approved);
	if (h_expires)
		fprintf (fd, "Expires: %s\n", h_expires);
	if (h_supersed)
		fprintf (fd, "Supersedes: %s\n",h_supersed);
	if (h_keywords)
		fprintf (fd, "Keywords: %s\n", h_keywords);
	if (h_references)
		fprintf (fd, "References: %s\n", h_references);
	if (h_summary)
		fprintf (fd, "Summary: %s\n", h_summary);
	if (h_followup_to)
		fprintf (fd, "Followup-To: %s\n", h_followup_to);
	if (h_from_ )
		fprintf (fd, "X-Return-Path: %s\n",h_from_);
	if (lines)
		fprintf (fd, "Lines: %d\n", lines);
	if (h_mime_version)
		fprintf (fd, "Mime-Version: %s\n", h_mime_version);
	if (h_content_type)
		fprintf (fd, "Content-Type: %s\n", h_content_type);
	if (h_content_transfer_encoding)
		fprintf (fd, "Content-Transfer-Encoding: %s\n",
			h_content_transfer_encoding);
	if (h_content_length)
		fprintf (fd, "Content-Length: %s\n", h_content_length);

	fprintf (fd, "\n");
}

checkgroups (tmpf)
char *tmpf;
{
	char *buf, *p, *gname;
	struct activetab *a;
	int ngroups;
	int msgprinted = 0;
	long g;

	/* Parse the Newsgroups: contents */
	p = buf = strcopy (h_newsgroups);
	ngroups = 0;

	/* Check that all group names are valid */
	while (*p) {
		gname = nextword (&p);
		if (! gname)
			break;
		if (! *gname)
			continue;
		g = grouptag (gname);
		if (! g) {
			/* bad group name found */
			if (! msgprinted) {
				printf (groups_cannot_post);
				msgprinted = 1;
			}
			printf ("    %s - %s\n", gname, dont_exist);
			needreply = 1;
			continue;
		}
		a = getactive (gname);
		if (!a || a->mode=='n' || a->mode=='x') {
			/* cannot post to this group */
			if (! msgprinted) {
				printf (groups_cannot_post);
				msgprinted = 1;
			}
			printf ("    %s - %s\n", gname, closed_for_posting);
			needreply = 1;
			continue;
		}
		if (a->mode=='m' && !h_approved) {
			/* unapproved article: mail it to the moderator(s) */
			char cmd [512];
			FILE *pd;

			sprintf (cmd, "%s/bin/mailmdrr %s", SERVDIR, gname);
			pd = popen (cmd, "w");
			if (pd) {
				printunsent (pd, tmpf);
				messg ("article from %s posted to %s moderator", h_from, gname);
			}
			if (! pd || pclose (pd) != 0) {
				/* cannot mail to moderator */
				if (! msgprinted) {
					printf (groups_cannot_post);
					msgprinted = 1;
				}
				printf ("    %s - %s\n", gname, no_moderator);
				error ("moderator %s not foundr", gname);
				needreply = 1;
				continue;
			}
		}
		++ngroups;
	}
	free (buf);

	if (! ngroups) {
		needreply = 1;
		printf (bad_newsgroups_field, h_newsgroups);
		return (0);
	}
	return (1);
}

printunsent (fd, fname)
FILE *fd;
char *fname;
{
	FILE *fd2;
	register c;

	fd2 = fopen (fname, "r");
	if (! fd2)
		return;

	writehead (fd, 0);

	/* copy article body */
	while ((c = getc (fd2)) >= 0)
		putc (c, fd);
	fclose (fd2);
}

static strintab (str, tab, ntab)
char *str, **tab;
{
	int i;

	for (i=0; i<ntab; ++i)
		if (! strcmp (str, tab [i]))
			return (1);
	return (0);
}

parsestat (size)
long size;
{
	char *buf, *p, *gname, *gclass;
	char *tab [128];
	int ntab;

	p = buf = strcopy (h_newsgroups);
	ntab = 0;
	while (*p) {
		gname = nextword (&p);
		if (! gname)
			break;
		if (! *gname)
			continue;
		gclass = groupclass (gname);
		if (strintab (gclass, tab, ntab))
			continue;
		if (ntab >= sizeof (tab) / sizeof (tab [0]))
			break;
		tab [ntab++] = gclass;
	}
	free (buf);
	instat (tab, ntab, h_from_ ? h_from_ : sender, size);
}
