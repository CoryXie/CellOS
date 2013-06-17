/* sys/sem.h - XSI semaphore facility */

#ifndef _POSIX_SYS_SEM_H
#define _POSIX_SYS_SEM_H

#include <sys.h>
#include <sys/ipc.h>

/*
 * The <sys/sem.h> header shall define the following symbolic
 * constant for use as a semaphore operation flag
 */
 
/* Set up adjust on exit entry. */
#define SEM_UNDO	10

/* 
 * The <sys/sem.h> header shall define the following symbolic 
 * constants for use as commands for the semctl() function
 */
 
#define GETPID		3	/* Get process ID of last element manipulating */
#define GETVAL		4	/* Get semval */
#define GETALL		5	/* Get all semval */
#define GETNCNT		6	/* Get semncnt */
#define GETZCNT		7	/* Get semzcnt */
#define SETVAL		8	/* Set semval */
#define SETALL		9	/* Set all semval */

/* The <sys/sem.h> header shall define the semid_ds structure */
typedef struct semid_ds 
    {
	struct ipc_perm		sem_perm;	/* Operation permission structure */
	unsigned short		sem_nsems;	/* Number of semaphores in set */
	time_t				sem_otime;	/* Last semop */
	time_t				sem_ctime;	/* Last time changed by semctl */
    }semid_ds_t;

/* Structure passed as parameter to the semop function */
typedef struct sembuf 
    {
	unsigned short	sem_num;	/* Semaphore number */
	short			sem_op;		/* Semaphore operation */
	short			sem_flg;	/* Operation flags */
    }sembuf_t;

/*
 * A semaphore shall be represented by an anonymous structure
 */

typedef struct anonymous_sem
    {
    unsigned short  semval;   /* Semaphore value. */ 
    pid_t           sempid;   /* Process ID of last operation. */ 
    unsigned short  semncnt;  /* Number of processes waiting for semval 
                               * to become greater than current value. */ 
    unsigned short  semzcnt;  /* Number of processes waiting for semval 
                               * to become 0. */
    }anonymous_sem_t;
/*
 * Semaphore info structure. Useful for the ipcs
 * standard utily
 */
 
typedef struct seminfo 
    {
	int	semmni;	/* Number of semaphore identifies */
	int semmns;	/* Number of semaphore in system */
	int semmnu;	/* Number of undo structures in system */
	int semmsl;	/* Max number of semaphores per id */
	int semopm;	/* Max number of operations per semop call */
	int semume;	/* Max number of undo entries per process */
	int semusz;	/* Size in bytes of undo structure */
	int semvmx;	/* Semaphore maximum valure */
	int semaem;	/* adjust on exit max value */
    }seminfo_t;

#ifdef __cplusplus
extern "C" {
#endif

int		semctl(int semID, int semNum, int command, ...);
int		semget(key_t key, int numSems, int semFlags);
int		semop(int semID, struct sembuf *semOps, size_t numSemOps);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* _POSIX_SYS_SEM_H */
