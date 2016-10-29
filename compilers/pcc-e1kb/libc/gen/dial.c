/*      @(#)dial.c	1.16     */
/*LINTLIBRARY*/
/***************************************************************
 *      dial() returns an fd for an open tty-line connected to the
 *      specified remote.  The caller should trap all ways to
 *      terminate, and call undial(). This will release the `lock'
 *      file and return the outgoing line to the system.  This routine
 *      would prefer that the calling routine not use the `alarm()'
 *      system call, nor issue a `signal(SIGALRM, xxx)' call.
 *      If you must, then please save and restore the alarm times.
 *      The sleep() library routine is ok, though.
 *
 *	#include <sys/types.h>
 *	#include <sys/stat.h>
 *      #include "dial.h"
 *
 *      int dial(call);
 *      CALL call;
 *
 *      void undial(rlfd);
 *      int rlfd;
 *
 *      rlfd is the "remote-lne file descriptor" returned from dial.
 *
 *      The CALL structure as (defined in dial.h):
 *
 *      typedef struct {
 *              struct termio *attr;    ptr to term attribute structure
 *              int     baud;           transmission baud-rate
 *              int     speed;          212A modem: low=300, high=1200
 *              char    *line;          device name for out-going line
 *              char    *telno;         ptr to tel-no digit string
 *		char 	*device		Will hold the name of the device
 *					used to makes a connection.
 *		int	dev_len		This is the length of the device
 *					used to makes a connection.
 *      } CALL;
 *
 *      The error returns from dial are negative, in the range -1
 *      to -12, and their meanings are:
 *
 *              INTRPT   -1: interrupt occured
 *              D_HUNG   -2: dialer hung (no return from write)
 *              NO_ANS   -3: no answer within 20 seconds
 *              ILL_BD   -4: illegal baud-rate
 *              A_PROB   -5: acu problem (open() failure)
 *              L_PROB   -6: line problem (open() failure)
 *              NO_Ldv   -7: can't open L-devs file
 *              DV_NT_A  -8: specified device not available
 *              DV_NT_K  -9: specified device not known
 *              NO_BD_A -10: no device available at requested baud-rate
 *              NO_BD_K -11: no device known at requested baud-rate
 *		DV_NT_E -12: requested speed does not match
 *
 *      Setting attributes in the termio structure indicated in
 *      the `attr' field of the CALL structure before passing the
 *      structure to dial(), will cause those attributes to be set
 *      before the connection is made.  This can be important for
 *      some attributes such as parity and baud.
 *
 *      As a device-lockout semaphore mechanism, we create an entry,
 *      in the directory #defined as LOCK, whose name is LCK..dev
 *      where dev is the device name taken from the "line" column
 *      in the file #defined as LDEVS.  Be sure to trap every possible
 *      way out of execution in order to "release" the device.
 *      This entry is `touched' every hour in order to keep uucp
 *      from removing it on its 90 minute rounds.
 *      Also, have the system start-up procedure clean all such
 *      entries from the LOCK directory.
 *
 *      With an error return (negative value), there will not be
 *      any `lock-file' entry, so no need to call undial().
 ***************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <termio.h>
#include <errno.h>
#include "dial.h"

#define DEV	"/dev/"
#define UPDTE   3600            /* how often to touch the lock entry */
#define ACULAST "<"             /* character which terminates dialing*/
#define YES     1               /* mnemonic */
#define NO      0               /* mnemonic */
#define DIFFER  strcmp          /* mnemonic */
#define say     (void)fprintf   /* mnemonic */

extern unsigned
        sleep(),
        alarm();

extern int errno;

extern char *malloc();

static char
        cul[15+sizeof(DEVDIR)] = DEV, /* line's device-name */
        cua[15+sizeof(DEVDIR)] = DEV, /* acu's device-name */
        *find_dev(),            /* local function */
        lock[16+sizeof(LOCK)];  /* directory in which to make lockfile*/

static int
	sperfg=0,		/* requested speed not available */
        found=0,                /* set when device is seen legal */
        saverr,                 /* hide errno during other calls */
        rlfd,                   /* fd for remote comm line */
        lfd= -1,                /* fd for the device-lock file */
        intflag=NO,             /* interrupt indicator */
        connect(),              /* local function */
        intcatch(),             /* interrupt routine */
        alrmcatch(),            /* interrupt routine */
        hupcatch();             /* interrupt routine */

#ifdef ddt
static void dump();
#endif

char device[34];
int     _debug=0;
void    undial();

int
dial(call)
CALL call;
{
        FILE *Ldevices;         /* file pointer for Device name file */
        char dvc[30];
        int (*savint)(), (*savhup)();

#ifdef ddt
        if(_debug == YES) {
                say(stderr, "call dial(%d)\r\n", call);
                dump(&call, 0);
        }
#endif

        saverr = 0;
        savint = signal(SIGINT, intcatch);
        savhup = signal(SIGHUP, hupcatch);
        (void)signal(SIGALRM, alrmcatch);

        if(call.telno == NULL && call.line == NULL) {
                	rlfd = DV_NT_K;
                	goto OUT;
        }

        if((Ldevices = fopen(LDEVS, "r")) == NULL) {
                saverr = errno;
                rlfd = NO_Ldv;
                goto OUT;
        }

        while(1) {
                int xx;

                (void)strcpy(dvc, find_dev(Ldevices, &call));
                if(strlen(dvc) == 0) 
                        goto F1;        /* failure to find device */
                (void)strcpy(lock, LOCK);
                (void)strcat(lock, dvc);

                /* creat will always succeed if usr-id is 0,
                   so  check that case separately */
                if(geteuid() == 0  && access(lock, 0) == 0)  
                        goto F0;        /* device found but busy */ 
                if((lfd = creat(lock, 0444)) < 0) 
                        goto F0;        /* device found but busy */ 

                xx = getpid();
                (void)write(lfd, (char*)&xx, sizeof(xx));
		break; /* we have a device get out of here */
        F0:
                if(!call.line)            /* dial device is busy */
                        continue;       /* try to find another */

        F1:
                if(call.line)
                        if(found)       /* specific device request */
                                rlfd = DV_NT_A;
                        else if(sperfg == 1)  
				rlfd = DV_NT_E;
			else
                                rlfd = DV_NT_K;
		else
                        if(found)        /* we are dialing */
                                rlfd = NO_BD_A;
                        else
                                rlfd = NO_BD_K;
                goto CLOUT;
        }
        if(intflag == YES)
                rlfd = INTRPT;
        else
                if((rlfd = connect(&call)) < 0) 
                        undial(rlfd);
                else
                        (void)alarm(UPDTE);
CLOUT:
        (void)fclose(Ldevices);
OUT:
        (void)signal(SIGINT, savint);
        (void)signal(SIGHUP, savhup);
        errno = saverr;
        return(rlfd);
}

/***************************************************************
 *      connect: establish dial-out or direct connection.
 *      Negative values returned (-1...-7) are error message indices.
 ***************************************************************/
static int
connect(call)
CALL *call;
{
        struct termio *lvp, lv;
        unsigned u;
        int er=0, dum, fdac, fd=0, t, w, x;
        char *p, sp_code, b[30];

#ifdef ddt
        if(_debug == YES) {
                say(stderr, "call connect(%o)\n", call);
                dump(call, 0);
        }
#endif

        switch(call->baud) {
                case 110:
                        sp_code = (B110 | CSTOPB);
                        break;
                case 134:
                        sp_code = B134;
                        break;
                case 150:
                        sp_code = B150;
                        break;
                case 300:
                        sp_code = B300;
                        break;
                case 600:
                        sp_code = B600;
                        break;
                case 1200:
                        sp_code = B1200;
                        break;
                case 2400:
                        sp_code = B2400;
                        break;
                case 4800:
                        sp_code = B4800;
                        break;
                case 9600:
                        sp_code = B9600;
                        break;
                default:
                        er = ILL_BD;
                        goto RTN;
        }
        if((fd = open(cul, O_EXCL | O_RDWR | O_NDELAY)) < 0) {
                perror(cul);
                er = L_PROB;
                goto RTN;
        }
	if(call->device && call->dev_len !=0) {
		strncpy(call->device, cul, call->dev_len);
		if(strlen(cul) >= call->dev_len)
			call->device[call->dev_len -1] = '\0';
	}
        if(!call->attr)
                lvp = &lv;
        else
                lvp = call->attr;
        lvp->c_cflag |= (CREAD | HUPCL);
        if(!(lvp->c_cflag & CSIZE))
                lvp->c_cflag |= CS8;
        if( (call->telno == NULL ) && (call->modem) ) {
                lvp->c_cflag |= CLOCAL;

        } else
                lvp->c_cflag &= ~CLOCAL;
#ifdef  ddt
        if(_debug == YES) say(stderr,"value of cflag = %o\n\r", lvp->c_cflag);
#endif
        lvp->c_cflag &= ~CBAUD;
        lvp->c_cflag |= sp_code;
        if((t = ioctl(fd, TCSETA, lvp)) < 0) {
                perror("stty for remote");
                er = L_PROB;
                goto RTN;
        }
        if(call->telno) {
		(void)alarm(30);
                if((fdac = open(cua, O_WRONLY)) < 0) {
                        perror(cua);
                        er = A_PROB;
                        goto RTN;
                }
		alrmcatch();
                t = strlen(strcat(strcpy(b, call->telno), ACULAST));
#ifdef ddt
                if(_debug == YES)
                        say(stderr, "dialing %s\n", b);
#endif
                w = write(fdac, b, (unsigned)t); /* dial the number */
                x = errno;
		p = &b[t-2];
		for(; *p-- == '-'; t--);
                if(w < t) {
                        errno = x;
                        if(w == -1)
                                perror("write to acu");
                        else
                                say(stderr, "%s: Semaphore failure\n", cul);
                        er = (errno == EINTR)? D_HUNG: A_PROB;
			(void)close(fdac);
                        goto RTN;
                }
                (void)close(fdac);      /* dialing is complete */
#ifdef ddt
                if(_debug == YES)
                        say(stderr, "dialing complete\n");
#endif
        }
        (void)alarm(20);        /* should answer within 20 seconds */
        dum = open(cul, O_RDWR); /* wait here for carrier */
        x = errno;
	alrmcatch();
        if(dum < 0) {
                errno = x;
#ifdef ddt
                if(_debug == YES)
                        perror(cul);
#endif
                er = (errno == EINTR)? NO_ANS: L_PROB;
                goto RTN;
        }
        (void)close(dum);       /* the dummy open used for waiting*/
        (void)fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) & ~O_NDELAY);
RTN:
        if(intflag == YES)
                er = INTRPT;
#ifdef ddt
        if(_debug == YES)
                say(stderr, "connect ends with er=%d, fd=%d\n", er, fd);
#endif
	if(er) {
		close(fd);		
		return(er);
	} else
		return(fd);
}

/***************************************************************
 *      find_dev: find a device pair with the wanted characteristics
 *            specified in line and baud arguments.
 *      Return pointer to device name for use in lock-out semaphore.
 *      The variables 'cua' and 'cul' will be set to contain the
 *              complete path names of the corresponding devices.
 *      If the L-devices list contains a '0' entry because the
 *              line is direct, the variable 'cua' is set to '\0'.
 ***************************************************************/

static char*
find_dev(iop, call)
FILE *iop;
CALL *call;
{
        char buf[50], typ[4], temp[15], *b;
        int tspeed;

#ifdef ddt
        if(_debug == YES) {
                say(stderr, "call find_dev(%o)\n", call);
                dump(call, 0);
        }
#endif

        if(call->telno == NULL)
                (void)strcpy(typ, "DIR");
        else
                (void)strcpy(typ, "ACU");

        while(fgets(buf, 50, iop) != NULL) {

                if (strchr("# \t\n", buf[0]) != NULL)
                        continue;
                if(DIFFER(typ, strtok(buf, " \t")))
                        continue;

                (void)strcat(strcpy(cul,DEVDIR),strtok((char*)0," \t"));

                if(*(b = strtok((char*)0, " \t")) == '0')
                        cua[0] = '\0';
                else {
                        (void)strcat(strcpy(cua, DEVDIR), b);

                } tspeed = atoi(strtok((char*)0," \t\n"));

                if(call->line) {
                        if(strchr((b=call->line), '/') == 0) {
                                (void)strcpy(temp, DEVDIR);
                                b = strcat(temp, call->line);
                        }
                        if(DIFFER(b, cul))
                                continue;
			if(call->baud < 0) {
				/*found line, no baud rate requested, set */
                        	call->baud = call->speed = tspeed;
			} else if(tspeed != call->baud) {
				/* found line at wrong speed, keep looking */
				sperfg = 1;
				continue;
			} else {
				/* found line at correct speed, clear error */
				sperfg = 0;
			}
                }
                if(call->telno) {
                        if(call->speed != tspeed)
                                continue;
                }
                if(call->baud > call->speed)
                        continue;
                ++found;
                return(1+strrchr(cul, '/'));
        }
        return("");
}

void
undial(rfd)
int rfd;
{
#ifdef ddt
        if(_debug == YES)
                say(stderr, "call undial(%d)\n", rfd);
#endif
        if(rfd > 0) 
                (void)close(rfd);
        if(lfd > 0) {
                (void)close(lfd);
                lfd = -1;
                if(unlink(lock) < 0)
                        say(stderr, "Can't unlink lock-file\r\n");
#ifdef ddt
                else if(_debug == YES)
                        say(stderr, "Lock-file unlinked\r\n");
#endif
        }
        return;
}

static int
alrmcatch()
{
        (void)alarm(UPDTE);
        (void)utime(lock, (struct {long a,b;} *)0);
        (void)signal(SIGALRM, alrmcatch);
}

static int
hupcatch()
{
        undial(rlfd);
}

static int
intcatch()
{
        intflag = YES;
        (void)signal(SIGINT, intcatch);
}

#ifdef ddt
static void
dump(arg, fd)
CALL *arg;
int fd;
{
        struct termio xv;
        int i;

        if(fd > 0) {
                say(stderr, "\r\ndevice status for fd=%d\r\n", fd);
                say(stderr, "F_GETFL=%o\r\n", fcntl(fd, F_GETFL,1));
                if(ioctl(fd, TCGETA, &xv) < 0) {
                        char buf[100];
                        int x=errno;

                        (void)sprintf(buf, "\rtdmp for fd=%d:", fd);
                        errno = x;
                        perror(buf);
                        return;
                }
                say(stderr, "iflag=`%o',", xv.c_iflag);
                say(stderr, "oflag=`%o',", xv.c_oflag);
                say(stderr, "cflag=`%o',", xv.c_cflag);
                say(stderr, "lflag=`%o',", xv.c_lflag);
                say(stderr, "line=`%o'\r\n", xv.c_line);
                say(stderr, "cc[0]=`%o',", xv.c_cc[0]);
                for(i=1; i<8; ++i)
                        say(stderr, "[%d]=`%o',", i, xv.c_cc[i]);
                say(stderr, "\r\n");
        }
        say(stderr,"baud=%d, ",arg->baud);
        say(stderr,"speed=%d, ",arg->speed);
        say(stderr,"line=%s, ",arg->line? arg->line: "(null)");
        say(stderr,"telno=%s\r\n",arg->telno? arg->telno: "(null)");
}
#endif
