/*	@(#)shmsys.c	1.2	*/
#ifndef	pdp11
#include	"sys/types.h"
#include	"sys/ipc.h"
#include	"sys/shm.h"

#define	SHMSYS	52

#define	SHMAT	0
#define	SHMCTL	1
#define	SHMDT	2
#define	SHMGET	3

shmat(shmid, shmaddr, shmflg)
int shmid;
char *shmaddr;
int shmflg;
{
	return(syscall(SHMSYS, SHMAT, shmid, shmaddr, shmflg));
}

shmctl(shmid, cmd, buf)
int shmid, cmd;
struct shmid_ds *buf;
{
	return(syscall(SHMSYS, SHMCTL, shmid, cmd, buf));
}

shmdt(shmaddr)
char *shmaddr;
{
	return(syscall(SHMSYS, SHMDT, shmaddr));
}

shmget(key, size, shmflg)
key_t key;
int size, shmflg;
{
	return(syscall(SHMSYS, SHMGET, key, size, shmflg));
}
#endif
