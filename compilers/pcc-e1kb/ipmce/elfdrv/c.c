/*$Header: c.c,v 1.4 91/01/26 20:48:36 drun Exp $ */

/*$Log:	c.c,v $
 * Revision 1.4  91/01/26  20:48:36  drun
 * Пepeвoд ядpa нa includ'ы из /usr/include/sys.
 * 
 * Revision 1.3  91/01/13  22:03:21  mike
 * Изменения, связанные с введением поля d_nqueue в 
 * структуру bdevsw. Вычищен всякий мусор типа SCSI.
 * 
 * Revision 1.2  90/10/23  11:32:39  drun
 * Bвeдeнo пoнятиe кoнтpoллepa ycтpoйcтвa. Koличecтвo кoнтpoллepoв пo типaм
 * зaдaeтcя в фaйлe devconf.h
 * 
 * Revision 1.1  90/08/14  16:03:18  root
 * Initial revision
 *  */

#include <sys/param.h>
#include <sys/devconf.h>
#include <sys/systm.h>
#include <sys/buf.h>
#include <sys/listc.h>
#include <sys/conf.h>
#include <sys/proc.h>
#include <sys/text.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/file.h>
#include <sys/inode.h>
#include <sys/acct.h>
#include <sys/map.h>
#include <sys/filsys.h>
#include <sys/mount.h>
#include <sys/callo.h>

extern  int     nulldev();
extern  int     nodev();

int tmopen(), tmclose(), tmstrategy(),tmioctl();
int rpread(), rpwrite(), rpopen(), rpclose(), rpstrategy(),rpioctl();
int bdread(), bdwrite(), bdopen(), bdclose(), bdstrategy();
int rpbufstrat();
int elopen(), elclose(), elstrategy();
int demopen(), demclose();
extern struct buf teutab[], eltab[], rptab[];

int rpstrategy();

/*      RUBIN lock driver */
#include <sys/rubin.h>

struct	bdevsw	bdevsw[] =
{
	demopen, demclose, rpstrategy, rptab, NDEMQ   ,/*  0 для гп дeмoca  */
	tmopen, tmclose, tmstrategy, teutab,  NTMQ,    /*  1 = tm */
	rpopen, rpclose, rpstrategy, rptab,   NRPQ,    /*  2 = rp */
	elopen, elclose, elstrategy, eltab,   NELQ,    /*  3 = el */
	rpopen, rpclose, rpbufstrat, rptab,   NRPBUFQ, /*  4 = rpbuf */
	bdopen, bdclose, bdstrategy, bdtab,   NBDQ,    /*  5 = bd */
	0
};

int     nblkdev = 0;
int     elmajor = 3;

/*      Utec RS-232 driver      */
/* int     rsopen(), rsclose(), rsread(), rswrite(), rsioctl();
extern  struct  tty     rs[];
*/
int adopen(), adclose(), adread(), adwrite(), adioctl();
int tubopen(),tubclose(),tubread(),tubwrite();
int mpdopen(),mpdclose(),mpdread(),mpdwrite(), mpdioctl();
int klopen(), klclose(), klread(), klwrite(), klioctl();
int lpopen(), lpclose(), lpwrite(), lpioctl();
int mpopen(), mpclose(), mpwrite(), mpioctl();
int tmread(), tmwrite();
int rootopen(),rootclose(), rootread(), rootwrite();
/*      Memory access (pseudo-device)   */
int	mmread(), mmwrite();
int     elread(), elwrite();

/*      Process control terminal (pseudo-device) */
int     syopen(), syread(), sywrite(), syioctl();

/*  # define tmread  nulldev  */
/*  # define tmwrite nulldev   */

# define klopen  nulldev
# define klclose nulldev
# define klread  nulldev
# define klwrite nulldev
# define klioctl nulldev

struct	cdevsw	cdevsw[] =
{
adopen,  adclose, adread,  adwrite, adioctl, nulldev,    0,     /*  0 = console */
nulldev, nulldev, nulldev, nulldev, nulldev, nulldev,    0,     /*  1 = null*/
nulldev, nulldev, mmread,  mmwrite, nodev,   nulldev,    0,     /*  2 = mem */
syopen,  nulldev, syread,  sywrite, syioctl, nulldev,    0,     /*  3 = tty */
adopen,  adclose, adread,  adwrite, adioctl, nulldev,    0,     /*  4 = ad */
lpopen,  lpclose, nodev,   lpwrite, lpioctl, nulldev,    0,     /*  5 = lp */
nodev,   nodev,   nodev,   nodev,   nodev,   nulldev,    0,     /*  6 = rubin */
tubopen, tubclose,tubread, tubwrite,adioctl, nulldev,    0,     /*  7 = tube */
mpopen,  mpclose, nodev,   mpwrite, mpioctl, nulldev,    0,     /*  8 = mp */
elopen,  elclose, elread,  elwrite, nodev,   nulldev,    0,     /*  9 = el   */
tmopen,  tmclose, tmread,  tmwrite, tmioctl, nulldev,    0,     /* 10 = tm */
rpopen,  rpclose, rpread,  rpwrite, nodev,   nulldev,    0,     /* 11 = rp */
demopen, demclose,rpread,  rpwrite, nodev,   nulldev,    0,     /* 12 = гп */
mpdopen, mpdclose,mpdread, mpdwrite,mpdioctl,nulldev,    0,     /* 13 = mpd */
bdopen,  bdclose, bdread,  bdwrite, nodev,   nulldev,    0,     /* 14 = bd */
nodev,   nodev,   nodev,   nodev,   nodev,   nulldev,    0,	/* 15 = rx */
nodev,   nodev,   nodev,   nodev,   nodev,   nulldev,    0,     /* 16 = root */
0
};

int	nchrdev = sizeof(cdevsw) / sizeof(cdevsw[0]);

dev_t   bootdev = makedev(2,070);
dev_t   pipedev ;

struct  proc            proc[NPROC];
struct  text            text[NTEXT];
struct  file            file[NFILE];
struct  inode           inode[NINODE];
struct  mount           mount[NMOUNT];
struct  map             coremap[CMAPSIZ];
struct  map             swapmap[SMAPSIZ];
struct  callo           callout[NCALL];
struct  cblock          *cfree;
struct  cblock          *cfreelist;
int	cfreecount;

/* struct  fichier         fich_tab[NBFICH];      */

struct  buf             buf[NBUF];
struct  buf             bfreelist;

struct  acct            acctbuf;
struct  inode           *acctp;
