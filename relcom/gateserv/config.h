/*
 * Конфигурация сервера по умолчанию.
 *
 * Copyright (C) 1992-1995 Cronyx Ltd.
 * Автор: Сергей Вакуленко, vak@cronyx.ru
 * Wed Feb  8 18:29:31 MSK 1995
 */
#define VERSION   "E-mail to News Gate, Version 1.0"
#define COPYRIGHT "Copyright (C) 1995 Cronyx Ltd."

extern char *MAILDIR, *NEWSSPOOLDIR, *LOGFILE, *CONFIGFILE, *LISTS, *APPROVED;
extern int DAEMONDELAY;

#ifdef INITCONFIG

char *CONFIGFILE        = "/etc/gateserv.conf";
char *MAILDIR           = "/var/mail";
char *APPROVED          = "nobody@nowhere.net";
char *LISTS             = "";
char *LOGFILE           = "/var/log/gatelog";
char *NEWSSPOOLDIR      = "/var/spool/news";

int DAEMONDELAY         = 30;           /* seconds */

struct {
	char *name;
	int *value;
} inttable [] = {
	{ "daemondelay",        &DAEMONDELAY },
	{ 0,                    0 },
};

struct {
	char *name;
	char **value;
} strtable [] = {
	{ "log",                &LOGFILE },
	{ "maildir",            &MAILDIR },
	{ "newsspooldir",       &NEWSSPOOLDIR },
	{ "lists",              &LISTS },
	{ "approved",           &APPROVED },

	{ 0,                    0 },
};

#endif /* INITCONFIG */
