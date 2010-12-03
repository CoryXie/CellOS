/* sched_runq.c - scheduling run queue manangement */

#include <sys.h>
#include <arch.h>
#include <os.h>

status_t sched_runq_init 
    (
    sched_runq_t *  runq,
    char *          name,
    struct sched_policy * policy
    )
    {
    runq->policy = policy;
    
    runq->magic = MAGIC_VALID;

    runq->runnable = 0;

    strncpy(runq->name, name, NAME_MAX);

    spinlock_init(&runq->lock);
    
    return OK;
    }
