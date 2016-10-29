struct cmdtab {
	char *cmdname;          /* command name */
	int (*cmdfunc) ();      /* function to execute */
};

struct lot {
	char *host;
	char *name;
	long t1;
	long t2;
	char *info;
};

struct glistentry {
	struct glistentry *next;
	char *name;
	long t0;
};

struct glist {
	int changed;
	struct glistentry *list;
};

extern struct glist *loadglist ();
extern struct lot *loadlot ();
