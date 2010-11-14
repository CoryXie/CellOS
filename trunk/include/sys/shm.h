/* sys/shm.h - XSI shared memory facility */

#ifndef _POSIX_SYS_SHM_H
#define _POSIX_SYS_SHM_H

/* 
 * The <sys/shm.h> header shall define the pid_t, size_t, and time_t
 * types as described in <sys.h>. 
 */
 
#include <sys.h>
#include <sys/ipc.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The <sys/shm.h> header shall define the following symbolic constants 
 */

#define SHM_RDONLY  0x01 /* Attach read-only (else read-write). */
#define SHM_RND     0x02 /* Round attach address to SHMLBA. */
#define SHMLBA      0x03 /* Segment low boundary address multiple.*/

/*
 * shmatt_t
 *
 * Unsigned integer used for the number of current attaches that 
 * must be able to store values at least as large as a type
 * unsigned short.
 */

typedef uint32_t shmatt_t;

/* The <sys/shm.h> header shall define the shmid_ds structure */

typedef struct shmid_ds
    {
    struct ipc_perm shm_perm;   /* Operation permission structure. */ 
    size_t          shm_segsz;  /* Size of segment in bytes. */ 
    pid_t           shm_lpid;   /* Process ID of last shared memory operation. */ 
    pid_t           shm_cpid;   /* Process ID of creator. */ 
    shmatt_t        shm_nattch; /* Number of current attaches. */
    time_t          shm_atime;  /* Time of last shmat(). */ 
    time_t          shm_dtime;  /* Time of last shmdt(). */ 
    time_t          shm_ctime;  /* Time of last change by shmctl(). */ 
    }shmid_ds_t;


void *shmat(int, const void *, int);
int   shmctl(int, int, struct shmid_ds *);
int   shmdt(const void *);
int   shmget(key_t, size_t, int);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _POSIX_SYS_SHM_H */

