/* sys/msg.h - XSI message queue structures */

#ifndef _POSIX_SYS_MSG_H
#define _POSIX_SYS_MSG_H

#include <sys/cdefs.h>
#include <sys/ipc.h>
#include <sys.h>

typedef uint32_t msgqnum_t; /* Used for the number of messages in the message queue. */
typedef uint32_t msglen_t;  /* Used for the number of bytes allowed in a message queue.*/

/* 
 * The <sys/msg.h> header shall define the following symbolic
 * constant as a message operation flag
 */
 
#define MSG_NOERROR 0x1000 /* No error if big message */

/* The <sys/msg.h> header shall define the msqid_ds structure */

typedef struct msqid_ds 
    {
	struct ipc_perm		msg_perm;	/* Operation permission structure */
	msgqnum_t			msg_qnum;	/* Number of messages currently on queue */
	msglen_t			msg_qbytes;	/* Max number of bytes allowed on queue */
	pid_t				msg_lspid;	/* PID of last msgsnd */
	pid_t				msg_lrpid;	/* PID of last msgrcv */
	time_t				msg_stime;	/* Time of last msgsnd */
	time_t				msg_rtime;	/* Time of last msgrcv */
	time_t				msg_ctime;	/* Time of last change */
    }msqid_ds_t;

/* Structure used to send/receive a message */

typedef struct msgbuf 
    {
	long	mtype;		/* message type */
	char	mtext[1];	/* message text */
    }msgbuf_t;

#ifdef __cplusplus
extern "C" {
#endif

int			msgctl(int, int, struct msqid_ds *);
int			msgget(key_t, int);
ssize_t		msgrcv(int, void *, size_t, long, int);
int			msgsnd(int, const void *, size_t, int);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* _POSIX_SYS_MSG_H */
