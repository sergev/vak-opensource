#define VERSION         "Mail Server, Version 1.4"
#define COPYRIGHT       "Copyright (C) 1990-95 Serge Vakulenko"

extern char *MAILBOX, *SERVDIR, *PUBLICDIR, *MAILTOCMD,
	*MYADDRESS, *CONFIGFILE, *HEADLINE, *ADMIN;

extern int DAEMONDELAY;

int config (char *name);

#ifdef INITCONFIG

char *CONFIGFILE        = "/var/spool/mailserv/config";

char *MAILBOX           = "/var/mail/mailserv";
char *SERVDIR           = "/var/spool/mailserv";
char *PUBLICDIR         = "/pub";
char *MAILTOCMD         = "/usr/sbin/sendmail";
char *MYADDRESS         = "mailserv";
char *ADMIN             = "root";
char *HEADLINE          = 0;

int DAEMONDELAY         = 30;           /* seconds */

struct {
	char *name;
	int *value;
} inttable [] = {
	{ "daemondelay",        &DAEMONDELAY    },
	{ 0 }
};

struct {
	char *name;
	char **value;
} strtable [] = {
	{ "admin",                &ADMIN        },
	{ "headline",             &HEADLINE     },
	{ "mailbox",              &MAILBOX      },
	{ "mailtocmd",            &MAILTOCMD    },
	{ "myaddress",            &MYADDRESS    },
	{ "publicdir",            &PUBLICDIR    },
	{ "servdir",              &SERVDIR      },
	{ 0 }
};

#endif /* INITCONFIG */
