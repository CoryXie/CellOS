#include <sys.h>
#include <arch.h>
#include <os.h>

sched_cpu_t     cpus[CONFIG_NR_CPUS];
sched_cpu_t*    current_cpus[CONFIG_NR_CPUS];

static spinlock_t cpu_group_list_lock;
static list_t cpu_group_list;
static size_t cpu_group_cout = 0;
static id_t   cpu_group_id_next = 0;

#define SCHED_CPU_GROUP_LOCK()    \
    spinlock_lock(&cpu_group_list_lock)
    
#define SCHED_CPU_GROUP_UNLOCK()  \
    spinlock_unlock(&cpu_group_list_lock)
    
/* 
 * Initialize CPUs
 *
 * Initialize kernel CPUs support.
 */
 
void cpu_early_init(void) 
    {
	unsigned int i;
	
	/* initialize everything */
    
	memset(cpus, 0, sizeof(sched_cpu_t) * CONFIG_NR_CPUS);

	for (i = 0; i < CONFIG_NR_CPUS; i++) 
        {        
		spinlock_init(&cpus[i].lock);
        
        list_init(&cpus[i].cpu_group_list);
        
        current_cpus[i] = &cpus[i];
        current_cpus[i]->cpu_idx = i;
	    }
        
    spinlock_init(&cpu_group_list_lock);

    list_init(&cpu_group_list);
    }

sched_cpu_group_t * sched_cpu_group_find 
    (
    cpu_set_t * cpu_set
    )
    {
    sched_cpu_group_t * cpu_group = NULL;

    /* Lock the cpu group list */
    SCHED_CPU_GROUP_LOCK();

    LIST_FOREACH(&cpu_group_list, iter)
        {
        cpu_group = LIST_ENTRY(iter, sched_cpu_group_t, cpu_group_node);

        if (cpu_group &&
            !CPU_CMP(0, CONFIG_NR_CPUS, &cpu_group->cpu_set, cpu_set))
            break;
        
        cpu_group = NULL;
        }
        
    /* Unlock the cpu group list */
    SCHED_CPU_GROUP_UNLOCK();

    return cpu_group;
    }

/* Add a cpu_group with the spcified cpu_set */
sched_cpu_group_t * sched_cpu_group_add 
    (
    cpu_set_t * cpu_set
    )
    {
    sched_cpu_group_t * cpu_group = NULL;

    /* Lock the cpu group list */
    SCHED_CPU_GROUP_LOCK();

    LIST_FOREACH(&cpu_group_list, iter)
        {
        cpu_group = LIST_ENTRY(iter, sched_cpu_group_t, cpu_group_node);

        if (cpu_group &&
            !CPU_CMP(0, CONFIG_NR_CPUS, &cpu_group->cpu_set, cpu_set))
            break;
        
        cpu_group = NULL;
        }

    if (cpu_group)
        {
        /* Unlock the cpu group list */
        SCHED_CPU_GROUP_UNLOCK();
        
        return cpu_group;
        }

    cpu_group = kmalloc(sizeof(sched_cpu_group_t));

    if (!cpu_group)
        {
        /* Unlock the cpu group list */
        SCHED_CPU_GROUP_UNLOCK();
        
        return NULL;
        }

    memset(cpu_group, 0, sizeof(sched_cpu_group_t));

    CPU_COPY(cpu_set, &cpu_group->cpu_set);
    
    list_init(&cpu_group->cpu_list);

    cpu_group->cpu_group_id = cpu_group_id_next++;
    
    cpu_group_cout++;
    
    /* Add the new cpu group to the cpu group list */
    list_append(&cpu_group_list, &cpu_group->cpu_group_node);
    
    /* Unlock the cpu group list */
    SCHED_CPU_GROUP_UNLOCK();

    return cpu_group;
    }

/* Remove a cpu_group with the spcified cpu_set */
void sched_cpu_group_remove
    (
    cpu_set_t * cpu_set
    )
    {
    sched_cpu_group_t * cpu_group = NULL;

    /* Lock the cpu group list */
    SCHED_CPU_GROUP_LOCK();

    LIST_FOREACH(&cpu_group_list, iter)
        {
        cpu_group = LIST_ENTRY(iter, sched_cpu_group_t, cpu_group_node);

        if (cpu_group &&
            !CPU_CMP(0, CONFIG_NR_CPUS, &cpu_group->cpu_set, cpu_set))
            {
            break;
            }
        
        cpu_group = NULL;
        }
    
    if (cpu_group)   
        {
        cpu_group_cout--;
        
        list_remove(&cpu_group->cpu_group_node);
        }
    
    /* Unlock the cpu group list */
    SCHED_CPU_GROUP_UNLOCK();
    }

sched_thread_t * sched_cpu_group_find_best_thread 
    (
    sched_policy_t * policy,
    sched_cpu_t *    cpu,
    sched_thread_t * check
    )
    {
    sched_cpu_group_t * cpu_group;
    sched_runq_t * runq;
    sched_thread_t * best_thread;

    /* Lock the cpu group list */
    SCHED_CPU_GROUP_LOCK();

    LIST_FOREACH(&cpu_group_list, iter)
        {
        cpu_group = LIST_ENTRY(iter, sched_cpu_group_t, cpu_group_node);

        /*
         * A cpu may belong to multiple cpu_groups.
         * Check if this cpu belongs to the cpu_group.
         */
        if (cpu_group && CPU_ISSET(cpu->cpu_idx, &cpu_group->cpu_set))
            {
            /* 
             * Find the owner cpu_group that this cpu belongs to. 
             * A cpu may belong to several cpu_groups.
             */
            runq = policy->get_cpu_group_runq(cpu_group);

            /* Check if this cpu_group has a better thread to run */
            if (policy->preemption_check(runq, check))
                {
                best_thread = policy->thread_dequeue(runq);

                if (best_thread) 
                    {
                    /* Unlock the cpu group list */
                    SCHED_CPU_GROUP_UNLOCK();
                    
                    return best_thread;
                    }
                }
            }
        }
        
    /* Unlock the cpu group list */
    SCHED_CPU_GROUP_UNLOCK();

    return NULL;
    }

