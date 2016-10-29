/*
 *      Elbrus-B console Xenix driver.
 */

/* ioctls */

# define PT_ADDR        0x100                   /* set memory address */
# define PT_SEEK        0x200                   /* update memory address */
# define PT_SEND        0x400                   /* set register */
# define PT_SENDL       0x800                   /* set register */
# define PT_RECV        0x1000                  /* get direct register */
# define PT_RECVL       0x2000                  /* get direct register */

# define PTADDR         PT_ADDR                 /* set memory address */
# define PTSEEK         PT_SEEK                 /* update memory address */
# define PTSEND(b)      ((b)&0xff|PT_SEND)      /* set register */
# define PTSENDL(b)     ((b)&0xff|PT_SENDL)     /* set register */
# define PTRECV(b)      ((b)&0xff|PT_RECV)      /* get direct register */
# define PTRECVL(b)     ((b)&0xff|PT_RECVL)     /* get direct register */

# define PTMASK         0xff00                  /* ioctl mask */

struct ptword {
	long right;			/* 4 bytes - len */
	long left;			/* 4 bytes - address */
	int tag;			/* 1 byte */
};
