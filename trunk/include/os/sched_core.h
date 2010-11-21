/* sched_core.h - Core OS scheduling interface */

#ifndef _OS_SCHED_CORE_H
#define _OS_SCHED_CORE_H

#include <sys.h>
#include <arch.h>
#include <sched.h>
#include <os/timer.h>
#include <os/sched_runq.h>
#include <os/sched_policy.h>
#include <os/sched_cpu.h>
#include <os/sched_mutex.h>
#include <os/sched_thread.h>
#include <os/sched_task.h>

extern sched_thread_t * kthread_current[];
extern sched_cpu_t*    current_cpus[];
extern list_t   kthread_list[];
extern uint64_t timer_ticks[];
extern long     cpu_intr_flags[];
extern spinlock_t reschedule_lock;

#if 0
#define SCHED_LOCK()    spinlock_lock(&reschedule_lock)
#define SCHED_UNLOCK()  spinlock_unlock(&reschedule_lock)
#else
#define SCHED_LOCK()    
#define SCHED_UNLOCK()  
#endif

#define kurrent kthread_current[this_cpu()]
#define kurrent_cpu current_cpus[this_cpu()] 
#define kurrent_tsk kurrent->tsk
#define kurrent_asp kurrent->asp

#define __cpu_intr_flags cpu_intr_flags[this_cpu()]

void sched_init(void);

void sched_tick
    (
    struct stack_frame * frame
    );

void sched_thread_common_entry
    (
    void *param
    );

void cpu_heart_beat 
    (
    int cpu
    );

extern __attribute__ ((returns_twice)) 
    int context_save(sched_context_t *c);

extern __attribute__ ((noreturn))
    void context_restore(sched_context_t *c);

extern void reschedule(void);

#endif /* _OS_SCHED_CORE_H */
