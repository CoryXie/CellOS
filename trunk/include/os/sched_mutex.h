/* sched_mutex.h - scheduler mutex management */

#ifndef _OS_SCHED_MUTEX_H
#define _OS_SCHED_MUTEX_H

#include <sys.h>
#include <arch.h>
#include <os/queue.h>

struct sched_thread;
struct sched_policy;

typedef struct sched_mutex_attr
    {
    /* Magic number */
    int             magic;
    
    /* The mutex name */
    char            name[NAME_MAX];
    
    /* Priority Inheritance or Priority Ceiling Protocol */
    int             protocol;  
    
    /* Ceiling priority when protocol is PTHREAD_PRIO_PROTECT */
    int             prioceiling;
    
    /* Type - Default, Normal, ErrorCheck, Recursive, Robust, Stalled */
    int             type;  

    /* Shared between processes? */
    int             pshared;

    /* Pending threads wake-up order - FIFO or Priority based? */
    int             order;

    /* How to handle the case if thread terminated while holding a lock? */
    int             robust;
    
    }sched_mutex_attr_t;

/* Kernel mutex structure */

typedef struct sched_mutex
    {
    /* Node for list of all mutexes - debug show */
    list_t  node;   
    
    /* Mutext attributes */
    struct sched_mutex_attr attr;

    /* Thread owning the mutex */
    struct sched_thread *owner;

    /* Owner scheduling policy */
    struct sched_policy * owner_policy;
    
    /* Original priority of owner thread */
    int owner_priority;

    /* Highest priority thread pending on this mutex */
    struct sched_thread *best_waiter;

    /* Mutex ownership count of the owning thread */
    atomic_t counter;

    /* Threads list pending on this mutex */
    qhead_t waitq;    

    /* Lock for the above list */
    spinlock_t lock;

    /* Magic number */
    int magic;
    } sched_mutex_t;

/* Default max recursive count */
#define SCHED_MUTEX_MAX_RECURSIVES 32

#endif /* _OS_SCHED_MUTEX_H */
