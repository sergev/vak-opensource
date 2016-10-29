struct cmdtab {
	char *cmdname;                  /* command name */
	void (*cmdfunc) ();             /* function to execute */
};

void error (char *s, ...);              /* log error message */
void message (char *s, ...);            /* print error message */
void fatal (char *s, ...);              /* print fatal error message */
int run (char *s, ...);                 /* run command */
int runs (char *cmd);                   /* run shell command */
char *getsendername (char *field);      /* get sender address */
void replyto (FILE *from, char *name);
char *strip (char *wd);
void doquit (void);
void mainloop (void);
void exitloop (void);
