/* sched_rr.h - round-robin scheduling policy */

#ifndef _OS_SCHED_RR_H
#define _OS_SCHED_RR_H

#include <sys.h>
#include <arch.h>
#include <sched.h>
#include <os/sched_core.h>


/* Per thread schedule parameter for this scheduling policy */
typedef struct sched_rr_param
    {
    /* Current scheduling priority */
	int     sched_priority;
    }sched_rr_param_t;

extern sched_policy_t   sched_policy_rr;

status_t sched_policy_rr_init (void);

#endif /* _OS_SCHED_RR_H */
