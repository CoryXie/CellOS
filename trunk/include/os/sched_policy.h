/* sched_policy.h - scheduling policy management */

#ifndef _OS_SCHED_CLASS_H
#define _OS_SCHED_CLASS_H

#include <sys.h>
#include <arch.h>
#include <sched.h>
#include <os/sched_core.h>

struct sched_cpu;
struct sched_cpu_group;
struct sched_runq;
struct sched_thread;
struct cpu_set;


/*
 * Scheduling policy definition.
 *
 * Each policy module should maintain a sched_runq for the
 * attached cpu and cpu_group seperately. And by default,
 * it should maintain a sched_runq for the system. 
 *
 * The runqs for the each cpu and the system are created at
 * initialization time of the scheduling policy, but the runq
 * for the cpu_group are created dynamically when cpu_group 
 * is created (which is dynamical since users will create 
 * threads with user defined cpu_set as the CPU affinity setting.).
 */
 
typedef struct sched_policy
    {
    /* List node in the scheduling policy list */
    
    list_t  node;
    
    /* Scheduling policy name */
    
    char    name[NAME_MAX];
    
    /* Scheduling policy ID */
    int     id;

    /* Scheduling policy precedence */
    int     precedence;

    /* Minimun priority limit */
    int     priority_min;
    
    /* Maximun priority limit */
    int     priority_max;
    
    /* Attach a cpu group to the scheduling policy */
    status_t        (*attach_cpu_group)
        (
        struct cpu_set * cpu_set
        );
    
    /* Detach a cpu group from the scheduling policy */
    status_t        (*detach_cpu_group)
        (
        struct cpu_set * cpu_set
        );
    
    /* Get the system runq of this scheduling policy */
    struct sched_runq *  (*get_sys_runq)
        (
        void
        );
    
    /* Get the cpu runq of this scheduling policy */
    struct sched_runq *  (*get_cpu_runq)
        (
        struct sched_cpu * cpu
        );

    /* Get the cpu group runq of this scheduling policy */
    struct sched_runq *  (*get_cpu_group_runq)
        (
        struct sched_cpu_group * cpu_group
        );

    /* Enqueue a thread to run */
    bool    (*thread_enqueue)
        (
        struct sched_runq *         runq,         
        struct sched_thread *       thread,       
        bool                        preempt   
        ); 
    
    /* Get the next thread to run */
    struct sched_thread *  (*thread_dequeue)
        (
        struct sched_runq *         runq          
        );          
                                        
    /* Remove thread from a run queue */
    void    (*thread_rmqueue)
        (
        struct sched_runq *         runq,          
        struct sched_thread *       thread         
        );        

    /* Get the schedule parameter */
    status_t (*get_sched_param)
        (
        struct sched_thread *       thread, 
        sched_param_t *  param
        );
    
    /* Set the schedule parameter */
    status_t (*set_sched_param)
        (
        struct sched_thread *       thread, 
        sched_param_t *  param
        );
    
    /* 
     * Check if the specified thread can be preempted 
     * by the best thread on the runq 
     */
    BOOL    (*preemption_check)
        (
        struct sched_runq *   runq,
        struct sched_thread * thread
        );

    /* Scheduling decisions at periodic clock tick */
    BOOL    (*sched_clock_tick)
        (
        struct sched_thread * thread
        );

    /* Priority update */
    void (*change_priority)
        (
        struct sched_thread *   thread,
        int                     prio
        );

    /* Get thread current priority */
    int (*get_priority)
        (
        struct sched_thread *   thread
        );

    /* Compare threads at the heads of two run queues */
    bool    (*runq_head_compare)
        (
        struct sched_runq *      runq1,
        struct sched_runq *      runq2
        );
    
    /* Compare precedence of threads with in this scheduling policy */
    bool    (*thread_precedence_compare)
        (
        struct sched_thread *   thread1,
        struct sched_thread *   thread2
        );
    }sched_policy_t;

#define SCHED_POLICY_PRECEDENCE_COUNT   64

/* Check if the precedence range is valid */
#define SCHED_POLICY_PRECEDENCE_VALID(policy1) \
    ((policy1) && \
     ((policy1)->precedence >= 0) && \
     ((policy1)->precedence < SCHED_POLICY_PRECEDENCE_COUNT))
    
/* TRUE if the policy1 has better precedence than policy2 */
#define SCHED_POLICY_PRECEDENCE_COMPARE(policy1, policy2)\
    ((policy1)->precedence < (policy1)->precedence)

/* TRUE if the thread1 has better precedence than thread2 */
#define SCHED_THREAD_PRECEDENCE_COMPARE(thread1, thread2)\
    (((thread1)->sched_policy->precedence < (thread1)->sched_policy->precedence) ? \
      TRUE :                                             \
     (thread1)->sched_policy->thread_precedence_compare((thread1), (thread2)))

status_t sched_policy_init (void);

status_t sched_policy_post_init (void);

sched_policy_t * sched_policy_get_by_id
    (
    int policy_id
    );

struct sched_thread * sched_find_best_thread 
    (
    struct sched_thread * check
    );

#endif /* _OS_SCHED_CLASS_H */
