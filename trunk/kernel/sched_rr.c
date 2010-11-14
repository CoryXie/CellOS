/* sched_rr.c - round-robin scheduling policy */

#include <os/sched_rr.h>

static sched_policy_t  sched_policy_rr;
static sched_runq_t    sched_runq_rr_system;
static sched_runq_t    sched_runq_rr_cpu[CONFIG_NR_CPUS];

/* Attach a cpu to the scheduling policy */
status_t  sched_rr_attach_cpu
    (
    sched_cpu_t * cpu
    )
    {
    char rqname[NAME_MAX];
    
    if (!cpu)
        {
        printk("Invalid parameter\n");
        return ERROR;
        }
    
    sprintf(rqname, "RR_RUNQ_CPU%d", cpu->cpu_idx);
    
    return sched_runq_init(&sched_runq_rr_cpu[cpu->cpu_idx],
                    rqname, 
                    &sched_policy_rr);
    }

/* Get the system runq of this scheduling policy */
sched_runq_t *  sched_rr_get_system_runq
    (
    void
    )
    {
    return &sched_runq_rr_system;
    }

/* Get the cpu runq of this scheduling policy */
sched_runq_t *  sched_rr_get_cpu_runq
    (
    sched_cpu_t * cpu
    )
    {
    if (!cpu)
        {
        printk("Invalid parameter\n");
        return ERROR;
        }
    
    if (sched_runq_rr_cpu[cpu->cpu_idx].magic != MAGIC_VALID)
        {
        printk("Invalid magic\n");
        return ERROR;
        }
    
    return &sched_runq_rr_cpu[cpu->cpu_idx];
    }

/* Enqueue a thread to run */
bool    sched_rr_thread_enqueue
    (
    sched_runq_t *    runq,         
    sched_thread_t *        thread,       
    bool              preempt   
    );
    {
    
    }

status_t sched_policy_rr_init (void)
    {
    sched_policy_t *policy = &sched_policy_rr;
    
    memset(&policy, 0, sizeof(sched_policy_t));

    policy->id = SCHED_RR;

    policy->precedence = 0;

    strncpy(policy->name, "SCHED_RR", NAME_MAX);

    policy->attach_cpu = sched_rr_attach_cpu;
    policy->attach_cpu_group = NULL;
    policy->detach_cpu = NULL;
    policy->detach_cpu_group = NULL;

    policy->get_system_runq = sched_rr_get_system_runq;
    policy->get_cpu_runq = sched_rr_get_cpu_runq;
    policy->get_cpu_group_runq = NULL;

    policy->thread_enqueue = NULL;
    policy->thread_dequeue = NULL;
    policy->thread_rmqueue = NULL;

    policy->get_sched_param = NULL;
    policy->set_sched_param = NULL;

    policy->sched_clock_tick = NULL;

    policy->preemption_check = NULL;

    policy->change_priority = NULL;

    policy->runq_head_compare = NULL;

    sched_runq_init(&sched_runq_rr_system,
                    "RR_RUNQ_SYS", 
                    policy);

    memset(sched_runq_rr_cpu,
           0,
           sizeof(sched_runq_t) * CONFIG_NR_CPUS);
    
    return OK;
    };

