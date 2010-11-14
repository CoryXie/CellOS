#ifndef _OS_PREEMPTION_H
#define _OS_PREEMPTION_H

#include <sys.h>
#include <arch.h>
#include <os/sched_core.h>

/** Increment preemption disabled counter. */
static inline void preemption_disable(void)
    {
    /* Increase preemption_disabled of the current thread */
    
    kurrent->preemption_disabled++;
    
    memory_barrier();
    }

/** Decrement preemption disabled counter. */
static inline void preemption_enable(void)
    {
    /* Make sure preemption_disabled has been increased before */
    
    ASSERT(kurrent->preemption_disabled); 
    
    memory_barrier();

    /* Decrease preemption_disabled of the current thread */
    
    kurrent->preemption_disabled--;
    }


#endif /* _OS_PREEMPTION_H */

