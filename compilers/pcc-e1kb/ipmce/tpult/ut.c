# ifdef MSDOS
# include <io.h>
# include <stdlib.h>
# else
extern char *malloc (), *realloc ();
# endif
# include <stdio.h>
# include <string.h>
# include "scr.h"
# include "ut.h"

extern void doabort ();

char *make_base_name (name)
register char * name;
{
	register char c;
	register int  i = strlen (name);
	if (i <= 0)
		return NULL;

	while ( --i >= 0 && (c = name[i]) != '/' &&
# ifdef MSDOS
		(c != '\\') &&
		(c != ':' )
# endif
	      )
	      ;

	return &name[++i];
}
char *extension (name, ext)
register char *name, *ext;
{
	if (strlen (name) == 0)
		return NULL;
	if (strrchr (name, '.') != NULL)
		return name;
	if (strlen (ext) == 0)
		return name;
	strcat (name, ".");
	return strcat (name, ext);
}

/* VARARGS 1 */
void outerr (s, p)
char *s, *p;
{
	char buf [80];

	sprintf (buf, s, p);
	write (2, buf, (unsigned) strlen (buf));
}

char *mallo (n)
{
	register char *p;

	if (! (p = malloc ((unsigned) n)))
		doabort("malloc : out of memory\n");
	return (p);
}

char *reallo (p, n)
register char *p;
{
	if (! (p = realloc (p, (unsigned) n)))
		doabort("malloc : out of memory\n");
	return (p);
}

/* VARARGS 1 */
/* ARGSUSED */

void mfree (p, n)
register char *p;
{
	free (p);
}

void hidecursor ()
{
	VMove (LINES-1, COLS-2);
}

/* VARARGS1 */

void Vprintf (fmt, a,b,c,d,e,f,g,h)
char *fmt;
{
	char buf [512];

	sprintf (buf, fmt, a,b,c,d,e,f,g,h);
	VPutString (buf);
}

# ifdef MSDOS
unsigned char tkoi8[128] =
"áâ÷çäåöúéêëìíîïðòóôõæèãþûýßùøüàñÁÂ×ÇÄÅÖÚÉÊËÌÍÎÏÐ\
\x20                                               \
ÒÓÔÕÆÈÃÞÛÝßÙØÜÀÑåÅ              ";

unsigned char talt [128] =
"                                \
\x20                               \
î ¡æ¤¥ä£å¨©ª«¬­®¯ïàáâã¦¢ìë§èíéçê\
ž€–„…”ƒ•ˆ‰Š‹ŒŽŸ‘’“†‚œ›‡˜™—š";
# endif

unsigned char tokoi8 (c)
unsigned char c;
{
# ifdef MSDOS
	return (c > (unsigned char) '\200') ? tkoi8[c - 0200] : c;
# else
	return c;
# endif
}

unsigned char toalt (c)
unsigned char c;
{
# ifdef MSDOS
	return (c > (unsigned char) '\200') ? talt [c - 0200] : c;
# else
	return c;
# endif
}

