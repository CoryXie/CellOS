/* semaphore.h - semaphores */

#ifndef _POSIX_SEMAPHORE_H
#define _POSIX_SEMAPHORE_H

#include <fcntl.h>
#include <stdint.h>
#include <time.h>
#include <os/sched_sem.h>

/*
 * The <semaphore.h> header shall define the sem_t type, used 
 * in performing semaphore operations. The semaphore may be 
 * implemented using a file descriptor, in which case applications
 * are able to open up at least a total of {OPEN_MAX} files 
 * and semaphores.
 */
 
typedef struct sched_semaphore  sem_t;

/* 
 * The <semaphore.h> header shall define the symbolic constant
 * SEM_FAILED which shall have type sem_t *.
 */
 
#define SEM_FAILED	((sem_t*)(NULL))

#ifdef __cplusplus
extern "C" {
#endif

int    sem_close(sem_t *);
int    sem_destroy(sem_t *);
int    sem_getvalue(sem_t *restrict, int *restrict);
int    sem_init(sem_t *, int, unsigned);
sem_t *sem_open(const char *, int, ...);
int    sem_post(sem_t *);
int    sem_timedwait(sem_t *restrict, const struct timespec *restrict);
int    sem_trywait(sem_t *);
int    sem_unlink(const char *);
int    sem_wait(sem_t *);

#ifdef __cplusplus
}
#endif


#endif	/* _POSIX_SEMAPHORE_H */
