/* sys/ipc.h - XSI interprocess communication access structure */

/*
 * The <sys/ipc.h> header is used by three mechanisms for XSI 
 * interprocess communication (IPC): messages, semaphores, and 
 * shared memory. All use a common structure type, ipc_perm,
 * to pass information used in determining permission to perform
 * an IPC operation.
 */
 
#ifndef _POSIX_SYS_IPC_H
#define _POSIX_SYS_IPC_H

#include <sys/cdefs.h>
#include <sys.h>

/* Mode bits for msgget(), semget(), and shmget() */
#define IPC_CREAT	01000	/* Create entry if key does not exist. */
#define IPC_EXCL	02000	/* Fail if key exists. */
#define IPC_NOWAIT	04000	/* Error if request must wait. */

/* Control commands for msgctl(), semctl(), and shmctl() */
#define IPC_RMID	0		/* Remove identifier. */
#define IPC_SET		1		/* Set options */
#define IPC_STAT	2		/* Get options */

/* Private key */
#define IPC_PRIVATE		(key_t)0x5A

/* The <sys/ipc.h> header shall define the ipc_perm structure */
struct ipc_perm 
    {
	key_t	key;			/* IPC identifier */
	uid_t	uid;			/* owner's user ID */
	gid_t	gid;			/* owner's group ID */
	uid_t	cuid;			/* creator's user ID */
	gid_t	cgid;			/* creator's group ID */
	mode_t	mode;			/* Read/write permission */
    };

#ifdef __cplusplus
extern "C" {
#endif

key_t ftok(const char *path, int id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* _POSIX_SYS_IPC_H */
