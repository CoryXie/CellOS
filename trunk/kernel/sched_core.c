#include <sys.h>
#include <arch.h>
#include <os.h>
long cpu_intr_flags[CONFIG_NR_CPUS];

uint64_t timer_ticks[CONFIG_NR_CPUS];

spinlock_t reschedule_lock;

sched_thread_t * kthread_current[CONFIG_NR_CPUS];

extern void *sched_bsp_idle_thread (void *notused);
extern void *sched_ap_idle_thread (void *notused);

int do_kthreads (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
    {
    sched_thread_global_show();
    return 0;
    }

CELL_OS_CMD(
    threads,    CONFIG_NR_CPUS + 1,        1,    do_kthreads,
    "show kernel thread list information",
    "[cpu...]\n"
    "    - list the kernel thread information on the specified cpus;\n"
    "if the cpu arguments are not specified, then list for all cpus."
    );

CELL_OS_CMD(
    i,    CONFIG_NR_CPUS + 1,        1,    do_kthreads,
    "alias for 'threads'",
    "[cpu...]\n"
    "    - list the kernel thread information on the specified cpus;\n"
    "if the cpu arguments are not specified, then list for all cpus."
    );

int do_ticks (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
    {
    for (int i = 0; i < smp_total_cpu_count(); i++)
        printk("cpu%d - %lld\n", i, timer_ticks[i]);
    
    return 0;
    }

CELL_OS_CMD(
    ticks,    CONFIG_NR_CPUS + 1,        1,    do_ticks,
    "get already running ticks",
    "[cpu...]\n"
    "get the current ticks that has passed"
    );

extern sched_cpu_t* current_cpus[];

void sched_core_init(void)
    {
    spinlock_init(&reschedule_lock);
    
    sched_cpu_init();
        
    sched_policy_init();

    sched_thread_init();

    memory_barrier();
    }

void sched_init(void)
    {
    char name[NAME_MAX];
    pthread_attr_t thread_attr;
    ulong flags;
    
    timer_ticks[this_cpu()] = 0;
    
    snprintf(name, NAME_MAX, "INIT_CPU%d", this_cpu());
    
    pthread_attr_init(&thread_attr);
        
    pthread_attr_setname_np(&thread_attr, name);
    
    pthread_attr_setautorun_np(&thread_attr, FALSE);

    pthread_attr_getflags_np(&thread_attr, &flags);

    flags |= THREAD_STANDALONE | THREAD_UNPREEMPTABL;
    
    pthread_attr_setflags_np(&thread_attr, flags);

    pthread_attr_setschedpolicy(&thread_attr, SCHED_RR);
    pthread_attr_settimeslice_np(&thread_attr, 100);

    if (this_cpu() == 0)
        {
        pthread_create(&kurrent,
                       &thread_attr,
                       sched_bsp_idle_thread,
                       NULL);
        }
    else
        {
        pthread_create(&kurrent,
                       &thread_attr,
                       sched_ap_idle_thread,
                       NULL);
        }
    
    sched_thread_remove_suspended(kurrent);
    
    spinlock_lock(&kurrent->thread_lock);

    kurrent->state = STATE_RUNNING;

    kurrent_cpu->prev_thread = kurrent_cpu->current = kurrent;
    
    kurrent_cpu->idle_thread = kurrent;

    kurrent->saved_context.ipl &= ~(RFLAGS_IF);
    
	context_restore(&kurrent_cpu->idle_thread->saved_context);
    }

void sched_context_dump (sched_context_t * contxt)
    {
    printk("Context ipl %p, pc %p, sp %p, rbp %p, rbx %p, "
           "r12 %p,r13 %p,r14 %p,r15 %p\n",
        contxt->ipl,
        contxt->pc,
        contxt->sp,
        contxt->rbp,
        contxt->rbx,
        contxt->r12,contxt->r13,contxt->r14,contxt->r15
        );
    }

#undef SCHED_DETAIL        

/* The SCHED_LOCK is called in reschedule */
void sched_thread_common_entry
    (
    void *param
    )
    {
    sched_thread_arch_post_switch(kurrent);

    kurrent->state = STATE_RUNNING;
    kurrent->resume_cycle = rdtsc();

#ifdef SCHED_DETAIL        
            printk("cpu%d - release lock for %s\n", 
            this_cpu(), kurrent->name);
#endif  
    
    spinlock_unlock(&kurrent->thread_lock);

    interrupts_restore(kurrent->saved_context.ipl);

    if (kurrent != kurrent_cpu->prev_thread)
        {
#ifdef SCHED_DETAIL        
        printk("cpu%d - release lock for %s\n", 
        this_cpu(), kurrent_cpu->prev_thread->name);
#endif  
        spinlock_unlock(&kurrent_cpu->prev_thread->thread_lock);
                
        if (!(kurrent_cpu->prev_thread->flags & THREAD_STANDALONE) &&
            (kurrent_cpu->prev_thread->state == STATE_READY))
            {
            kurrent_cpu->prev_thread->sched_policy->thread_enqueue(
                kurrent_cpu->prev_thread->sched_runq,
                kurrent_cpu->prev_thread, FALSE);
            }
        }

    kurrent->entry((void *)(kurrent->param));  

    kurrent->state = STATE_COMPLETED;
    }

/*
 * reschedule - scheduler core entry point
 */
 
void reschedule(void)
    {
    ipl_t ipl;
    sched_thread_t * new_thread;
    sched_thread_t * check_thread = kurrent;
    
#ifdef SCHED_DETAIL        
    printk("cpu%d - switching %s 1 state %s\n", 
    this_cpu(), kurrent->name, sched_thread_state_name(kurrent->state));
#endif  
    
    ipl = interrupts_disable();

    spinlock_lock(&kurrent->thread_lock);

#ifdef SCHED_DETAIL        
    printk("cpu%d - switching %s 2 state %s\n", 
    this_cpu(), kurrent->name, sched_thread_state_name(kurrent->state));
#endif  

    switch (kurrent->state)
        {
        case STATE_READY:
            check_thread = NULL;
            break;
        case STATE_PENDING:
            check_thread = NULL;
            break;
        case STATE_SUSPENDED:
            check_thread = NULL;
            break;
        case STATE_RUNNING:
            kurrent->state = STATE_READY;
            break;
        default:break;
        }

    if (check_thread && check_thread == kurrent_cpu->idle_thread)
        check_thread = NULL;
    
    new_thread = sched_find_best_thread(check_thread);

    if (new_thread == NULL)
        {
        new_thread = kurrent_cpu->idle_thread;

        kurrent_cpu->idle = TRUE;
        }
    else
        {
        kurrent_cpu->idle = FALSE;
        }

    if (new_thread != kurrent)
        {
        kurrent_cpu->prev_thread = kurrent;
        kurrent->saved_context.ipl = ipl;
        
        kurrent = new_thread;
        
        kurrent_cpu->current = kurrent;
        kurrent->cpu_idx = kurrent_cpu->cpu_idx;
        kurrent->sched_cpu = kurrent_cpu;

        kurrent->runcount++;
        
#ifdef SCHED_DETAIL        
        printk("cpu%d - switching from %s to %s\n", 
        this_cpu(), kurrent_cpu->prev_thread->name, kurrent->name);
#endif  
        /* 
         * context_save() returns 1, which causes context_restore() to 
         * run; context_restore() will have two possible directions:
         *
         * 1. If the new thread (kurrent) is the first time to run, it
         * will return to sched_thread_common_entry();
         *
         * 2. If the new thread (kurrent) has already been run, which
         * means it has previously been context_save()'d, then it will
         * return just back to the point where context_save() give off
         * control in the last context_save() call, and that is just
         * where the 'if' statement sits. This time, context_restore()
         * returns 0, and thus the 'if' statement judges to go the 
         * remaining part of this function. 
         *
         * This is why context_save() is marked as 'returns_twice'.
         * Since the context_restore() does not return to its normal
         * rturn path but causes the cpu to go somewhere else, it is 
         * marked as 'noreturn'.
         *
         * Another effect of context_restore() is that it switches the
         * stack pointer; the code before context_save() is actually
         * running with the stack space of 'kurrent_cpu->prev_thread',
         * while the code after context_restore() runs with stack space
         * of the new thread (kurrent). For this reason, it is critical
         * to note that the local varibles after context_restore() are
         * actually the "old local variables" of the new thread in its
         * last run to "reschedule".
         */
        if (context_save(&kurrent_cpu->prev_thread->saved_context)) 
            {
            context_restore(&kurrent->saved_context);
            }
        
        sched_thread_arch_post_switch(kurrent);

        kurrent->state = STATE_RUNNING;
        kurrent->resume_cycle = rdtsc();

#ifdef SCHED_DETAIL        
        printk("cpu%d - release lock for %s\n", 
        this_cpu(), kurrent_cpu->prev_thread->name);
#endif  

        spinlock_unlock(&kurrent_cpu->prev_thread->thread_lock);
        
        if (!(kurrent_cpu->prev_thread->flags & THREAD_STANDALONE))
            {
#ifdef SCHED_DETAIL        
            printk("cpu%d - previous thread %s state %s\n",
                this_cpu(),  
                kurrent_cpu->prev_thread->name,
                sched_thread_state_name(kurrent_cpu->prev_thread->state));
#endif  
            if (kurrent_cpu->prev_thread->state == STATE_READY)
                {
                kurrent_cpu->prev_thread->sched_policy->thread_enqueue(
                    kurrent_cpu->prev_thread->sched_runq,
                    kurrent_cpu->prev_thread, FALSE);
                }
            else
                {
                printk("cpu%d - current thread %s lost previous thread %s state %s\n",
                    this_cpu(), 
                    kurrent->name,
                    kurrent_cpu->prev_thread->name,
                    sched_thread_state_name(kurrent_cpu->prev_thread->state));
                }
            }
        }
    else
        {
#ifdef SCHED_DETAIL        
        printk("cpu%d - No switching, staying %s\n",
               this_cpu(), kurrent->name);
#endif   
#ifdef SCHED_DETAIL        
        printk("cpu%d - release lock for %s\n", 
        this_cpu(), kurrent->name);
#endif  
        kurrent->state = STATE_RUNNING;

        spinlock_unlock(&kurrent->thread_lock);
        }
    
    interrupts_restore(kurrent->saved_context.ipl);
    }

void sched_tick
    (
    stack_frame_t *frame
    )
    {    
    timer_ticks[this_cpu()]++;

    if ((timer_ticks[this_cpu()] % 100000) == 0)
        sched_thread_global_show();

    if ((timer_ticks[this_cpu()] % 100) == 0)
        {
        BOOL sched = TRUE;
        
        if (kurrent->sched_policy->sched_clock_tick)
            {
            sched = kurrent->sched_policy->sched_clock_tick(kurrent);
            }
        
        if (sched)
            reschedule();
        }
    }

/*
 * NAME
 *
 * sched_get_priority_min - get priority min limit (REALTIME)
 * sched_get_priority_max - get priority max limit (REALTIME)
 *
 * SYNOPSIS
 *
 * #include <sched.h>
 *
 * int sched_get_priority_max(int policy);
 * int sched_get_priority_min(int policy); 
 *
 * DESCRIPTION
 *
 * The sched_get_priority_max() and sched_get_priority_min() functions
 * shall return the appropriate maximum or minimum, respectively, for
 * the scheduling policy specified by policy.
 *
 * The value of policy shall be one of the scheduling policy values 
 * defined in <sched.h>.
 *
 * RETURN VALUE
 *
 * If successful, the sched_get_priority_max() and sched_get_priority_min()
 * functions shall return the appropriate maximum or minimum values, 
 * respectively. If unsuccessful, they shall return a value of -1 and set
 * errno to indicate the error.
 *
 * ERRORS
 *
 * The sched_get_priority_max() and sched_get_priority_min() functions shall
 * fail if:
 *
 * [EINVAL]
 *      The value of the policy parameter does not represent a defined 
 *      scheduling policy.
 */
int sched_get_priority_max
    (
    int policy
    )
    {
    sched_policy_t * sched_policy = sched_policy_get_by_id(policy);

    if (!sched_policy)
        return EINVAL;

    return sched_policy->priority_max;
    }

int sched_get_priority_min
    (
    int policy
    )
    {
    sched_policy_t * sched_policy = sched_policy_get_by_id(policy);

    if (!sched_policy)
        return EINVAL;

    return sched_policy->priority_min;
    }
/*
 * NAME
 *
 * sched_getparam - get scheduling parameters (REALTIME)
 *
 * SYNOPSIS
 *
 * #include <sched.h>
 *
 * int sched_getparam(pid_t pid, struct sched_param *param); 
 *
 * DESCRIPTION
 *
 * The sched_getparam() function shall return the scheduling parameters 
 * of a process specified by pid in the sched_param structure pointed 
 * to by param.
 *
 * If a process specified by pid exists, and if the calling process has 
 * permission, the scheduling parameters for the process whose process ID
 * is equal to pid shall be returned.
 *
 * If pid is zero, the scheduling parameters for the calling process shall 
 * be returned. The behavior of the sched_getparam() function is unspecified 
 * if the value of pid is negative.
 *
 * RETURN VALUE
 *
 * Upon successful completion, the sched_getparam() function shall return 
 * zero. If the call to sched_getparam() is unsuccessful, the function shall 
 * return a value of -1 and set errno to indicate the error.
 *
 * ERRORS
 *
 * The sched_getparam() function shall fail if:
 *
 * [EPERM]
 *
 *      The requesting process does not have permission to obtain the 
 *      scheduling parameters of the specified process.
 *
 * [ESRCH]
 *
 * No process can be found corresponding to that specified by pid.
 */
int sched_getparam
    (
    pid_t pid, 
    struct sched_param *param
    )
    {    
    return OK;
    }

/*
 * NAME
 *
 * sched_setparam - set scheduling parameters (REALTIME)
 * 
 * SYNOPSIS
 *
 * #include <sched.h>
 *
 * int sched_setparam(pid_t pid, const struct sched_param *param); 
 *
 * DESCRIPTION
 *
 * The sched_setparam() function shall set the scheduling parameters of 
 * the process specified by pid to the values specified by the sched_param
 * structure pointed to by param. The value of the sched_priority member
 * in the sched_param structure shall be any integer within the inclusive
 * priority range for the current scheduling policy of the process specified 
 * by pid. Higher numerical values for the priority represent higher 
 * priorities. If the value of pid is negative, the behavior of the 
 * sched_setparam() function is unspecified.
 *
 * If a process specified by pid exists, and if the calling process has 
 * permission, the scheduling parameters shall be set for the process whose 
 * process ID is equal to pid.
 *
 * If pid is zero, the scheduling parameters shall be set for the calling 
 * process.
 *
 * The conditions under which one process has permission to change the 
 * scheduling parameters of another process are implementation-defined.
 *
 * Implementations may require the requesting process to have appropriate
 * privileges to set its own scheduling parameters or those of another process.
 *
 * See Scheduling Policies for a description on how this function affects 
 * the scheduling of the threads within the target process.
 *
 * If the current scheduling policy for the target process is not SCHED_FIFO,
 * SCHED_RR, or SCHED_SPORADIC, the result is implementation-defined; this 
 * case includes the SCHED_OTHER policy.
 *
 * The specified sched_ss_repl_period shall be greater than or equal to the 
 * specified sched_ss_init_budget for the function to succeed; if it is not,
 * then the function shall fail.
 *
 * The value of sched_ss_max_repl shall be within the inclusive range 
 * [1,{SS_REPL_MAX}] for the function to succeed; if not, the function
 * shall fail. It is unspecified whether the sched_ss_repl_period and 
 * sched_ss_init_budget values are stored as provided by this function
 * or are rounded to align with the resolution of the clock being used. 
 *
 * This function is not atomic with respect to other threads in the process. 
 * Threads may continue to execute while this function call is in the process 
 * of changing the scheduling policy for the underlying kernel-scheduled 
 * entities used by the process contention scope threads.
 *
 * RETURN VALUE
 *
 * If successful, the sched_setparam() function shall return zero.
 *
 * If the call to sched_setparam() is unsuccessful, the priority shall
 * remain unchanged, and the function shall return a value of -1 and set 
 * errno to indicate the error.
 *
 * ERRORS
 *
 * The sched_setparam() function shall fail if:
 *
 * [EINVAL]
 *      One or more of the requested scheduling parameters is outside the 
 *      range defined for the scheduling policy of the specified pid.
 *
 * [EPERM]
 *      The requesting process does not have permission to set the scheduling 
 *      parameters for the specified process, or does not have appropriate 
 *      privileges to invoke sched_setparam().
 *
 * [ESRCH]
 *      No process can be found corresponding to that specified by pid.
 */
 
int sched_setparam
    (
    pid_t pid, 
    const struct sched_param *param
    )
    {
    return OK;
    }

/*
 * NAME
 *
 * sched_getscheduler - get scheduling policy (REALTIME)
 * 
 * SYNOPSIS
 *
 * #include <sched.h>
 *
 * int sched_getscheduler(pid_t pid); 
 *
 * DESCRIPTION
 *
 * The sched_getscheduler() function shall return the scheduling policy 
 * of the process specified by pid. If the value of pid is negative, the
 * behavior of the sched_getscheduler() function is unspecified.
 *
 * The values that can be returned by sched_getscheduler() are defined 
 * in the <sched.h> header.
 *
 * If a process specified by pid exists, and if the calling process has 
 * permission, the scheduling policy shall be returned for the process 
 * whose process ID is equal to pid.
 *
 * If pid is zero, the scheduling policy shall be returned for the calling 
 * process.
 *
 * RETURN VALUE
 *
 * Upon successful completion, the sched_getscheduler() function shall return
 * the scheduling policy of the specified process. If unsuccessful, the 
 * function shall return -1 and set errno to indicate the error.
 *
 * ERRORS
 *
 * The sched_getscheduler() function shall fail if:
 *
 * [EPERM]
 *      The requesting process does not have permission to determine the 
 *      scheduling policy of the specified process.
 *
 * [ESRCH]
 *      No process can be found corresponding to that specified by pid.
 */
int sched_getscheduler
    (
    pid_t pid
    )
    {
    return SCHED_FIFO;
    }
/*
 *
 * NAME
 *
 * sched_setscheduler - set scheduling policy and parameters (REALTIME)
 *
 * SYNOPSIS
 *
 * #include <sched.h>
 *
 * int sched_setscheduler(pid_t pid, int policy,
 *      const struct sched_param *param); 
 *
 * DESCRIPTION
 *
 * The sched_setscheduler() function shall set the scheduling policy and 
 * scheduling parameters of the process specified by pid to policy and the 
 * parameters specified in the sched_param structure pointed to by param, 
 * respectively. The value of the sched_priority member in the sched_param 
 * structure shall be any integer within the inclusive priority range for
 * the scheduling policy specified by policy. If the value of pid is negative, 
 * the behavior of the sched_setscheduler() function is unspecified.
 *
 * The possible values for the policy parameter are defined in the <sched.h> 
 * header.
 *
 * If a process specified by pid exists, and if the calling process has 
 * permission, the scheduling policy and scheduling parameters shall be set
 * for the process whose process ID is equal to pid.
 *
 * If pid is zero, the scheduling policy and scheduling parameters shall be
 * set for the calling process.
 *
 * The conditions under which one process has appropriate privileges to change
 * the scheduling parameters of another process are implementation-defined.
 *
 * Implementations may require that the requesting process have permission to 
 * set its own scheduling parameters or those of another process. Additionally, 
 * implementation-defined restrictions may apply as to the appropriate 
 * privileges required to set the scheduling policy of the process, or the 
 * scheduling policy of another process, to a particular value.
 *
 * The sched_setscheduler() function shall be considered successful if it 
 * succeeds in setting the scheduling policy and scheduling parameters of the 
 * process specified by pid to the values specified by policy and the structure
 * pointed to by param, respectively.
 *
 * See Scheduling Policies for a description on how this function affects 
 * the scheduling of the threads within the target process.
 *
 * If the current scheduling policy for the target process is not SCHED_FIFO, 
 * SCHED_RR, [SS]  or SCHED_SPORADIC,   the result is implementation-defined; 
 * this case includes the SCHED_OTHER policy.
 *
 * [SS] The specified sched_ss_repl_period shall be greater than or equal to  
 * the specified sched_ss_init_budget for the function to succeed; if it is not,
 * then the function shall fail.
 *
 * The value of sched_ss_max_repl shall be within the inclusive range 
 * [1,{SS_REPL_MAX}] for the function to succeed; if not, the function shall
 * fail. It is unspecified whether the sched_ss_repl_period and 
 * sched_ss_init_budget values are stored as provided by this function or are
 * rounded to align with the resolution of the clock being used. 
 *
 * This function is not atomic with respect to other threads in the process.
 * Threads may continue to execute while this function call is in the process 
 * of changing the scheduling policy and associated scheduling parameters for
 * the underlying kernel-scheduled entities used by the process contention 
 * scope threads.
 *
 * RETURN VALUE
 *
 * Upon successful completion, the function shall return the former scheduling
 * policy of the specified process. If the sched_setscheduler() function fails
 * to complete successfully, the policy and scheduling parameters shall remain 
 * unchanged, and the function shall return a value of -1 and set errno to 
 * indicate the error.
 *
 * ERRORS
 *
 * The sched_setscheduler() function shall fail if:
 *
 * [EINVAL]
 *      The value of the policy parameter is invalid, or one or more of the
 *      parameters contained in param is outside the valid range for the 
 *      specified scheduling policy.
 *
 * [EPERM]
 *      The requesting process does not have permission to set either or both
 *      of the scheduling parameters or the scheduling policy of the specified
 *      process.
 *
 * [ESRCH]
 *      No process can be found corresponding to that specified by pid.
 */
int sched_setscheduler
    (
    pid_t pid,
    int policy,
    const struct sched_param *param
    )
    {
    return OK;
    }

/*
 * NAME
 *
 * sched_rr_get_interval - get execution time limits (REALTIME)
 *
 * SYNOPSIS
 *
 *  #include <sched.h>
 *
 * int sched_rr_get_interval(pid_t pid, struct timespec *interval); 
 *
 * DESCRIPTION
 *
 * The sched_rr_get_interval() function shall update the timespec structure
 * referenced by the interval argument to contain the current execution time
 * limit (that is, time quantum) for the process specified by pid. If pid 
 * is zero, the current execution time limit for the calling process shall 
 * be returned.
 *
 * RETURN VALUE
 *
 * If successful, the sched_rr_get_interval() function shall return zero. 
 * Otherwise, it shall return a value of -1 and set errno to indicate the error.
 *
 * ERRORS
 *
 * The sched_rr_get_interval() function shall fail if:
 *
 * [ESRCH]
 *      No process can be found corresponding to that specified by pid.
 */

int sched_rr_get_interval
    (
    pid_t pid, 
    struct timespec *interval
    )
    {
    return OK;
    }

/*
 * NAME
 *
 * sched_yield - yield the processor
 * 
 * SYNOPSIS
 *
 * #include <sched.h>
 *
 * int sched_yield(void);
 *
 * DESCRIPTION
 *
 * The sched_yield() function shall force the running thread to relinquish 
 * the processor until it again becomes the head of its thread list. It 
 * takes no arguments.
 *
 * RETURN VALUE
 *
 * The sched_yield() function shall return 0 if it completes successfully;
 * otherwise, it shall return a value of -1 and set errno to indicate the error.
 *
 * ERRORS
 *
 * No errors are defined.
 */
 
int sched_yield(void)
    {  
    ipl_t ipl;
    
    ipl = interrupts_disable();

    spinlock_lock(&kurrent->thread_lock);

    kurrent->state = STATE_READY;
    
    spinlock_unlock(&kurrent->thread_lock);

    interrupts_restore(ipl);
    
    reschedule();
    
    return OK;
    }
