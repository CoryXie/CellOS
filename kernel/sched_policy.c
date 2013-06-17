#include <sys.h>
#include <arch.h>
#include <os.h>

static sched_policy_t * sched_policy_array[] =
    {
    &sched_policy_rr,
    &sched_policy_fifo
    };

int sched_policy_count = NELEMENTS(sched_policy_array);

/* Initialize the schedule policies */
status_t sched_policy_init(void)
    {
    sched_policy_fifo_init();
    sched_policy_rr_init();

    return OK;
    
    /* Add new schedule policy initialization here...*/
    }

/* Final intialization for schedule policies */
status_t sched_policy_post_init (void)
    {
    int idx;
    sched_policy_t * policy = NULL;
    
    for (idx = 0; idx < sched_policy_count; idx++)
        {
        policy = sched_policy_array[idx];
        
        if (policy)
            {
            policy->precedence = idx;
            }
        }
    return OK;
    }

/* Get the scheduling policy by ID */
sched_policy_t * sched_policy_get_by_id
    (
    int policy_id
    )
    {
    int idx;
    sched_policy_t * policy = NULL;
    
    for (idx = 0; idx < sched_policy_count; idx++)
        {
        policy = sched_policy_array[idx];
        
        if (policy && (policy->id == policy_id))
            {
            break;
            }
            
        policy = NULL;
        }
    
    return policy;
    }

sched_thread_t * sched_policy_find_best_thread 
    (
    sched_policy_t * policy,
    sched_thread_t * check
    )
    {
    sched_runq_t * runq;
    sched_thread_t * best_thread;
    sched_cpu_group_t *cpu_group;
    sched_cpu_t * cpu = current_cpus[this_cpu()];

    /* Find the local cpu runq */
    runq = policy->get_cpu_runq(cpu);

    /* Check if the local cpu has a better thread to run */
    if (policy->preemption_check(runq, check))
        {
        best_thread = policy->thread_dequeue(runq);

        if (best_thread) 
            return best_thread;
        }
    
    /* Check if the cpu_group has a better thread to run */
    best_thread = sched_cpu_group_find_best_thread(policy, cpu, check);

    if (best_thread) 
        return best_thread;

    /* Find the system runq */
    runq = policy->get_sys_runq();

    /* Check if the system has a better thread to run */
    if (policy->preemption_check(runq, check))
        {
        best_thread = policy->thread_dequeue(runq);

        if (best_thread) 
            return best_thread;
        }

    return NULL;
    }

struct sched_thread * sched_find_best_thread 
    (
    sched_thread_t * check
    )
    {
    int idx;
    sched_policy_t * policy;
    sched_thread_t * best_thread;
    
    for (idx = 0; idx < sched_policy_count; idx++)
        {
        policy = sched_policy_array[idx];
        
        if (policy)
            {
            best_thread = sched_policy_find_best_thread(policy, check);
            
            if (best_thread)
                {
                return best_thread;
                }
            }
        }
    
    return NULL;
    }
