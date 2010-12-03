/* sched_sem.h - scheduler semaphore management */

#ifndef _OS_SCHED_SEM_H
#define _OS_SCHED_SEM_H

#include <sys.h>
#include <arch.h>
#include <os.h>

/* Kernel semaphore structure */

typedef struct sched_semaphore
    {
    /* Node for list of all semaphores - debug show */
    list_t  node;   

    /* ID of semaphore */
    id_t    id;
        
    /* Name of the semaphore */
    char    name[NAME_MAX];
    
    /* Semaphore current count */
    int     count;

    /* Threads list pending on this semaphore */
    qhead_t  waitq;    

    /* Lock for the above list */
    spinlock_t lock;

    /* Highest priority thread pending on this semaphore */
    struct sched_thread *best_waiter;

    /* Magic number */
    int magic;
    } sched_semaphore_t;


#endif /* _OS_SCHED_SEM_H */
