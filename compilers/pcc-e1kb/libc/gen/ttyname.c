/*	@(#)ttyname.c	1.2	*/
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
/*
 * ttyname(f): return "/dev/ttyXX" which the the name of the
 * tty belonging to file f.
 *
 * This program works in two passes: the first pass tries to
 * find the device by matching device and inode numbers; if
 * that doesn't work, it tries a second time, this time doing a
 * stat on every file in /dev and trying to match device numbers
 * only. If that fails too, NULL is returned.
 */

#define	NULL	0
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>

extern int open(), read(), close(), stat(), fstat(), isatty();
extern char *strcpy(), *strcat();
extern long lseek();

static char rbuf[32], dev[]="/dev/";

char *
ttyname(f)
int	f;
{
	struct stat fsb, tsb;
	struct direct db;
	register int df, pass1;

	if(isatty(f) == 0)
		return(NULL);
	if(fstat(f, &fsb) < 0)
		return(NULL);
	if((fsb.st_mode & S_IFMT) != S_IFCHR)
		return(NULL);
	if((df = open(dev, 0)) < 0)
		return(NULL);
	pass1 = 1;
	do {
		while(read(df, (char *)&db, sizeof(db)) == sizeof(db)) {
			if(db.d_ino == 0)
				continue;
			if(pass1 && db.d_ino != fsb.st_ino)
				continue;
			(void) strcpy(rbuf, dev);
			(void) strcat(rbuf, db.d_name);
			if(stat(rbuf, &tsb) < 0)
				continue;
			if(tsb.st_rdev == fsb.st_rdev &&
				(tsb.st_mode&S_IFMT) == S_IFCHR &&
				(!pass1 || tsb.st_ino == fsb.st_ino)) {
				(void) close(df);
				return(rbuf);
			}
		}
		(void) lseek(df, 0L, 0);
	} while(pass1--);
	(void) close(df);
	return(NULL);
}
