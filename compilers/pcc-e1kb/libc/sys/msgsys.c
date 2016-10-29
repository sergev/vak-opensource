/*	@(#)msgsys.c	1.1	*/
#include	"sys/types.h"
#include	"sys/ipc.h"
#include	"sys/msg.h"

#define	MSGSYS	49

#define	MSGGET	0
#define	MSGCTL	1
#define	MSGRCV	2
#define	MSGSND	3

msgget(key, msgflg)
key_t key;
int msgflg;
{
	return(syscall(MSGSYS, MSGGET, key, msgflg));
}

msgctl(msqid, cmd, buf)
int msqid, cmd;
struct msqid_ds *buf;
{
	return(syscall(MSGSYS, MSGCTL, msqid, cmd, buf));
}

msgrcv(msqid, msgp, msgsz, msgtyp, msgflg)
int msqid;
struct msgbuf *msgp;
int msgsz;
long msgtyp;
int msgflg;
{
	return(syscall(MSGSYS, MSGRCV, msqid, msgp, msgsz, msgtyp, msgflg));
}

msgsnd(msqid, msgp, msgsz, msgflg)
int msqid;
struct msgbuf *msgp;
int msgsz, msgflg;
{
	return(syscall(MSGSYS, MSGSND, msqid, msgp, msgsz, msgflg));
}
