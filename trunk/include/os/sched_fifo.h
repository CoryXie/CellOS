/* sched_fifo.h - First in, first out (FIFO) scheduling policy. */

#ifndef _OS_SCHED_FIFO_H
#define _OS_SCHED_FIFO_H

#include <sys.h>
#include <arch.h>
#include <os/sched_core.h>

/* Per thread schedule parameter for this scheduling policy */
typedef struct sched_fifo_param
    {
    /* Current scheduling priority */
	int             sched_priority;
    }sched_fifo_param_t;

extern sched_policy_t   sched_policy_fifo;

status_t sched_policy_fifo_init (void);

#endif /* _OS_SCHED_FIFO_H */
