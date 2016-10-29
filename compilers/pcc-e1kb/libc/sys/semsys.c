/*	@(#)semsys.c	1.1	*/
#include	"sys/types.h"
#include	"sys/ipc.h"
#include	"sys/sem.h"

#define	SEMSYS	53

#define	SEMCTL	0
#define	SEMGET	1
#define	SEMOP	2

semctl(semid, semnum, cmd, arg)
int semid, cmd;
int semnum;
union semun {
	int val;
	struct semid_ds *buf;
	ushort array[1];
} arg;
{
	return(syscall(SEMSYS, SEMCTL, semid, semnum, cmd, arg));
}

semget(key, nsems, semflg)
key_t key;
int nsems, semflg;
{
	return(syscall(SEMSYS, SEMGET, key, nsems, semflg));
}

semop(semid, sops, nsops)
int semid;
struct sembuf (*sops)[];
int nsops;
{
	return(syscall(SEMSYS, SEMOP, semid, sops, nsops));
}
