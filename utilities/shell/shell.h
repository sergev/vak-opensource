struct _node {
	int type;                               /* binary/files/string */
	union {
		struct {                        /* binary */
			struct _node *left;     /* left subtree */
			struct _node *right;    /* right subtree */
			int op;                 /* op code */
		} n;
		struct {                        /* in/out files */
			struct _node *left;     /* input file */
			char *infile;           /* input file */
			char *outfile;          /* output file */
			int flags;              /* flags */
		} p;
		struct {                        /* command argument */
			char *str;              /* character string */
			int flags;              /* flags */
		} a;
	} u;
};

typedef struct _node Shell;

extern Shell *ShellParse ();
