/* sched_sem.h - scheduler semaphore management */

#ifndef _OS_SCHED_SEM_H
#define _OS_SCHED_SEM_H

/* Kernel semaphore structure */

typedef struct ksemaphore
    {
    unsigned long   id;     /* semaphore id which is an interger */
    char *          name;   /* The semaphore name */
    int             flags;  /* Flags of this semaphore */
    
    /* Current semaphore count. Zero means not available. */
    
    size_t          count;

    /* Threads list and count pending on this semaphore */
    
    list_t          pending_thread_list;   
    unsigned long   pending_thread_count;

    /* List of all semaphores */
    
    list_t          list;   
    } ksemaphore_t;

/* 
 * Create a semaphore with specified name 
 * and initial count 
 */

ksemaphore_t * ksemaphore_create
    (
    char * name, 
    int count,
    int flags
    );

/* Delete a semaphore */

int ksemaphore_delete
    (
    ksemaphore_t * sema    
    );

/* Take a semaphore */

int ksemaphore_take
    (
    ksemaphore_t * sema,
    long wait_option
    );

/* Give a semaphore */

int ksemaphore_give
    (
    ksemaphore_t * sema
    );

/* 
 * Prioritize the pending thread to the head position
 * of the pending thread list
 */

int ksemaphore_prioritize
    (
    ksemaphore_t * sema    
    );

#endif /* _OS_SCHED_SEM_H */
