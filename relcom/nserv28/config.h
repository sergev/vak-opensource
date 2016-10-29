/*
 * $Header: config.h,v 2.0 92/08/05 08:43:21 vak Exp $
 * $Log:	config.h,v $
 * Revision 2.0  92/08/05  08:43:21  vak
 * 
 * 
 * Revision 1.3  92/06/19  22:19:50  usenet
 * Fix errors. Rudnev
 * 
 * Revision 1.2  92/06/19  22:11:55  usenet
 * 
 * Revision 1.2  92/06/19  22:08:42  usenet
 * News Server
 * 
 */
/*
 * Copyright (C) 1991 Сергей Вакуленко, <vak@kiae.su>.
 * All rights reserved.
 */

extern int MAXARTS;         /* Макс. число рассылаемых статей в одном newnews */
extern int MAXNEWGROUPS;    /* Если появилось БОЛЬШЕ новых групп - это ошибка, синхронизуем файлы */
extern int MAXNEWARTS;      /* Если появилось БОЛЬШЕ статей - признак аварии, синхронизуем файлы  */
extern int LISTSIZE;        /* Макс. размер листа - килобайт */
extern int FEEDLIMIT;       /* Макс. размер статьи, которую можно слать по FEED */
extern int TIMEOFLIFE;       /* (дней) Макс. время жизни одного подписчика, если он ничего не шлет */
extern int UPDATETIME;       /* Макс. время между сбросами groups+ файла (изменений) */

extern char *MAILBOX, *SERVDIR, *CONFIGFILE, *NEWSLIBDIR, *NEWSSPOOLDIR,
	*LOGFILE, *DBZNAME, *MAILCMD, *MYADDRESS, *INEWSNAME, *CONFIGFILE,
	*MYSERVADDR, *MYDOMAIN, *BMAILCMD, *FMAILCMD,
	*HEADLINE, *FHEADLINE, *BHEADLINE, *AGEFILE, *GROUPINFO, *GROUPINFO_T;

extern int DAEMONDELAY, FEEDTIME, NOTIFYTIME, BATCHSIZE, TIMEOFLIFE;

# ifdef INITCONFIG

char *CONFIGFILE        = "/usr/spool/newsserv/config";
char *INEWSNAME         = "/usr/local/bin/inews";
char *DBZNAME           = "/usr/lib/newsbin/dbz";
char *LOGFILE           = "/usr/spool/newsserv/log";
char *MAILBOX           = "/usr/spool/mail/newsserv";
char *MAILCMD           = "/usr/lib/sendmail";
char *BMAILCMD          = "/usr/lib/sendmail";
char *FMAILCMD          = "/usr/lib/sendmail";
char *MYADDRESS         = "news-service";
char *MYSERVADDR        = "newsserv";
char *MYDOMAIN          = "";
char *NEWSLIBDIR        = "/usr/lib/news";
char *NEWSSPOOLDIR      = "/usr/spool/news";
char *SERVDIR           = "/usr/spool/newsserv";
char *HEADLINE          = 0;
char *BHEADLINE         = 0;
char *FHEADLINE         = 0;
char *AGEFILE           = ".lastaging";
char *GROUPINFO         = ".newsinfo";
char *GROUPINFO_T       = ".Tnewsinfo";

int DAEMONDELAY         = 10;   /* seconds */
int FEEDTIME            = 60;   /* minutes */
int NOTIFYTIME          = 360;  /* minutes */
int BATCHSIZE           = 50;   /* in kilobytes */
int MAXARTS             = 80000;
int MAXNEWGROUPS        = 100;
int MAXNEWARTS          = 12000;
int LISTSIZE            = 50;
int FEEDLIMIT           = 120;
int TIMEOFLIFE          = 28;
int UPDATETIME          = 120; /* минут */


struct {
	char *name;
	int *value;
} inttable [] = {
	"feedlimit",            &FEEDLIMIT,
	"maxarticles",          &MAXARTS,
	"maxnewgroups",         &MAXNEWGROUPS,
	"maxnewarticles",       &MAXNEWARTS,
	"daemondelay",          &DAEMONDELAY,
	"feedtime",             &FEEDTIME,
	"notifytime",           &NOTIFYTIME,
	"batchsize",            &BATCHSIZE,
	"listsize",             &LISTSIZE,
	"timeoflife",           &TIMEOFLIFE,
	"updatetime",           &UPDATETIME,
	0,                      0,
};

struct {
	char *name;
	char **value;
} strtable [] = {
	"bheadline",            &BHEADLINE,
	"bmailcmd",             &BMAILCMD,
	"dbzname",              &DBZNAME,
	"fheadline",            &FHEADLINE,
	"fmailcmd",             &FMAILCMD,
	"headline",             &HEADLINE,
	"inewsname",            &INEWSNAME,
	"log",                  &LOGFILE,
	"mailbox",              &MAILBOX,
	"mailcmd",              &MAILCMD,
	"myaddress",            &MYADDRESS,
	"mydomain",             &MYDOMAIN,
	"myservaddr",           &MYSERVADDR,
	"newslibdir",           &NEWSLIBDIR,
	"newsspooldir",         &NEWSSPOOLDIR,
	"servdir",              &SERVDIR,

	0,                      0,
};

# endif /* INITCONFIG */
