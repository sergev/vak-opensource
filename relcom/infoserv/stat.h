/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	Copyright (c) 1987, 1988 Microsoft Corporation	*/
/*	  All Rights Reserved	*/

/*	This Module contains Proprietary Information of Microsoft  */
/*	Corporation and should be treated as Confidential.	   */


#if !defined(_H_STAT)
#define _H_STAT
#ident	"@(#)stat.h	2.14 - 89/07/16"

#include <sys/types.h>
/*
 * stat structure, used by stat(2) and fstat(2)
 */
#if defined(_POSIX_SOURCE)
struct	stat {
	dev_t	st_dev;
	ino_t	st_ino;
	mode_t 	st_mode;
	nlink_t	st_nlink;
	uid_t 	st_uid;
	gid_t 	st_gid;
	dev_t	st_rdev;
	off_t	st_size;
	time_t	st_atime;
	time_t	st_mtime;
	time_t	st_ctime;
};
#else
struct	stat {
	dev_t	st_dev;
	ushort	st_ino;
	ushort 	st_mode;
	short  	st_nlink;
	ushort 	st_uid;
	ushort 	st_gid;
	dev_t	st_rdev;
	off_t	st_size;
	time_t	st_atime;
	time_t	st_mtime;
	time_t	st_ctime;
};
#endif

#if defined(_POSIX_SOURCE)
#define S_ISDIR(m)	(((m) & 0170000) == 0040000)
#define S_ISCHR(m)	(((m) & 0170000) == 0020000)
#define S_ISBLK(m)	(((m) & 0170000) == 0060000)
#define S_ISREG(m)	(((m) & 0170000) == 0100000)
#define S_ISFIFO(m)	(((m) & 0170000) == 0010000)
#else
#define	S_IFMT	0170000		/* type of file */
#define		S_IFDIR	0040000	/* directory */
#define		S_IFCHR	0020000	/* character special */
#define		S_IFBLK	0060000	/* block special */
#define		S_IFREG	0100000	/* regular */
#define		S_IFIFO	0010000	/* fifo */
#define		S_IFNAM 0050000 /* special named file */
#endif

#define	S_IRWXU	00700		/* read, write, execute: owner */
#define	S_IRUSR	00400		/* read permission: owner */
#define	S_IWUSR	00200		/* write permission: owner */
#define	S_IXUSR	00100		/* execute permission: owner */
#define	S_IRWXG	00070		/* read, write, execute: group */
#define	S_IRGRP	00040		/* read permission: group */
#define	S_IWGRP	00020		/* write permission: group */
#define	S_IXGRP	00010		/* execute permission: group */
#define	S_IRWXO	00007		/* read, write, execute: other */
#define	S_IROTH	00004		/* read permission: other */
#define	S_IWOTH	00002		/* write permission: other */
#define	S_IXOTH	00001		/* execute permission: other */
#define	S_ISUID	04000		/* set user id on execution */
#define	S_ISGID	02000		/* set group id on execution */

#if !defined(_POSIX_SOURCE)
#define S_INSEM 01		/* XENIX semaphore subtype of IFNAM file */
#define S_INSHD 02		/* XENIX shared data subtype of IFNAM file */
#define	S_ISVTX	01000		/* save swapped text even after use */
#define	S_IREAD	00400		/* read permission, owner */
#define	S_IWRITE	00200	/* write permission, owner */
#define	S_IEXEC	00100		/* execute/search permission, owner */
#define	S_ENFMT	S_ISGID		/* record locking enforcement flag */
#endif

#if defined(__STDC__)
extern int    chmod(char const *, int);
extern int    fstat(int, struct stat *);
extern int    mkdir(char const *, int);
extern int    mkfifo(char const *, int);
extern int    stat(char const *, struct stat *);
#if defined(_POSIX_SOURCE)
extern mode_t umask(mode_t);
#else
extern int    umask();
#endif
#else
extern int    chmod();
extern int    fstat();
extern int    mkdir();
extern int    mkfifo();
extern int    stat();
#if defined(_POSIX_SOURCE)
extern mode_t umask();
#else
extern int    umask();
#endif
#endif
#endif
