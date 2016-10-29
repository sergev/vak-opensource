/*
 * Протокол взаимодействия с базой данных сервера телеконференций.
 *
 * Copyright (C) 1991 Сергей Вакуленко, <vak@kiae.su>.
 * All rights reserved.
 */

typedef string stringp <>;
typedef long longvect <>;
typedef int intvect <>;

struct subscrrec {
	long    tag;
	short   mode;
};

typedef subscrrec subscrvect <>;

struct statrec {
	int subs;
	int feed;
	int rfeed;
};

typedef statrec statvect <>;

struct addgroupparms {
	string  name <>;
	long    last;
};

struct subscrparms {
	long    user;
	long    group;
	int     mode;
};

struct unsubscrparms {
	long    user;
	long    group;
};

struct usersubscrparms {
	long    user;
	stringp gtab <>;
};

struct setuflagparms {
	long    tag;
	long    flags;
};

struct setsubscrparms {
	long    group;
	subscrvect tab;
};

struct setglastparms {
	long tag;
	long last;
};

program RGROUPSPROG {
	version RGROUPSVERS {
		void            DIE (void)                      = 1;
		void            RESTART (void)                  = 2;

		int             LOADGROUPS (int)                = 11;
		int             SAVEGROUPS (void)               = 12;
		long            GROUPTAG (string)               = 13;
		long            USERTAG (string)                = 14;
		long            ADDUSER (string)                = 15;
		int             DELUSER (string)                = 16;
		intvect         DELUSERTAB (longvect)           = 17;
		subscrvect      GROUPSUBSCR (long)              = 18;
		subscrvect      USERSUBSCR (usersubscrparms)    = 19;
		long            ADDGROUP (addgroupparms)        = 20;
		int             DELGROUP (string)               = 21;
		int             SUBSCRIBE (subscrparms)         = 22;
		int             UNSUBSCRIBE (unsubscrparms)     = 23;
		intvect         UNSUBSCRTAB (longvect)          = 24;
		int             UNSUBSCRALL (long)              = 25;
		long            USERFLAGS (long)                = 26;
		long            GROUPLAST (long)                = 27;
		string          GROUPNAME (long)                = 28;
		string          USERNAME (long)                 = 29;
		void            GROUPSLIMIT (int)               = 30;
		void            GROUPSSYNC (void)               = 31;
		longvect        USERLIST (void)                 = 32;
		longvect        GROUPLIST (void)                = 33;
		void            SETUSERFLAGS (setuflagparms)    = 34;
		void            SETSUBSCR (setsubscrparms)      = 35;
		void            SETGROUPLAST (setglastparms)    = 36;
		statrec         GROUPSTAT (long)                = 37;
		statvect        USERSTAT (longvect)             = 38;
	} = 1;
} = 0x2baf0001;
