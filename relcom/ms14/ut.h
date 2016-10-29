/*
 * Utility functions.
 * Copyright (C) 1990-95 Serge Vakulenko
 */
FILE *f2open (char *dirname, char *filename, char *mode);
char *strcopy (char *str);
char *strlower (char *str);
int checkpermissions (char *username, char *dirname, char *permfile);
int f2access (char *dirname, char *filename, int mode);
int f2cat (char *dir, char *file);
int fcat (char *name, int maxlen);
int ffcopy (FILE *from, FILE *to);
int ffncopy (FILE *from, FILE *to, long len);
int fgetinfo (char *info, int maxlen, FILE *fd);
int getstr (FILE *fd, char *s);
int isdir (char *name);
int locked (char *name);
int lower (int c);
int strnccmp (char *str, char *pat, int len);
int textfile (char *name);
long filesize (char *name);
void f2clean (char *dirname, char *filename);
void fputse (char *p, FILE *fd);
void lock (char *name);
void mkinfoname (char *name, char *buf);
void sigcatch (void);
void sigign (void);
void unlock (char *name);
