/* sched_runq.h - scheduling run queue management */

#ifndef _OS_SCHED_RUNQ_H
#define _OS_SCHED_RUNQ_H

#include <sys.h>
#include <arch.h>
#include <os/sched_core.h>
#include <os/sched_policy.h>

struct sched_thread;

typedef struct sched_runq_node
    {
    list_t          node;
    struct sched_thread * thread;
    }sched_runq_node_t;

/*
 * Each CPU has one run queue for each sched_policy;
 * Each CPU group has one group-global run queue for each sched_policy;
 * Each system has one system-global run queue for each sched_policy.
 */
typedef struct sched_runq 
    {
    char                    name[NAME_MAX]; /* Runq name */
    int                     magic;     /* Magic for validity */
    struct sched_policy *   policy;    /* Scheduling policy */
    size_t                  runnable;   /* Runnable thread count */
    spinlock_t              lock;      /* Lock for the runq */
    }sched_runq_t;

#define SCHED_RUNQ_LOCK(runq)    spinlock_lock(&(runq)->lock)
#define SCHED_RUNQ_UNLOCK(runq)  spinlock_unlock(&(runq)->lock)

struct sched_policy;

status_t sched_runq_init 
    (
    sched_runq_t *  runq,
    char *          name,
    struct sched_policy * policy
    );

#endif /* _OS_SCHED_RUNQ_H */
