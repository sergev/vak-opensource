# define MAX_BATCH      0x10

union word {
	unsigned long   l[2];
	unsigned int    i[4];
	unsigned char   c[8];
};

struct wordb {                          /* § £®«®¢®ª */
	int tag;                        /* 1 byte */
	long left;                      /* 4 bytes - address */
	long right;                     /* 4 bytes - len */
};

struct ioregs {
	long r [16];            /* Œ0-ŒF index registers */
	long s [8];             /* ˆ‘0-ˆ‘7 special registers */
	union word a;           /* ‘Œ accumulator */
	union word i;           /* ‚ input register */
	union word y;           /* Œ young register */
	long pc;                /* ‘—€‘ address of next command */
	long cr;                /* Š command register */
};

extern void pt_store(), pt_sendl(), pt_init(), stop(), zero(), pt_enable(),run();
extern int  put_tst_file(), put_any_file(), get_any_file(), get_part_to_file ();
extern int  put_file_to_part (), pt_load();

# ifdef MSDOS
# ifndef MK_FP
# define MK_FP(seg, off) (long) (((long)(seg) << 16) | (off))
# endif

/* BIOS printer port base */

# define PSEG   0
# define POFF   0x408

/* register definitions */

# ifdef VAR_PORT

# define RDATA          (pt_port)       /* place characters here */
# define RSTATUS        (pt_port1)      /* non zero means busy */
# define RCNTRL         (pt_port2)      /* write control here */

# else

# define RDATA          ( 0x378 )       /* place characters here */
# define RSTATUS        (RDATA+1)       /* non zero means busy */
# define RCNTRL         (RDATA+2)       /* write control here */

#endif

/* control port definitions */

# define CSTROBE        1               /* 1 when sending byte */
# define CAUTOLF        2               /* 1 causes LF after CR */
# define CINIT          4               /* 0 resets the printer */
# define CSELECT        010             /* 1 selects the printer */
# define CIENABL        020             /* interrupt when SACK goes false */

/* status port definitions */

# define SERROR         010             /* 0=printer signals an error */
# define SSELECT        020             /* 1=printer is selected */
# define SPE            040             /* 1=out of paper */
# define SACK           0100            /* 0=ready for next character */
# define SBUSY          0200            /* 0=busy or offline or error */
# endif

/* Elbrus-B control commands */

/* read from to Elbrus-B */

# define BRDWORD       0210            /* 210-217 Œ— */

/* write to Elbrus-B */

# define BBUTTON1       0351            /* “‘Š, Š’, “0‘, “0 */
# define BBUTTON2       0352            /* ƒ—€“, ˆŠ…, “‘… */
# define BBUTTON3       0353            /* “‘Š€, ‡€‘, “ƒ€Œ */

# define BINDEX         0357            /* ˆ€ */
# define BSTEP          0362            /*  */

# define BCMD           0371            /* Š */
# define BCMDADDR       0372            /* €Š */
# define BMEMADDR       0373            /* €— */
# define BADDR          0374            /* €Œ */
# define BWRWORD        0376            /* Œ‡ */

# ifdef MSDOS
/* Elbrus-B phys memory mask */

# define ADDRMASK       0x7fffffL

/* toggle printer STROBE bits */
# define pt_strobe() { outp(RCNTRL, CINIT|CSTROBE); outp(RCNTRL, CINIT); }

/* toggle printer INIT bits */
# define pt_init() { outp(RCNTRL, 0); outp(RCNTRL, CINIT); }

/* send byte to printer port */
# define pt_putc(c) { outp(RDATA,~(c)); pt_strobe(); }

/* receive bit from Elbrus-B and invert it */
# define pt_getbit() ((inp (RSTATUS) & SPE) == 0)

/* send data to Elbrus-B */
# define pt_data(c) { pt_init(); pt_putc(c); }

/* send command to Elbrus-B */
# define pt_send(c,d) { pt_data (c); pt_putc (BINDEX); pt_putc(d); }

/* get byte from Elbrus-B */
# define pt_get(addr, bits, cc) {\
	c = (unsigned char) (0xFFu << (bits)); \
	outp(RDATA, ~(addr)); \
	for ( j = 0; j < (bits); j++) { \
		pt_strobe(); \
		c |= (((inp ( RSTATUS ) >> 5) & 1) << j); \
	} \
	(cc) = ~c; \
}
# else
/* send command to Elbrus-B */
# define pt_send(c,d)    ioctl (f_pult, PTSEND (d & 0xff), (char far *) (long) c);
# endif

extern int f_pult;
extern unsigned long old_addr, pt_getl ();
extern unsigned char pt_getb ();
extern unsigned pt_port, pt_port1, pt_port2; /* base printer port address */
extern int open_part ();

