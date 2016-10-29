#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include "header.h"

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
char *h_from_;

struct headertable headertable [] = {
	{ "approved",           &h_approved     },
	{ "date",               &h_date         },
	{ "distribution",       &h_distribution },
	{ "expires",            &h_expires      },
	{ "followup-to",        &h_followup_to  },
	{ "from",               &h_from         },
	{ "keywords",           &h_keywords     },
	{ "message-id",         &h_message_id   },
	{ "newsgroups",         &h_newsgroups   },
	{ "organization",       &h_organization },
	{ "references",         &h_references   },
	{ "reply-to",           &h_reply_to     },
	{ "resent-from",        &h_resent_from  },
	{ "sender",             &h_sender       },
	{ "subject",            &h_subject      },
	{ "summary",            &h_summary      },
	{ 0 }
};

/*
 * Get header line.
 */
static char *gethline (FILE *fd)
{
	char line [512], *p;
	int c;

	c = getc (fd);
	if (c < 0) {
		syslog (LOG_ERR, "cannot read mail header");
		return (0);
	}
	for (p=line; c>=0 && c!='\n'; c=getc(fd)) {
		*p++ = c;
		if (p-line >= sizeof (line) - 2) {
			syslog (LOG_ERR, "too long header line");
			return (0);
		}
	}
	*p = 0;

	if (! line [0])
		return (0);

	p = malloc ((unsigned) (strlen (line) + 1));
	if (! p) {
		syslog (LOG_ERR, "out of memory, aborted");
		exit (-1);
	}
	strcpy (p, line);
	return (p);
}

/*
 * Parse header field.
 * Fill .value in headertable.
 */
static void parsefield (char *field)
{
	char *name, *value;
	int i;

	if (! strncmp (field, "From ", 5)) {
		value = field + 5;
		while (*value==' ' || *value=='\t')
			++value;
		for (name=value; *name && *name!=' '; ++name);
		*name = 0;
		if (*value) {
			h_from_ = strdup (value);
			if (! h_from_) {
				syslog (LOG_ERR, "out of memory in parsefield");
				exit (-1);
			}
		}
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
	for (i=0; headertable[i].name; ++i)
		if (!strcasecmp (name, headertable[i].name)) {
			*headertable[i].value = strdup (value);
			if (! *headertable[i].value) {
				syslog (LOG_ERR, "Out of memory in parsefield");
				exit (-1);
			}
			break;
		}
}

void freeheader ()
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

/*
 * Scan mail header.
 */
void scanheader (FILE *fd)
{
	char *p;
	int c;

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
			char *s, *p2, *np;
			int len;

			p2 = gethline (fd);
			if (! p2)
				break;
			for (s=p2; *s==' ' || *s=='\t'; ++s);
			len = strlen (p) + strlen (s) + 1;
			if (len < 511) {
				np = malloc ((unsigned) len + 1);
				if (! np) {
					syslog (LOG_ERR, "out of memory, aborted");
					exit (-1);
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
