/* sched_fifo.c - round-robin scheduling policy */

/*
SCHED_FIFO

    Conforming implementations shall include a scheduling policy 
    called the FIFO scheduling policy.
    
    Threads scheduled under this policy are chosen from a thread 
    list that is ordered by the time its threads have been on the
    list without being executed; generally, the head of the list
    is the thread that has been on the list the longest time, and 
    the tail is the thread that has been on the list the shortest 
    time.
    
    Under the SCHED_FIFO policy, the modification of the definitional
    thread lists is as follows:
    
    1. When a running thread becomes a preempted thread, it becomes 
    the head of the thread list for its priority.
    
    2. When a blocked thread becomes a runnable thread, it becomes 
    the tail of the thread list for its priority.
    
    3. When a running thread calls the sched_setscheduler( ) function,
    the process specified in the function call is modified to the 
    specified policy and the priority specified by the param argument.
    
    4. When a running thread calls the sched_setparam( ) function,
    the priority of the process specified in the function call is 
    modified to the priority specified by the param argument.
    
    5. When a running thread calls the pthread_setschedparam( ) 
    function, the thread specified in the function call is modified
    to the specified policy and the priority specified by the param
    argument.
    
    6. When a running thread calls the pthread_setschedprio( ) 
    function, the thread specified in the function call is modified
    to the priority specified by the prio argument.
    
    7. If a thread whose policy or priority has been modified other
    than by pthread_setschedprio( ) is a running thread or is 
    runnable, it then becomes the tail of the thread list for its
    new priority.
    
    8. If a thread whose priority has been modified by 
    pthread_setschedprio( ) is a running thread or is runnable, the 
    effect on its position in the thread list depends on the 
    direction of the modification, as follows:
    
        a. If the priority is raised, the thread becomes the tail
           of the thread list.
           
        b. If the priority is unchanged, the thread does not change 
           position in the thread list.
           
        c. If the priority is lowered, the thread becomes the head 
           of the thread list.
           
    9. When a running thread issues the sched_yield( ) function, 
    the thread becomes the tail of the thread list for its priority.
    
    10. At no other time is the position of a thread with this 
    scheduling policy within the thread lists affected.
    
    For this policy, valid priorities shall be within the range 
    returned by the sched_get_priority_max( ) and 
    sched_get_priority_min( ) functions when SCHED_FIFO is provided
    as the parameter.
    
    Conforming implementations shall provide a priority range of
    at least 32 priorities for this policy.
*/
#include <os/sched_fifo.h>

#define SCHED_FIFO_PRIO_MIN     (0)
#define SCHED_FIFO_PRIO_MAX     (63)
#define SCHED_FIFO_PRIO_COUNT   (SCHED_FIFO_PRIO_MAX - SCHED_FIFO_PRIO_MIN + 1)

/* Runq structure for this scheduling policy */
typedef struct sched_fifo_runq
    {
    sched_runq_t runq; /* Common runq interface - must be first */
    qhead_t prio_array[SCHED_FIFO_PRIO_COUNT];  /* Priority array */
    int best_priority; /* The best priority (lowest value) */
    id_t owner_id;     /* The ID of the owner (cpu or cpu_group, or system) */
    list_t node;       /* Runq list node */
    }sched_fifo_runq_t;

sched_policy_t       sched_policy_fifo;
static sched_fifo_runq_t    sched_runq_fifo_system;
static sched_fifo_runq_t    sched_runq_fifo_cpu[CONFIG_NR_CPUS];
/* List of runqs for the cpu_groups */
static list_t sched_fifo_cpu_group_runq_list;
static spinlock_t sched_fifo_cpu_group_runq_list_lock;

#define SCHED_FIFO_CPU_GROUP_RUNQ_LOCK()    \
    spinlock_lock(&sched_fifo_cpu_group_runq_list_lock)
    
#define SCHED_FIFO_CPU_GROUP_RUNQ_UNLOCK()  \
    spinlock_unlock(&sched_fifo_cpu_group_runq_list_lock)

/* 
 * Cast the opaque schedule parameter area to the specific 
 * paramter structure of this scheduling policy
 */
#define FIFO_SCHED_PARAM(thread)    \
    (sched_fifo_param_t *)((thread)->sched_param_area)

#define FIFO_SCHED_RUNQ(runq)    \
    (sched_fifo_runq_t *)(runq)

/* Initialize the runq structure */
status_t sched_fifo_runq_init
    (
    sched_fifo_runq_t * runq,
    id_t owner_id,
    char * name
    )
    {
    int i;
    char qname[NAME_MAX];

    memset(runq, 0, sizeof(sched_fifo_runq_t));
    
    sched_runq_init(&runq->runq,
                    name, 
                    &sched_policy_fifo);
    
    for (i = 0; i < SCHED_FIFO_PRIO_COUNT; i++)
        {
        snprintf(qname, NAME_MAX, "%s_PRIO%d", name, i);
        
        queue_init(&runq->prio_array[i], qname, FALSE);
        }

    list_init(&runq->node);
    
    runq->owner_id = owner_id;
    
    return OK;
    }

/* Attach a cpu to the scheduling policy */
status_t  sched_fifo_attach_cpu_group
    (
    cpu_set_t * cpu_set
    )
    {
    sched_cpu_group_t * cpu_group = sched_cpu_group_find(cpu_set); 
    sched_fifo_runq_t * cpu_group_runq = NULL;
    char rqname[NAME_MAX];
    
    /*
     * If we have found the cpu_group, then we MAY have created a
     * runq for that cpu_group, so use it on the way!
     */
    if (cpu_group)
        {
        /* Check if we already have a runq for this cpu_group */

        SCHED_FIFO_CPU_GROUP_RUNQ_LOCK();
        
        cpu_group_runq = NULL;
        LIST_FOREACH(&sched_fifo_cpu_group_runq_list,iter)
            {
            cpu_group_runq = LIST_ENTRY(iter, sched_fifo_runq_t, node);
            if (cpu_group_runq->owner_id == cpu_group->cpu_group_id)
                break;
            cpu_group_runq = NULL;
            }

        SCHED_FIFO_CPU_GROUP_RUNQ_UNLOCK();

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

    cpu_group_runq = kmalloc(sizeof(sched_fifo_runq_t));

    if (!cpu_group_runq)
        {
        printk("Could not create cpu_group_runq\n");
        return ERROR;
        }
    
    sprintf(rqname, "FIFO_RUNQ_CPU_GROUP%d", cpu_group->cpu_group_id);

    sched_fifo_runq_init(cpu_group_runq, cpu_group->cpu_group_id, rqname);
    
    SCHED_FIFO_CPU_GROUP_RUNQ_LOCK();

    list_append(&sched_fifo_cpu_group_runq_list,
                &cpu_group_runq->node);
    
    SCHED_FIFO_CPU_GROUP_RUNQ_UNLOCK();

    return OK;
    }

/* Get the system runq of this scheduling policy */
sched_runq_t *  sched_fifo_get_sys_runq
    (
    void
    )
    {
    return &sched_runq_fifo_system.runq;
    }

/* Get the cpu runq of this scheduling policy */
sched_runq_t *  sched_fifo_get_cpu_runq
    (
    sched_cpu_t * cpu
    )
    {
    if (!cpu)
        {
        printk("Invalid parameter\n");
        return NULL;
        }
    
    if (sched_runq_fifo_cpu[cpu->cpu_idx].runq.magic != MAGIC_VALID)
        {
        printk("Invalid magic\n");
        return NULL;
        }
    
    return &sched_runq_fifo_cpu[cpu->cpu_idx].runq;
    }

/* Get the cpu group runq of this scheduling policy */
struct sched_runq *  sched_fifo_get_cpu_group_runq
    (
    struct sched_cpu_group * cpu_group
    )
    {
    sched_fifo_runq_t * cpu_group_runq = NULL;
    
    /* Check if we already have a runq for this cpu_group */
    
    SCHED_FIFO_CPU_GROUP_RUNQ_LOCK();
    
    cpu_group_runq = NULL;
    
    LIST_FOREACH(&sched_fifo_cpu_group_runq_list,iter)
        {
        cpu_group_runq = LIST_ENTRY(iter, sched_fifo_runq_t, node);
        
        if (cpu_group_runq->owner_id == cpu_group->cpu_group_id)
            break;
        
        cpu_group_runq = NULL;
        }
    
    SCHED_FIFO_CPU_GROUP_RUNQ_UNLOCK();

    return (struct sched_runq *)cpu_group_runq;
    }

/* Enqueue a thread to run */
bool    sched_fifo_thread_enqueue
    (
    sched_runq_t *    runq,         
    sched_thread_t *  thread,       
    bool              preemptable   
    )
    {
    sched_fifo_param_t * sched_param = FIFO_SCHED_PARAM(thread);
    sched_fifo_runq_t * sched_runq = FIFO_SCHED_RUNQ(runq);
    qhead_t * prioq;
    
    /* Lock the runq */
    SCHED_RUNQ_LOCK(runq);

    /* Make sure the priority is in valid range */
    if (sched_param->sched_priority < SCHED_FIFO_PRIO_MIN)
        sched_param->sched_priority = SCHED_FIFO_PRIO_MIN;
    else if (sched_param->sched_priority > SCHED_FIFO_PRIO_MAX)
        sched_param->sched_priority = SCHED_FIFO_PRIO_MAX;

    /* Get the priority queue */
    prioq = &sched_runq->prio_array[sched_param->sched_priority];

    /*
     * SCHED_FIFO:
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
            (FIFO_SCHED_PARAM(kurrent))->sched_priority);
    }

/* Get the next thread to run */
sched_thread_t *  sched_fifo_thread_dequeue
    (
    sched_runq_t *    runq          
    )
    {
    sched_fifo_runq_t * sched_runq = FIFO_SCHED_RUNQ(runq);
    sched_thread_t * thread = NULL;
    qhead_t * prioq;
    int prio;
    
    /* Lock the runq */
    SCHED_RUNQ_LOCK(runq);

    if (sched_runq->runq.runnable == 0)
        {
        /* Unlock the runq */
        SCHED_RUNQ_UNLOCK(runq);

        //printk("No runnable thread!\n");
        
        return NULL;
        }
    
    for (prio = sched_runq->best_priority; 
         prio >= SCHED_FIFO_PRIO_MIN; 
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
void sched_fifo_thread_rmqueue
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
bool sched_fifo_preemption_check
    (
    sched_runq_t *   runq,
    sched_thread_t * thread
    )
    {
    /* If the thread is NULL. return TRUE to cause a runq dequeue */
    if (thread == NULL)
        return TRUE;
    
    sched_fifo_param_t * sched_param = FIFO_SCHED_PARAM(thread);
    sched_fifo_runq_t * sched_runq = FIFO_SCHED_RUNQ(runq);
    bool preempt = FALSE;
    
    if (thread->sched_policy_id != sched_policy_fifo.id)
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
bool sched_fifo_thread_precedence_compare
    (
    struct sched_thread *   thread1,
    struct sched_thread *   thread2
    )
    {
    sched_fifo_param_t * sched_param1 = FIFO_SCHED_PARAM(thread1);
    sched_fifo_param_t * sched_param2 = FIFO_SCHED_PARAM(thread2);
    
    return sched_param1->sched_priority > sched_param2->sched_priority;
    }

/* Initialize this scheduling policy */
status_t sched_policy_fifo_init (void)
    {
    int idx;
    
    sched_policy_t *policy = &sched_policy_fifo;
    
    memset(policy, 0, sizeof(sched_policy_t));

    policy->id = SCHED_FIFO;

    policy->precedence = 0;
    policy->priority_min = SCHED_FIFO_PRIO_MIN;
    policy->priority_max = SCHED_FIFO_PRIO_MAX;
    
    strncpy(policy->name, "SCHED_FIFO", NAME_MAX);

    policy->attach_cpu_group = sched_fifo_attach_cpu_group;
    policy->detach_cpu_group = NULL;

    policy->get_sys_runq = sched_fifo_get_sys_runq;
    policy->get_cpu_runq = sched_fifo_get_cpu_runq;
    policy->get_cpu_group_runq = sched_fifo_get_cpu_group_runq;

    policy->thread_enqueue = sched_fifo_thread_enqueue;
    policy->thread_dequeue = sched_fifo_thread_dequeue;
    policy->thread_rmqueue = sched_fifo_thread_rmqueue;

    policy->get_sched_param = NULL;
    policy->set_sched_param = NULL;

    policy->sched_clock_tick = NULL;

    policy->preemption_check = sched_fifo_preemption_check;

    policy->change_priority = NULL;

    policy->runq_head_compare = NULL;

    sched_fifo_runq_init(&sched_runq_fifo_system, 0, 
                         "FIFO_RUNQ_SYS");

    memset(sched_runq_fifo_cpu,
           0,
           sizeof(sched_fifo_runq_t) * CONFIG_NR_CPUS);

    for (idx = 0; idx < CONFIG_NR_CPUS; idx++)
        {
        char rqname[NAME_MAX];
            
        sprintf(rqname, "FIFO_RUNQ_CPU%d", idx);
        
        sched_fifo_runq_init(&sched_runq_fifo_cpu[idx], idx,
                        rqname);
        }
        
    list_init(&sched_fifo_cpu_group_runq_list);
    
    spinlock_init(&sched_fifo_cpu_group_runq_list_lock);
    
    return OK;
    };


