/* sched_rr.c - round-robin scheduling policy */

/*
  SCHED_RR
  
  Conforming implementations shall include a scheduling policy called the 
  "round robin" scheduling policy. This policy shall be identical to the 
  SCHED_RR policy with the additional condition that when the implementation 
  detects that a running thread has been executing as a running thread for 
  a time period of the length returned by the sched_rr_get_interval() function 
  or longer, the thread shall become the tail of its thread list and the 
  head of that thread list shall be removed and made a running thread.
  
  The effect of this policy is to ensure that if there are multiple SCHED_RR
  threads at the same priority, one of them does not monopolize the processor.
  An application should not rely only on the use of SCHED_RR to ensure 
  application progress among multiple threads if the application includes 
  threads using the SCHED_RR policy at the same or higher priority levels or
  SCHED_RR threads at a higher priority level.
  
  A thread under this policy that is preempted and subsequently resumes 
  execution as a running thread completes the unexpired portion of its round 
  robin interval time period.
  
  For this policy, valid priorities shall be within the range returned by the 
  sched_get_priority_max() and sched_get_priority_min() functions when 
  SCHED_RR is provided as the parameter. Conforming implementations shall 
  provide a priority range of at least 32 priorities for this policy.
*/

#include <os/sched_rr.h>

#define SCHED_RR_PRIO_MIN     (0)
#define SCHED_RR_PRIO_MAX     (63)
#define SCHED_RR_PRIO_COUNT   (SCHED_RR_PRIO_MAX - SCHED_RR_PRIO_MIN + 1)

/* Runq structure for this scheduling policy */
typedef struct sched_rr_runq
    {
    sched_runq_t runq; /* Common runq interface - must be first */
    qhead_t prio_array[SCHED_RR_PRIO_COUNT];  /* Priority array */
    int best_priority; /* The best priority (lowest value) */
    id_t owner_id;     /* The ID of the owner (cpu or cpu_group, or system) */
    list_t node;       /* Runq list node */
    }sched_rr_runq_t;

sched_policy_t       sched_policy_rr;
static sched_rr_runq_t    sched_runq_rr_system;
static sched_rr_runq_t    sched_runq_rr_cpu[CONFIG_NR_CPUS];
/* List of runqs for the cpu_groups */
static list_t sched_rr_cpu_group_runq_list;
static spinlock_t sched_rr_cpu_group_runq_list_lock;

#define SCHED_RR_CPU_GROUP_RUNQ_LOCK()    \
    spinlock_lock(&sched_rr_cpu_group_runq_list_lock)
    
#define SCHED_RR_CPU_GROUP_RUNQ_UNLOCK()  \
    spinlock_unlock(&sched_rr_cpu_group_runq_list_lock)

/* 
 * Cast the opaque schedule parameter area to the specific 
 * paramter structure of this scheduling policy
 */
#define RR_SCHED_PARAM(thread)    \
    (sched_rr_param_t *)((thread)->sched_param_area)

#define RR_SCHED_RUNQ(runq)    \
    (sched_rr_runq_t *)(runq)

/* Initialize the runq structure */
status_t sched_rr_runq_init
    (
    sched_rr_runq_t * runq,
    id_t owner_id,
    char * name
    )
    {
    int i;
    char qname[NAME_MAX];

    memset(runq, 0, sizeof(sched_rr_runq_t));
    
    sched_runq_init(&runq->runq,
                    name, 
                    &sched_policy_rr);
    
    for (i = 0; i < SCHED_RR_PRIO_COUNT; i++)
        {
        snprintf(qname, NAME_MAX, "%s_PRIO%d", name, i);
        
        queue_init(&runq->prio_array[i], qname, FALSE);
        }

    list_init(&runq->node);
    
    runq->owner_id = owner_id;
    
    return OK;
    }

/* Attach a cpu to the scheduling policy */
status_t  sched_rr_attach_cpu_group
    (
    cpu_set_t * cpu_set
    )
    {
    sched_cpu_group_t * cpu_group = sched_cpu_group_find(cpu_set); 
    sched_rr_runq_t * cpu_group_runq = NULL;
    char rqname[NAME_MAX];
    
    /*
     * If we have found the cpu_group, then we MAY have created a
     * runq for that cpu_group, so use it on the way!
     */
    if (cpu_group)
        {
        /* Check if we already have a runq for this cpu_group */

        SCHED_RR_CPU_GROUP_RUNQ_LOCK();
        
        cpu_group_runq = NULL;
        LIST_FOREACH(&sched_rr_cpu_group_runq_list,iter)
            {
            cpu_group_runq = LIST_ENTRY(iter, sched_rr_runq_t, node);
            if (cpu_group_runq->owner_id == cpu_group->cpu_group_id)
                break;
            cpu_group_runq = NULL;
            }

        SCHED_RR_CPU_GROUP_RUNQ_UNLOCK();

        /* 
         * We have found an already created runq for this cpu_group, 
         * so return here.
         */
        if (cpu_group_runq)
            return OK;

        /* Otherwise, we have to create a runq for this cpu_group */
        }
    
    /* If we do not have a usable cpu_group, create one! */
    if (!cpu_group)
        {
        cpu_group = sched_cpu_group_add(cpu_set); 

        if (!cpu_group)
            {
            printk("Could not create cpu_group\n");
            return ERROR;
            }
        }

    cpu_group_runq = kmalloc(sizeof(sched_rr_runq_t));

    if (!cpu_group_runq)
        {
        printk("Could not create cpu_group_runq\n");
        return ERROR;
        }
    
    sprintf(rqname, "RR_RUNQ_CPU_GROUP%d", cpu_group->cpu_group_id);

    sched_rr_runq_init(cpu_group_runq, cpu_group->cpu_group_id, rqname);
    
    SCHED_RR_CPU_GROUP_RUNQ_LOCK();

    list_append(&sched_rr_cpu_group_runq_list,
                &cpu_group_runq->node);
    
    SCHED_RR_CPU_GROUP_RUNQ_UNLOCK();

    return OK;
    }

/* Get the system runq of this scheduling policy */
sched_runq_t *  sched_rr_get_sys_runq
    (
    void
    )
    {
    return &sched_runq_rr_system.runq;
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
        return NULL;
        }
    
    if (sched_runq_rr_cpu[cpu->cpu_idx].runq.magic != MAGIC_VALID)
        {
        printk("Invalid magic\n");
        return NULL;
        }
    
    return &sched_runq_rr_cpu[cpu->cpu_idx].runq;
    }

/* Get the cpu group runq of this scheduling policy */
struct sched_runq *  sched_rr_get_cpu_group_runq
    (
    struct sched_cpu_group * cpu_group
    )
    {
    sched_rr_runq_t * cpu_group_runq = NULL;
    
    /* Check if we already have a runq for this cpu_group */
    
    SCHED_RR_CPU_GROUP_RUNQ_LOCK();
    
    cpu_group_runq = NULL;
    
    LIST_FOREACH(&sched_rr_cpu_group_runq_list,iter)
        {
        cpu_group_runq = LIST_ENTRY(iter, sched_rr_runq_t, node);
        
        if (cpu_group_runq->owner_id == cpu_group->cpu_group_id)
            break;
        
        cpu_group_runq = NULL;
        }
    
    SCHED_RR_CPU_GROUP_RUNQ_UNLOCK();

    return (struct sched_runq *)cpu_group_runq;
    }

/* Enqueue a thread to run */
bool    sched_rr_thread_enqueue
    (
    sched_runq_t *    runq,         
    sched_thread_t *  thread,       
    bool              preemptable   
    )
    {
    sched_rr_param_t * sched_param = RR_SCHED_PARAM(thread);
    sched_rr_runq_t * sched_runq = RR_SCHED_RUNQ(runq);
    qhead_t * prioq;
    
    /* Lock the runq */
    SCHED_RUNQ_LOCK(runq);

    /* Make sure the priority is in valid range */
    if (sched_param->sched_priority < SCHED_RR_PRIO_MIN)
        sched_param->sched_priority = SCHED_RR_PRIO_MIN;
    else if (sched_param->sched_priority > SCHED_RR_PRIO_MAX)
        sched_param->sched_priority = SCHED_RR_PRIO_MAX;

    /* Get the priority queue */
    prioq = &sched_runq->prio_array[sched_param->sched_priority];

    /*
     * SCHED_RR:
     * 
     * When a blocked thread becomes a runnable thread, 
     * it becomes the tail of the thread list for its 
     * priority.
     */
    enqueue(prioq, &thread->runq_node, FALSE);

    /* Record the best priority (with max priority value) */
    if ((sched_param->sched_priority > sched_runq->best_priority) ||
        (sched_runq->runq.runnable == 0))
        sched_runq->best_priority = sched_param->sched_priority;

    sched_runq->runq.runnable++;

    /* Unlock the runq */
    SCHED_RUNQ_UNLOCK(runq);

    if (!preemptable)
        {        
        return FALSE;
        }

    /* 
     * Return if the enqueued thread could preempt 
     * the current thread
     */
    return (sched_param->sched_priority > 
            (RR_SCHED_PARAM(kurrent))->sched_priority);
    }

/* Get the next thread to run */
sched_thread_t *  sched_rr_thread_dequeue
    (
    sched_runq_t *    runq          
    )
    {
    sched_rr_runq_t * sched_runq = RR_SCHED_RUNQ(runq);
    sched_thread_t * thread = NULL;
    qhead_t * prioq;
    int prio;
    
    /* Lock the runq */
    SCHED_RUNQ_LOCK(runq);

    if (sched_runq->runq.runnable == 0)
        {
        /* Unlock the runq */
        SCHED_RUNQ_UNLOCK(runq);
        
        return NULL;
        }
    
    for (prio = sched_runq->best_priority; 
         prio >= SCHED_RR_PRIO_MIN; 
         prio++)
        {
        prioq = &sched_runq->prio_array[prio];

        if (!queue_empty(prioq))
            {
            qelement_t * entry = dequeue(prioq, TRUE);
            
            thread = queue_entry(entry, 
                                 sched_thread_t, 
                                 runq_node);
            
            sched_runq->runq.runnable--;
            
            break;
            }
        }
         
    /* Unlock the runq */
    SCHED_RUNQ_UNLOCK(runq);

    return thread;
    }

/* Remove thread from a run queue */
void sched_rr_thread_rmqueue
    (
    sched_runq_t *   runq,          
    sched_thread_t *       thread         
    )
    {
    /* Lock the runq */
    SCHED_RUNQ_LOCK(runq);

    queue_remove(&thread->runq_node, TRUE);
    
    /* Unlock the runq */
    SCHED_RUNQ_UNLOCK(runq);
    }

/* 
 * Check if the specified thread can be preempted 
 * by the best thread on the runq 
 */
bool sched_rr_preemption_check
    (
    sched_runq_t *   runq,
    sched_thread_t * thread
    )
    {
    /* If the thread is NULL. return TRUE to cause a runq dequeue */
    if (thread == NULL)
        return TRUE;
    
    sched_rr_param_t * sched_param = RR_SCHED_PARAM(thread);
    sched_rr_runq_t * sched_runq = RR_SCHED_RUNQ(runq);
    bool preempt = FALSE;
    
    if (thread->sched_policy_id != sched_policy_rr.id)
        return FALSE;

    /* Lock the runq */
    SCHED_RUNQ_LOCK(runq);

    if (sched_runq->runq.runnable == 0)
        preempt = FALSE;
    else
        preempt = (sched_runq->best_priority > 
                    sched_param->sched_priority);
        
    /* Unlock the runq */
    SCHED_RUNQ_UNLOCK(runq);
    
    return preempt;
    }

/* Compare precedence of threads with in this scheduling policy */
bool sched_rr_thread_precedence_compare
    (
    struct sched_thread *   thread1,
    struct sched_thread *   thread2
    )
    {
    sched_rr_param_t * sched_param1 = RR_SCHED_PARAM(thread1);
    sched_rr_param_t * sched_param2 = RR_SCHED_PARAM(thread2);
    
    return sched_param1->sched_priority > sched_param2->sched_priority;
    }

/* Priority update */
void sched_rr_change_priority
    (
    struct sched_thread *   thread,
    int                     prio
    )
    {
    return;
    }

/* Get thread current priority */
int sched_rr_get_priority
    (
    struct sched_thread *   thread
    )
    {
    sched_rr_param_t * sched_param = RR_SCHED_PARAM(thread);

    return sched_param->sched_priority;
    }

/* Scheduling decisions at periodic clock tick */
BOOL sched_rr_sched_clock_tick
    (
    struct sched_thread * thread
    )
    {    
    thread->remain_time_slice--;

    if (thread->remain_time_slice <= 0)
        {
        thread->remain_time_slice = thread->sched_time_slice;

        thread->state = STATE_READY;

        return TRUE;
        }

    return FALSE;
    }

/* Get the schedule parameter */
status_t sched_rr_get_sched_param
    (
    struct sched_thread *       thread, 
    sched_param_t *  param
    )
    {
    return OK;
    }

/* Set the schedule parameter */
status_t sched_rr_set_sched_param
    (
    struct sched_thread *       thread, 
    sched_param_t *  param
    )
    {
    return OK;
    }

/* Initialize this scheduling policy */
status_t sched_policy_rr_init (void)
    {
    int idx;
    
    sched_policy_t *policy = &sched_policy_rr;
    
    memset(policy, 0, sizeof(sched_policy_t));

    policy->id = SCHED_RR;

    policy->precedence = 0;
    policy->priority_min = SCHED_RR_PRIO_MIN;
    policy->priority_max = SCHED_RR_PRIO_MAX;
    
    strncpy(policy->name, "SCHED_RR", NAME_MAX);

    policy->attach_cpu_group = sched_rr_attach_cpu_group;
    policy->detach_cpu_group = NULL;

    policy->get_sys_runq = sched_rr_get_sys_runq;
    policy->get_cpu_runq = sched_rr_get_cpu_runq;
    policy->get_cpu_group_runq = sched_rr_get_cpu_group_runq;

    policy->thread_enqueue = sched_rr_thread_enqueue;
    policy->thread_dequeue = sched_rr_thread_dequeue;
    policy->thread_rmqueue = sched_rr_thread_rmqueue;

    policy->get_sched_param = NULL;
    policy->set_sched_param = NULL;

    policy->sched_clock_tick = sched_rr_sched_clock_tick;

    policy->preemption_check = sched_rr_preemption_check;

    policy->change_priority = sched_rr_change_priority;
    policy->get_priority = sched_rr_get_priority;
    
    policy->runq_head_compare = NULL;
    
    policy->thread_precedence_compare = sched_rr_thread_precedence_compare;
    
    sched_rr_runq_init(&sched_runq_rr_system, 0, 
                         "RR_RUNQ_SYS");

    memset(sched_runq_rr_cpu,
           0,
           sizeof(sched_rr_runq_t) * CONFIG_NR_CPUS);

    for (idx = 0; idx < CONFIG_NR_CPUS; idx++)
        {
        char rqname[NAME_MAX];
            
        sprintf(rqname, "RR_RUNQ_CPU%d", idx);
        
        sched_rr_runq_init(&sched_runq_rr_cpu[idx], idx,
                        rqname);
        }
        
    list_init(&sched_rr_cpu_group_runq_list);
    
    spinlock_init(&sched_rr_cpu_group_runq_list_lock);
    
    return OK;
    };


