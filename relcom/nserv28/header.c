/*
 * $Header: header.c,v 2.0 92/08/05 08:42:51 vak Exp $
 * $Log:	header.c,v $
 * Revision 2.0  92/08/05  08:42:51  vak
 * 
 * 
 * Revision 1.2  92/06/19  22:11:10  usenet
 * Исправлено чтение заголовков - на предмет переполнения. Руднев.
 * 
 * Revision 1.2  92/06/19  22:08:23  usenet
 * News Server
 * 
 */
# include <stdio.h>
# include "header.h"

char *h_approved;
char *h_date;
char *h_distribution;
char *h_expires;
char *h_followup_to;
char *h_from;
char *h_keywords;
char *h_message_id;
char *h_newsgroups;
char *h_organization;
char *h_references;
char *h_reply_to;
char *h_resent_from;
char *h_sender;
char *h_subject;
char *h_summary;
char *h_supersed;
char *h_mime_version;
char *h_content_type;
char *h_content_transfer_encoding;
char *h_content_length;
char *h_from_;

struct headertable headertable [] = {
	"approved",             &h_approved,
	"content-length",	&h_content_length,
	"content-transfer-encoding", &h_content_transfer_encoding,
	"content-type",		&h_content_type,
	"date",                 &h_date,
	"distribution",         &h_distribution,
	"expires",              &h_expires,
	"followup-to",          &h_followup_to,
	"from",                 &h_from,
	"keywords",             &h_keywords,
	"message-id",           &h_message_id,
	"mime-version",		&h_mime_version,
	"newsgroups",           &h_newsgroups,
	"organization",         &h_organization,
	"supersedes",           &h_supersed,
	"references",           &h_references,
	"reply-to",             &h_reply_to,
	"resent-from",          &h_resent_from,
	"sender",               &h_sender,
	"subject",              &h_subject,
	"summary",              &h_summary,
	0,                      0,
};

extern char *malloc (), *strcopy (), *strcpy (), *strcat (), *strlower ();

/*
 * Get header line.
 */

char *gethline (fd)
FILE *fd;
{
	register char *p;
	char line [512];
	register c;

	c = getc (fd);
	if (c < 0) {
		error ("cannot read mail header");
		return (0);
	}
	for (p=line; c>=0 && c!='\n'; c=getc(fd)) {
		*p++ = c;
		if (p-line >= sizeof (line) - 2) {
			error ("too long header line");
			return (0);
		}
	}
	*p = 0;

	if (! line [0])
		return (0);

	p = malloc ((unsigned) (strlen (line) + 1));
	if (! p) {
		error ("out of memory");
		quit ();
	}
	strcpy (p, line);
	return (p);
}

/*
 * Parse header field.
 * Fill .value in headertable.
 */

static void parsefield (field)
char *field;
{
	register i;
	register char *name, *value;

	if (! strncmp (field, "From ", 5)) {
		value = field + 5;
		while (*value==' ' || *value=='\t')
			++value;
		for (name=value; *name && *name!=' '; ++name);
		*name = 0;
		if (*value)
			h_from_ = strcopy (value);
		return;
	}
	name = field;
	for (value=field; *value && *value!=':'; ++value);
	if (! *value)
		return;
	*value++ = 0;
	while (*value==' ' || *value=='\t')
		++value;
	if (! *value)
		return;
	strlower (name);
	for (i=0; headertable[i].name; ++i)
		if (!strcmp (name, headertable[i].name)) {
			*headertable[i].value = strcopy (value);
			break;
		}
}

/*
 * Scan mail header.
 */

scanheader (fd)
FILE *fd;
{
	register char *p;
	register c;

	freeheader ();
	for (;;) {
		/* get line from file */
		p = gethline (fd);

		/* end of header */
		if (! p)
			break;

		/* handle multiline header fields */
		c = getc (fd);
		while (c == ' ' || c == '\t') {
			register char *s, *p2, *np;
			int len;

			p2 = gethline (fd);
			if (! p2)
				break;
			for (s=p2; *s==' ' || *s=='\t'; ++s);
			len = strlen (p) + strlen (s) + 1;
			if (len < 511) {
				np = malloc ((unsigned) len + 1);
				if (! np) {
					error ("out of memory");
					quit ();
				}
				strcpy (np, p);
				strcat (np, " ");
				strcat (np, s);
				free (p);
				p = np;
			}
			free (p2);
			c = getc (fd);
		}
		ungetc (c, fd);

		parsefield (p);

		free (p);
	}
}

freeheader ()
{
	int i;

	for (i=0; headertable[i].name; ++i)
		if (*headertable[i].value) {
			free (*headertable[i].value);
			*headertable[i].value = 0;
		}
	if (h_from_) {
		free (h_from_);
		h_from_ = 0;
	}
}
