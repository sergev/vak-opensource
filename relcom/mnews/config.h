/*
 * Copyright (C) 1991 Сергей Вакуленко, <vak@kiae.su>.
 * All rights reserved.
 */

extern char *MAILBOX, *SERVDIR, *LOGFILE, *CONFIGFILE;

extern int DAEMONDELAY;

# ifdef INITCONFIG

char *CONFIGFILE        = "/usr/spool/mailnews/configfile";

char *MAILBOX           = "/usr/spool/mail/news";
char *SERVDIR           = "/usr/spool/mailnews";
char *LOGFILE           = "/usr/spool/mailnews/logfile";

int DAEMONDELAY         = 10;

struct {
	char *name;
	int *value;
} inttable [] = {
	"daemondelay",          &DAEMONDELAY,
	0,                      0,
};

struct {
	char *name;
	char **value;
} strtable [] = {
	"logfile",              &LOGFILE,
	"mailbox",              &MAILBOX,
	"servdir",              &SERVDIR,

	0,                      0,
};

# endif /* INITCONFIG */
