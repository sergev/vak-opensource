/*
 * Copyright (C) 1991 Сергей Вакуленко, <vak@kiae.su>.
 * All rights reserved.
 */

# define ENQUEUE        "Enqueue"
# define BATCHER        "Batcher"
# define SERVER         "Server"
# define DAEMON         "Daemon"

extern char *MAILBOX, *SERVDIR, *PUBLICDIR, *MAILTOCMD, *LOGFILE,
	*ARCHIVER, *MYADDRESS, *CONFIGFILE;

extern int DAEMONDELAY, EXPIRETIME, DOTEXPIRETIME;

# ifdef INITCONFIG

char *CONFIGFILE        = "/usr/spool/trade/configfile";

char *MAILBOX           = "/usr/spool/mail/trade";
char *SERVDIR           = "/usr/spool/trade";
char *PUBLICDIR         = "/usr/spool/trade/ar";
char *MAILTOCMD         = "/usr/lib/sendmail";
char *LOGFILE           = "/usr/spool/trade/logfile";
char *MYADDRESS         = "trade-server";
char *ARCHIVER          = "zoo aq";

int DAEMONDELAY         = 10;
int EXPIRETIME          = 24;           /* 24 hours */
int DOTEXPIRETIME       = 720;          /* 30 days */

struct {
	char *name;
	int *value;
} inttable [] = {
	"daemondelay",          &DAEMONDELAY,
	"dotexpiretime",        &DOTEXPIRETIME,
	"expiretime",           &EXPIRETIME,
	0,                      0,
};

struct {
	char *name;
	char **value;
} strtable [] = {
	"archiver",             &ARCHIVER,
	"logfile",              &LOGFILE,
	"mailbox",              &MAILBOX,
	"mailtocmd",            &MAILTOCMD,
	"myaddress",            &MYADDRESS,
	"publicdir",            &PUBLICDIR,
	"servdir",              &SERVDIR,

	0,                      0,
};

# endif /* INITCONFIG */
