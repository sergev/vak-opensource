/*	@(#)getut.c	1.3	*/

/*	Routines to read and write the /etc/utmp file.			*/
/*									*/
#include	<sys/param.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<utmp.h>
#include	<errno.h>
#include	<fcntl.h>

#define	MAXFILE	79	/* Maximum pathname length for "utmp" file */

#ifdef	DEBUG
#undef	UTMP_FILE
#define	UTMP_FILE "utmp"
#endif

static int fd = -1;	/* File descriptor for the utmp file. */
static char utmpfile[MAXFILE+1] = UTMP_FILE;	/* Name of the current
						 * "utmp" like file.
						 */
static long loc_utmp;	/* Where in "utmp" the current "ubuf" was
			 * found.
			 */
static struct utmp ubuf;	/* Copy of last entry read in. */


/* "getutent" gets the next entry in the utmp file.
 */

struct utmp *getutent()
{
	extern int fd;
	extern char utmpfile[];
	extern struct utmp ubuf;
	extern long loc_utmp,lseek();
	extern int errno;
	register char *u;
	register int i;
	struct stat stbuf;

/* If the "utmp" file is not open, attempt to open it for
 * reading.  If there is no file, attempt to create one.  If
 * both attempts fail, return NULL.  If the file exists, but
 * isn't readable and writeable, do not attempt to create.
 */

	if (fd < 0) {

/* Make sure file is a multiple of 'utmp' entries long */
		if (stat(utmpfile,&stbuf) == 0) {
			if((stbuf.st_size % sizeof(struct utmp)) != 0) {
				unlink(utmpfile);
			}
		}
		if ((fd = open(utmpfile, O_RDWR|O_CREAT, 0644)) < 0) {

/* If the open failed for permissions, try opening it only for
 * reading.  All "pututline()" later will fail the writes.
 */
			if (errno == EACCES
			    && (fd = open(utmpfile, O_RDONLY)) < 0)
				return(NULL);
		}
	}

/* Try to read in the next entry from the utmp file.  */
	if (read(fd,&ubuf,sizeof(ubuf)) != sizeof(ubuf)) {

/* Make sure ubuf is zeroed. */
		for (i=0,u=(char *)(&ubuf); i<sizeof(ubuf); i++) *u++ = '\0';
		loc_utmp = 0;
		return(NULL);
	}

/* Save the location in the file where this entry was found. */
	loc_utmp = lseek(fd,0L,1) - (long)(sizeof(struct utmp));
	return(&ubuf);
}

/*	"getutid" finds the specified entry in the utmp file.  If	*/
/*	it can't find it, it returns NULL.				*/

struct utmp *getutid(entry)
register struct utmp *entry;
{
	extern struct utmp ubuf;
	struct utmp *getutent();
	register short type;

/* Start looking for entry.  Look in our current buffer before */
/* reading in new entries. */
	do {

/* If there is no entry in "ubuf", skip to the read. */
		if (ubuf.ut_type != EMPTY) {
			switch(entry->ut_type) {

/* Do not look for an entry if the user sent us an EMPTY entry. */
			case EMPTY:
				return(NULL);

/* For RUN_LVL, BOOT_TIME, OLD_TIME, and NEW_TIME entries, only */
/* the types have to match.  If they do, return the address of */
/* internal buffer. */
			case RUN_LVL:
			case BOOT_TIME:
			case OLD_TIME:
			case NEW_TIME:
				if (entry->ut_type == ubuf.ut_type) return(&ubuf);
				break;

/* For INIT_PROCESS, LOGIN_PROCESS, USER_PROCESS, and DEAD_PROCESS */
/* the type of the entry in "ubuf", must be one of the above and */
/* id's must match. */
			case INIT_PROCESS:
			case LOGIN_PROCESS:
			case USER_PROCESS:
			case DEAD_PROCESS:
				if (((type = ubuf.ut_type) == INIT_PROCESS
					|| type == LOGIN_PROCESS
					|| type == USER_PROCESS
					|| type == DEAD_PROCESS)
				    && ubuf.ut_id[0] == entry->ut_id[0]
				    && ubuf.ut_id[1] == entry->ut_id[1]
				    && ubuf.ut_id[2] == entry->ut_id[2]
				    && ubuf.ut_id[3] == entry->ut_id[3])
					return(&ubuf);
				break;

/* Do not search for illegal types of entry. */
			default:
				return(NULL);
			}
		}
	} while (getutent() != NULL);

/* Return NULL since the proper entry wasn't found. */
	return(NULL);
}

/* "getutline" searches the "utmp" file for a LOGIN_PROCESS or
 * USER_PROCESS with the same "line" as the specified "entry".
 */

struct utmp *getutline(entry)
register struct utmp *entry;
{
	extern struct utmp ubuf,*getutent();
	register struct utmp *cur;

/* Start by using the entry currently incore.  This prevents */
/* doing reads that aren't necessary. */
	cur = &ubuf;
	do {
/* If the current entry is the one we are interested in, return */
/* a pointer to it. */
		if (cur->ut_type != EMPTY && (cur->ut_type == LOGIN_PROCESS
		    || cur->ut_type == USER_PROCESS) && strncmp(&entry->ut_line[0],
		    &cur->ut_line[0],sizeof(cur->ut_line)) == 0) return(cur);
	} while ((cur = getutent()) != NULL);

/* Since entry wasn't found, return NULL. */
	return(NULL);
}

/*	"pututline" writes the structure sent into the utmp file.	*/
/*	If there is already an entry with the same id, then it is	*/
/*	overwritten, otherwise a new entry is made at the end of the	*/
/*	utmp file.							*/

struct utmp *pututline(entry)
struct utmp *entry;
{
	int fc;
	struct utmp *answer;
	extern long time();
	extern struct utmp ubuf;
	extern long loc_utmp,lseek();
	extern struct utmp *getutid();
	extern int fd,errno;
	struct utmp tmpbuf;

/* Copy the user supplied entry into our temporary buffer to */
/* avoid the possibility that the user is actually passing us */
/* the address of "ubuf". */
	tmpbuf = *entry;
	getutent();
	if (fd < 0) {
#ifdef	ERRDEBUG
		gdebug("pututline: Unable to create utmp file.\n");
#endif
		return((struct utmp *)NULL);
	}
/* Make sure file is writable */
	if ((fc=fcntl(fd, F_GETFL, NULL)) == -1
	    || (fc & O_RDWR) != O_RDWR) {
		return((struct utmp *)NULL);
	}

/* Find the proper entry in the utmp file.  Start at the current */
/* location.  If it isn't found from here to the end of the */
/* file, then reset to the beginning of the file and try again. */
/* If it still isn't found, then write a new entry at the end of */
/* the file.  (Making sure the location is an integral number of */
/* utmp structures into the file incase the file is scribbled.) */

	if (getutid(&tmpbuf) == NULL) {
#ifdef	ERRDEBUG
		gdebug("First getutid() failed.  fd: %d",fd);
#endif
		setutent();
		if (getutid(&tmpbuf) == NULL) {
#ifdef	ERRDEBUG
			loc_utmp = lseek(fd, 0L, 1);
			gdebug("Second getutid() failed.  fd: %d loc_utmp: %ld\n",fd,loc_utmp);
#endif
			fcntl(fd, F_SETFL, fc | O_APPEND);
		} else {
			lseek(fd, -(long)sizeof(struct utmp), 1);
		}
	} else {
		lseek(fd, -(long)sizeof(struct utmp), 1);
	}

/* Write out the user supplied structure.  If the write fails, */
/* then the user probably doesn't have permission to write the */
/* utmp file. */
	if (write(fd,&tmpbuf,sizeof(tmpbuf)) != sizeof(tmpbuf)) {
#ifdef	ERRDEBUG
		gdebug("pututline failed: write-%d\n",errno);
#endif
		answer = (struct utmp *)NULL;
	} else {
/* Copy the user structure into ubuf so that it will be up to */
/* date in the future. */
		ubuf = tmpbuf;
		answer = &ubuf;

#ifdef	ERRDEBUG
		gdebug("id: %c%c loc: %x\n",ubuf.ut_id[0],ubuf.ut_id[1],
		    ubuf.ut_id[2],ubuf.ut_id[3],loc_utmp);
#endif
	}
	fcntl(fd, F_SETFL, fc);
	return(answer);
}

/*	"setutent" just resets the utmp file back to the beginning.	*/

setutent()
{
	register char *ptr;
	register int i;
	extern int fd;
	extern struct utmp ubuf;
	extern long loc_utmp;

	if (fd != -1) lseek(fd,0L,0);

/* Zero the stored copy of the last entry read, since we are */
/* resetting to the beginning of the file. */

	for (i=0,ptr=(char*)&ubuf; i < sizeof(ubuf);i++) *ptr++ = '\0';
	loc_utmp = 0L;
}

/*	"endutent" closes the utmp file.				*/

endutent()
{
	extern int fd;
	extern long loc_utmp;
	extern struct utmp ubuf;
	register char *ptr;
	register int i;

	if (fd != -1) close(fd);
	fd = -1;
	loc_utmp = 0;
	for (i=0,ptr= (char *)(&ubuf); i < sizeof(ubuf);i++) *ptr++ = '\0';
}

/*	"utmpname" allows the user to read a file other than the	*/
/*	normal "utmp" file.						*/

utmpname(newfile)
char *newfile;
{
	extern char utmpfile[];

/* Determine if the new filename will fit.  If not, return 0. */
	if (strlen(newfile) > MAXFILE) return (0);

/* Otherwise copy in the new file name. */
	else strcpy(&utmpfile[0],newfile);

/* Make sure everything is reset to the beginning state. */
	endutent();
	return(1);
}

#ifdef	ERRDEBUG
#include	<stdio.h>

gdebug(format,arg1,arg2,arg3,arg4,arg5,arg6)
char *format;
int arg1,arg2,arg3,arg4,arg5,arg6;
{
	register FILE *fp;
	register int errnum;
	extern int errno;

	if ((fp = fopen("/etc/dbg.getut","a+")) == NULL) return;
	fprintf(fp,format,arg1,arg2,arg3,arg4,arg5,arg6);
	fclose(fp);
}
#endif
